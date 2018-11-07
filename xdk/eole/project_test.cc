#include "xdk/eole/project.h"

#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/strip.h"
#include "xdk/eole/fixture.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {
namespace {
using ::testing::ElementsAre;

class ProjectTest : public ::testing::Test {
protected:
  Fixture fixture_;
};

TEST_F(ProjectTest, Works) {
  fixture_.AddFile("pages/TODO.txt", "");
  fixture_.AddFile("pages/about.md", "");
  fixture_.AddFile("posts/absl.md", "");
  fixture_.AddFile("posts/config.lua", "");
  fixture_.AddFile("posts/gtest.md", "");
  fixture_.AddFile("posts/lua.md", "");
  Project project(fixture_.Dirpath());
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
          content.push_back(absl::StrCat("file:", filepath));
        }
        for (const auto &subdirectory : directory->directories) {
          traverse(subdirectory.get());
        }
      };
  traverse(project.root());
  EXPECT_THAT(content, ElementsAre("dir:",                     //
                                   "dir:/posts",               //
                                   "config:/posts/config.lua", //
                                   "file:/posts/gtest.md",     //
                                   "file:/posts/absl.md",      //
                                   "file:/posts/lua.md",       //
                                   "dir:/pages",               //
                                   "file:/pages/TODO.txt",     //
                                   "file:/pages/about.md"      //
                                   ));
}

TEST_F(ProjectTest, ExceptionIsThrownIfNotExisting) {
  ASSERT_THROW(Project project(fixture_.Filepath("nonexistent")),
               std::runtime_error);
}

} // namespace
} // namespace eole
} // namespace xdk
