#include "xdk/eole/fixture.h"

#include "xdk/eole/filesystem.h"
#include "xdk/eole/path.h"
#include "gmock/gmock.h"
#include "gtest/gtest-spi.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {
namespace {
using ::testing::_;
using ::testing::EndsWith;
using ::testing::HasSubstr;

class FixtureTest : public ::testing::Test {
public:
  Fixture fixture_;
};

TEST_F(FixtureTest, ContainsCurrentTestInfo) {
  EXPECT_THAT(fixture_.Dirpath(),
              EndsWith(Path::Join("FixtureTest", "ContainsCurrentTestInfo")));
}

TEST_F(FixtureTest, CanAddFile) {
  const std::string relative_filepath = Path::Join("foo", "bar", "baz.txt");
  fixture_.AddFile(relative_filepath, "some content");

  EXPECT_EQ(Filesystem::Read(fixture_.Path(relative_filepath)), "some content");
}

TEST_F(FixtureTest, HasFileSuccess) {
  fixture_.AddFile(Path::Join("foo", "bar.txt"), "the content");

  EXPECT_THAT(fixture_, Not(HasFile("bar.txt", "not the content")));
  EXPECT_THAT(fixture_, HasFile(Path::Join("foo", "bar.txt"), "the content"));
  EXPECT_THAT(fixture_,
              HasFile(Path::Join("foo", "bar.txt"), HasSubstr("content")));
  EXPECT_THAT(fixture_, HasFile(Path::Join("foo", "bar.txt"), _));
  EXPECT_THAT(fixture_, Not(HasFile(Path::Join("foo", "baz.txt"), _)));
  EXPECT_THAT(fixture_, Not(HasFile("foo", _)));
}

TEST_F(FixtureTest, HasFileFailure) {
  const std::string &foo_bar = Path::Join("foo", "bar.txt");
  const std::string &foo_baz = Path::Join("foo", "baz.txt");
  fixture_.AddFile(foo_bar, "the content");

  // Test matchers on content.
  EXPECT_NONFATAL_FAILURE(
      EXPECT_THAT(fixture_, HasFile(foo_bar, "not the content")),
      R"(Expected: has file 'foo/bar.txt' whose content is equal to "not the content")");
  EXPECT_NONFATAL_FAILURE(
      EXPECT_THAT(fixture_, Not(HasFile(foo_bar, "the content"))),
      R"(Expected: hasn't file 'foo/bar.txt' or its content isn't equal to "the content")");
  // Test matcher on non existing file.
  EXPECT_NONFATAL_FAILURE(
      EXPECT_THAT(fixture_, HasFile(foo_baz, "the content")),
      R"(Actual: Fixture($TEST_TMPDIR/FixtureTest/HasFileFailure) (of type xdk::eole::Fixture), where 'foo/baz.txt' doesn't exist)");
  // Test matcher on existing directory.
  EXPECT_NONFATAL_FAILURE(
      EXPECT_THAT(fixture_, HasFile("foo", _)),
      R"(Actual: Fixture($TEST_TMPDIR/FixtureTest/HasFileFailure) (of type xdk::eole::Fixture), where 'foo' exists but isn't a regular file)");
}

} // namespace
} // namespace eole
} // namespace xdk
