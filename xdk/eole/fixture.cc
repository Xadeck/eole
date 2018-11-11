#include "xdk/eole/fixture.h"

#include <dirent.h>
#include <fstream>

#include "xdk/eole/filesystem.h"
#include "xdk/eole/path.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {

using ::testing::MakeMatcher;
using ::testing::Matcher;
using ::testing::MatcherInterface;
using ::testing::MatchResultListener;
using ::testing::UnitTest;

namespace {
struct HasFileMatcher final
    : public ::testing::MatcherInterface<const Fixture &> {
  const std::string relative_path;
  const Matcher<absl::string_view> matcher;

  explicit HasFileMatcher(absl::string_view relative_path,
                          const Matcher<absl::string_view> &matcher)
      : relative_path(relative_path), matcher(matcher) {}

  void DescribeTo(std::ostream *os) const final {
    *os << "has file '" << relative_path << "' whose content ";
    matcher.DescribeTo(os);
  }

  void DescribeNegationTo(std::ostream *os) const final {
    *os << "hasn't file '" << relative_path << "' or its content ";
    matcher.DescribeNegationTo(os);
  }

  bool MatchAndExplain(const Fixture &fixture,
                       MatchResultListener *result_listener) const final {
    const std::string path = fixture.Path(relative_path);
    struct stat stat;
    if (::stat(path.c_str(), &stat) != 0) {
      *result_listener << "where '" << relative_path << "' doesn't exist";
      return false;
    }
    if (!S_ISREG(stat.st_mode)) {
      *result_listener << "where '" << relative_path
                       << "' exists but isn't a regular file";
      return false;
    }

    std::ifstream ifs(path);
    const std::string content{std::istreambuf_iterator<char>(ifs),
                              std::istreambuf_iterator<char>()};
    return matcher.MatchAndExplain(content, result_listener);
  }
};
} // namespace

Fixture::Fixture()
    : name_(Path::Join(
          UnitTest::GetInstance()->current_test_info()->test_case_name(),
          UnitTest::GetInstance()->current_test_info()->name())),
      dirpath_(Path::Join(std::getenv("TEST_TMPDIR"), name_)) {}

std::string Fixture::Path(absl::string_view relative_filepath) const {
  return Path::Join(dirpath_, relative_filepath);
}

void Fixture::AddFile(absl::string_view relative_filepath,
                      absl::string_view content) const {
  const auto filepath = Path(relative_filepath);
  Filesystem::Mkdir(Path::Dirname(filepath));
  std::fstream fs(filepath, fs.binary | fs.trunc | fs.out);
  fs << content;
}

std::ostream &operator<<(std::ostream &os, const Fixture &fixture) {
  return os << "Fixture(" << Path::Join("$TEST_TMPDIR", fixture.name_) << ")";
}

Matcher<const Fixture &> HasFile(absl::string_view relative_path,
                                 Matcher<absl::string_view> matcher) {
  return MakeMatcher(new HasFileMatcher(relative_path, matcher));
}

} // namespace eole
} // namespace xdk
