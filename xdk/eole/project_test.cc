#include "xdk/eole/project.h"

#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/strip.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {
namespace {
using ::testing::ElementsAre;

class MarkdownFileGenerator final : public Project::File::Generator {
public:
  const char *Name() const final { return "MD"; }
  bool CanGenerate(absl::string_view filepath) const final {
    return absl::EndsWith(filepath, ".md");
  }
};

class ProjectTest : public ::testing::Test {
protected:
};

TEST_F(ProjectTest, Works) {
  std::string root =
      absl::StrCat(std::getenv("TEST_SRCDIR"), "/xdk_eole/xdk/eole/testdata");
  MarkdownFileGenerator markdown_file_generator;
  Project project =
      Project::Builder(root).AddFileGenerator(&markdown_file_generator).Build();
  // Use a local tree traversal to get the list of directories and files in the
  // project, in depth first manner. There is no need to add traversing function
  // to the Project::Directory class. It also allows to have expectations in the
  // traversal, for example that path are absolute to the root path.
  std::vector<std::string> content;
  std::function<void(const Project::Directory *)> traverse =
      [&project, &content, &traverse](const Project::Directory *directory) {
        absl::string_view dirpath = directory->path;
        EXPECT_TRUE(absl::ConsumePrefix(&dirpath, project.root()->path));
        content.push_back(absl::StrCat("dir:", dirpath));
        if (directory->config) {
          absl::string_view configpath = *directory->config;
          EXPECT_TRUE(absl::ConsumePrefix(&configpath, project.root()->path));
          content.push_back(absl::StrCat("config:", configpath));
        }
        for (const auto &file : directory->files) {
          absl::string_view filepath = file.path;
          EXPECT_TRUE(absl::ConsumePrefix(&filepath, project.root()->path));
          content.push_back(
              absl::StrCat("file[", file.generator->Name(), "]:", filepath));
        }
        for (const auto &subdirectory : directory->directories) {
          traverse(subdirectory.get());
        }
      };
  traverse(project.root());
  EXPECT_THAT(content, ElementsAre("dir:",                     //
                                   "dir:/posts",               //
                                   "config:/posts/config.lua", //
                                   "file[MD]:/posts/gtest.md", //
                                   "file[MD]:/posts/absl.md",  //
                                   "file[MD]:/posts/lua.md",   //
                                   "dir:/pages",               //
                                   "file[MD]:/pages/about.md"  //
                                   ));
}

} // namespace
} // namespace eole
} // namespace xdk
