#include "xdk/eole/project.h"
#include "xdk/lua/matchers.h"
#include "xdk/lua/stack.h"

#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/strip.h"
#include "xdk/eole/fixture.h"
#include "xdk/lua/state.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {
namespace {
using lua::Element;
using lua::HasField;
using lua::IsNil;
using lua::IsNumber;
using lua::IsString;
using ::testing::_;
using ::testing::ElementsAre;

class ProjectTest : public ::testing::Test {
protected:
  void SetUp() override { lua_newtable(L); }

  lua::State L;
  Fixture fixture_;
};

TEST_F(ProjectTest, Works) {
  fixture_.AddFile("pages/TODO.txt", "");
  fixture_.AddFile("pages/about.md", "");
  fixture_.AddFile("posts/absl.md", "");
  fixture_.AddFile("posts/config.lua", "");
  fixture_.AddFile("posts/gtest.md", "");
  fixture_.AddFile("posts/lua.md", "");
  Project project(L, fixture_.Dirpath());
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
  EXPECT_THAT(content, ElementsAre("dir:",                 //
                                   "dir:/posts",           //
                                   "file:/posts/gtest.md", //
                                   "file:/posts/absl.md",  //
                                   "file:/posts/lua.md",   //
                                   "dir:/pages",           //
                                   "file:/pages/TODO.txt", //
                                   "file:/pages/about.md"  //
                                   ));
  ASSERT_THAT(lua_gettop(L), 1);
}

TEST_F(ProjectTest, ExceptionIsThrownIfNotExisting) {
  ASSERT_THROW(Project project(L, fixture_.Path("nonexistent")),
               std::runtime_error);
  ASSERT_EQ(lua_gettop(L), 2);
}

TEST_F(ProjectTest, ConfigAreExecuted) {
  // Define a global variable.
  lua_newtable(L);
  lua_setfield(L, -2, "project");
  // Execute a config that sets a global global variable and a local variable.
  fixture_.AddFile("config.lua", R"LUA(project.name = "Good" name = "Bad")LUA");
  Project project(L, fixture_.Dirpath());
  // Check that `project` is indeed set but not `name`.
  EXPECT_THAT(lua::Stack::Element(L, -1),
              HasField("project", HasField("name", IsString("Good"))));
  EXPECT_THAT(lua::Stack::Element(L, -1), HasField("name", IsNil()));
}

} // namespace
} // namespace eole
} // namespace xdk
