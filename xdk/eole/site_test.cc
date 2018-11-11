#include "xdk/eole/site.h"
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

class SiteTest : public ::testing::Test {
protected:
  void SetUp() override { lua_newtable(L); }

  lua::State L;
  Fixture fixture_;
};

TEST_F(SiteTest, ExceptionIsThrownIfNotExisting) {
  ASSERT_THROW(Site::Build(L, fixture_.Path("nonexistent")),
               std::runtime_error);
  ASSERT_EQ(lua_gettop(L), 2);
}

TEST_F(SiteTest, ConfigAreExecuted) {
  // Define a global variable.
  lua_newtable(L);
  lua_setfield(L, -2, "site");
  // Execute a site that sets a global global variable and a local variable.
  fixture_.AddFile("site.lua", R"LUA(site.name = "Good" name = "Bad")LUA");
  Site::Build(L, fixture_.Dirpath());
  // Check that `site` is indeed set but not `name`.
  EXPECT_THAT(lua::Stack::Element(L, -1),
              HasField("site", HasField("name", IsString("Good"))));
  EXPECT_THAT(lua::Stack::Element(L, -1), HasField("name", IsNil()));
}

TEST_F(SiteTest, FilesAreBuilt) {
  // Execute a site that sets a global global variable and a local variable.
  fixture_.AddFile("three.md", R"(three = {{1+2}})");
  fixture_.AddFile("four.md", R"(four = {{2+2}})");
  Site::Build(L, fixture_.Dirpath());
  // Check that files have been build in the fixture.
  EXPECT_THAT(fixture_, HasFile("_/three.md", "three = 3"));
  EXPECT_THAT(fixture_, HasFile("_/four.md", "four = 4"));
}
} // namespace
} // namespace eole
} // namespace xdk
