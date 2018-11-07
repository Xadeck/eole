#include "xdk/eole/fixture.h"

#include <fstream>

#include "xdk/eole/filesystem.h"
#include "xdk/eole/path.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {

using ::testing::UnitTest;

Fixture::Fixture()
    : dirpath_(Path::Join(
          std::getenv("TEST_TMPDIR"),
          UnitTest::GetInstance()->current_test_info()->test_case_name(),
          UnitTest::GetInstance()->current_test_info()->name())) {}

std::string Fixture::Filepath(absl::string_view relative_filepath) const {
  return Path::Join(dirpath_, relative_filepath);
}

void Fixture::AddFile(absl::string_view relative_filepath,
                      absl::string_view content) const {
  const auto filepath = Filepath(relative_filepath);
  Mkdir(Path::Dirname(filepath));
  std::fstream fs(filepath, fs.binary | fs.trunc | fs.out);
  fs << content;
}

} // namespace eole
} // namespace xdk
