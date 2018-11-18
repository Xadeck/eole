#include "xdk/eole/site.h"

#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/strip.h"
#include "xdk/eole/filesystem.h"
#include "xdk/eole/fixture.h"
#include "xdk/lua/matchers.h"
#include "xdk/lua/stack.h"
#include "xdk/lua/state.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {
namespace {
using lua::Element;
using lua::HasField;
using lua::IsNumber;
using ::testing::_;
using ::testing::StrEq;

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

TEST_F(SiteTest, FilesAreBuilt) {
  // Execute a site that sets a global global variable and a local variable.
  fixture_.AddFile("three.md", R"(three = {{1+2}})");
  fixture_.AddFile("four.md", R"(four = {{2+2}})");
  Site::Build(L, fixture_.Dirpath());
  // Check that files have been build in the fixture.
  EXPECT_THAT(fixture_, HasFile("_/three.md", "three = 3"));
  EXPECT_THAT(fixture_, HasFile("_/four.md", "four = 4"));
}

TEST_F(SiteTest, SandboxMechanismWorks) {
  // Global environment.
  lua_pushnumber(L, 1);
  lua_setfield(L, -2, "version");
  // Site configuration. It has access to global environment.  All variables
  // set here are passed to pages, but do not pollute the global table.
  fixture_.AddFile("site.lua", R"LUA(
    site = "Site "..path.." for test"
  )LUA");
  fixture_.AddFile("subdir/site.lua", R"LUA(
    banner = path.."@"..version
  )LUA");
  // Pages have access to global environment and site variables, but they can't
  // pollute any of them. If it were the case - since both examples below try
  // to - one of the two files would be corrupted (depending on execution order
  // which has no guarantee).
  fixture_.AddFile("subdir/fileA.md", R"LUA(
    # {{ site }}
    I wish version was 3 instead of {{version}}
    and banner was three instead of {{banner}}
    {%- version = 3 banner='three' -%})LUA");
  fixture_.AddFile("subdir/fileB.md", R"LUA(
    # {{ site }}
    I wish version was 7 instead of {{version}}
    and banner was seven instead of {{banner}}
    {%- version = 4 banner='four' -%})LUA");
  // Pages also have `path` available, and can't nil it
  fixture_.AddFile("fileC.md", "path = {{path}}");

  Site::Build(L, fixture_.Dirpath());
  // Check that global environment was not polluted.
  EXPECT_THAT(lua::Stack::Element(L, -1), HasField("version", IsNumber(1)));
  // Check that files were written with expected content.
  EXPECT_THAT(Filesystem::Read(fixture_.Path("_/subdir/fileA.md")), StrEq(R"(
    # Site . for test
    I wish version was 3 instead of 1.0
    and banner was three instead of ./subdir@1.0)"));
  EXPECT_THAT(Filesystem::Read(fixture_.Path("_/subdir/fileB.md")), StrEq(R"(
    # Site . for test
    I wish version was 7 instead of 1.0
    and banner was seven instead of ./subdir@1.0)"));

  EXPECT_THAT(Filesystem::Read(fixture_.Path("_/fileC.md")),
              StrEq("path = ./fileC.md"));
}

TEST_F(SiteTest, ExtendWorks) {
  fixture_.AddFile("layouts/final.html", R"(<html>
<body>{{ getblock() }}</body>
</html>)");
  fixture_.AddFile("layouts/intermediate.html", R"(
{%  extend('layouts/final.html') %}
<h1>Title</h1>
<div>{{ getblock() }}</div>
</html>)");
  fixture_.AddFile("posts/page.md", "{% extend('layouts/intermediate.html') %}"
                                    "This is the page.");
  Site::Build(L, fixture_.Dirpath());
  EXPECT_THAT(Filesystem::Read(fixture_.Path("_/posts/page.md")),
              StrEq(R"(<html>
<body>

<h1>Title</h1>
<div>This is the page.</div>
</html></body>
</html>)"));
}

} // namespace
} // namespace eole
} // namespace xdk
