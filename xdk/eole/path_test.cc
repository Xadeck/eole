#include "xdk/eole/path.h"

#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {
namespace {

TEST(PathTest, JoinWorks) {
  EXPECT_EQ(Path::Join("foo"), "foo");
  EXPECT_EQ(Path::Join("foo", "bar"), "foo/bar");
  EXPECT_EQ(Path::Join("foo", "bar", "baz"), "foo/bar/baz");
}

TEST(PathTest, EmptyStringsAreHandledCorrectly) {
  EXPECT_EQ(Path::Join("foo", ""), "foo");
  EXPECT_EQ(Path::Join("", "foo"), "foo");
  EXPECT_EQ(Path::Join("foo", "", "baz"), "foo/baz");
}

TEST(PathTest, ExtraSeparatorsAreHandled) {
  EXPECT_EQ(Path::Join("foo//", "bar"), "foo/bar");
  EXPECT_EQ(Path::Join("foo", "//bar"), "foo/bar");
}

TEST(PathTest, DirnameWorks) {
  EXPECT_EQ(Path::Dirname(""), ".");
  EXPECT_EQ(Path::Dirname("/"), "/");
  EXPECT_EQ(Path::Dirname("/foo"), "/");
  EXPECT_EQ(Path::Dirname("/foo/bar"), "/foo");
  EXPECT_EQ(Path::Dirname("/foo/bar/"), "/foo");
  EXPECT_EQ(Path::Dirname("baz"), ".");
}

TEST(PathTest, BasenameWorks) {
  EXPECT_EQ(Path::Basename(""), "");
  EXPECT_EQ(Path::Basename("/"), "/");
  EXPECT_EQ(Path::Basename("/foo"), "foo");
  EXPECT_EQ(Path::Basename("/foo/bar"), "bar");
  EXPECT_EQ(Path::Basename("/foo/bar/"), "bar");
  EXPECT_EQ(Path::Basename("baz"), "baz");
}

} // namespace
} // namespace eole
} // namespace xdk
