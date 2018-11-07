#include "xdk/eole/filesystem.h"

#include <fstream>
#include <iostream>

#include "xdk/eole/path.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {
namespace {

TEST(FilesystemTest, WorksRecursively) {
  const std::string &dirpath =
      Path::Join(std::getenv("TEST_TMPDIR"), "dir/and/subdir");
  Filesystem::Mkdir(dirpath);
  // Check that a file can be written.
  const auto &filepath = Path::Join(dirpath, "file.txt");
  {
    std::fstream s(filepath, s.binary | s.trunc | s.out);
    ASSERT_TRUE(s.is_open());
    s << "some content\n";
  }
}

} // namespace
} // namespace eole
} // namespace xdk
