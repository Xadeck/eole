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

} // namespace
} // namespace eole
} // namespace xdk
