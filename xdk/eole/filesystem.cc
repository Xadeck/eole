#include "xdk/eole/filesystem.h"
#include "absl/strings/match.h"
#include "xdk/eole/path.h"
#include <cstdlib>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <sys/stat.h>

namespace xdk {
namespace eole {

void Filesystem::Mkdir(absl::string_view path) throw(std::system_error) {
  if (path == "/" || path.empty()) {
    return;
  }
  Mkdir(Path::Dirname(path));
  if (mkdir(std::string(path).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) ==
      -1) {
    if (errno != EEXIST) {
      throw std::system_error(errno, std::generic_category(),
                              absl::StrCat("mkdir(", path, ")"));
    }
  }
}

Filesystem::Listing
Filesystem::LsDir(absl::string_view path) throw(std::system_error) {
  DIR *const dir = opendir(std::string(path).c_str());
  if (dir == nullptr) {
    throw std::system_error(errno, std::generic_category(),
                            absl::StrCat("opendir(", path, ")"));
  }
  Listing listing;
  dirent *entry = nullptr;
  while ((entry = readdir(dir)) != nullptr) {
    const absl::string_view entry_name = entry->d_name;
    if (absl::StartsWith(entry_name, ".")) {
      continue;
    }
    const std::string entry_path = Path::Join(path, entry_name);
    struct stat stat;
    if (::stat(entry_path.c_str(), &stat) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              absl::StrCat("stat(", entry_path, ")"));
    }
    if (S_ISDIR(stat.st_mode)) {
      listing.directories.emplace(entry_name);
      continue;
    }
    if (S_ISREG(stat.st_mode)) {
      listing.files.emplace(entry_name);
      continue;
    }
  }
  if (closedir(dir) == -1) {
    throw std::system_error(errno, std::generic_category(),
                            absl::StrCat("close(", path, ")"));
  }
  return listing;
}

void Filesystem::Write(const std::string &filepath, absl::string_view content) {
  std::ofstream(filepath) << content;
}

// TODO: throw an exception if does not exist.
std::string Filesystem::Read(const std::string &filepath) {
  std::ifstream ifs(filepath);
  return std::string(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());
}

} // namespace eole
} // namespace xdk
