#include "xdk/eole/fixture.h"

#include <fstream>

#include "xdk/eole/path.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {
namespace {
using ::testing::EndsWith;

class FixtureTest : public ::testing::Test {
public:
  Fixture fixture_;
};

TEST_F(FixtureTest, ContainsCurrentTestInfo) {
  EXPECT_THAT(fixture_.Dirpath(),
              EndsWith(Path::Join("FixtureTest", "ContainsCurrentTestInfo")));
}

TEST_F(FixtureTest, CanAddFile) {
  constexpr char relative_filepath[] = "foo/bar/baz.txt";
  fixture_.AddFile(relative_filepath, "some content");

  std::ifstream ifs(fixture_.Path(relative_filepath));
  std::string content{std::istreambuf_iterator<char>(ifs),
                      std::istreambuf_iterator<char>()};
  EXPECT_EQ(content, "some content");
}
} // namespace
} // namespace eole
} // namespace xdk
