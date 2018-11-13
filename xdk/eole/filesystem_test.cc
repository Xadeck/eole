#include "xdk/eole/filesystem.h"

#include <fstream>
#include <iostream>

#include "xdk/eole/path.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace xdk {
namespace eole {
namespace {
using ::testing::UnitTest;
using ::testing::UnorderedElementsAre;

class FilesystemTest : public ::testing::Test {
public:
  const std::string tmp_ =
      Path::Join(std::getenv("TEST_TMPDIR"),
                 UnitTest::GetInstance()->current_test_info()->name());
};

TEST_F(FilesystemTest, MkdirWorksRecursively) {
  const std::string &dirpath = Path::Join(tmp_, "dir", "and", "subdir");
  Filesystem::Mkdir(dirpath);
  // Check that a file can be written.
  const auto &filepath = Path::Join(dirpath, "file.txt");
  std::ofstream ofs(filepath);
  ASSERT_TRUE(ofs.is_open());
  ofs << "some content\n";
}

TEST_F(FilesystemTest, LsDirWorks) {
  // Add two directories and two files.
  Filesystem::Mkdir(Path::Join(tmp_, "subdirA"));
  Filesystem::Mkdir(Path::Join(tmp_, "subdirB"));
  std::ofstream(Path::Join(tmp_, "file1.txt")) << "_";
  std::ofstream(Path::Join(tmp_, "file2.txt")) << "_";
  // Also add a file in one subdirectory and a hidden file.
  std::ofstream(Path::Join(tmp_, "subdirA", "fileA1")) << "";
  std::ofstream(Path::Join(tmp_, ".hidden")) << "";
  // Check that only the two directories and two files are found.
  const auto listing = Filesystem::LsDir(tmp_);
  EXPECT_THAT(listing.directories, UnorderedElementsAre("subdirA", "subdirB"));
  EXPECT_THAT(listing.files, UnorderedElementsAre("file1.txt", "file2.txt"));
}

TEST_F(FilesystemTest, WriteReadWork) {
  Filesystem::Mkdir(tmp_);
  const std::string path = Path::Join(tmp_, "test.txt");
  Filesystem::Write(path, "some random content");
  EXPECT_THAT(Filesystem::Read(path), "some random content");
}

} // namespace
} // namespace eole
} // namespace xdk
