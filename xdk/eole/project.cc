#include "xdk/eole/project.h"
#include "xdk/eole/path.h"

#include "absl/memory/memory.h"
#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include <cstdlib>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

namespace xdk {
namespace eole {
namespace {
void BuildDirectory(Project::Directory *directory) throw(std::system_error) {
  DIR *dir = opendir(directory->path.c_str());
  if (dir == nullptr) {
    throw std::system_error(errno, std::generic_category(),
                            absl::StrCat("opendir(", directory->path, ")"));
  }
  dirent *entry = nullptr;
  while ((entry = readdir(dir)) != nullptr) {
    const absl::string_view filename = entry->d_name;
    if (absl::StartsWith(filename, ".")) {
      continue;
    }
    const std::string filepath = Path::Join(directory->path, filename);
    struct stat filestat;
    if (stat(filepath.c_str(), &filestat) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              absl::StrCat("stat(", filepath, ")"));
    }
    if (S_ISDIR(filestat.st_mode)) {
      auto subdirectory = absl::make_unique<Project::Directory>(filepath);
      BuildDirectory(subdirectory.get());
      directory->directories.push_back(std::move(subdirectory));
      continue;
    }
    if (S_ISREG(filestat.st_mode)) {
      if (filename == "config.lua") {
        directory->config = absl::make_optional<std::string>(filepath);
        continue;
      }
      directory->files.emplace_back(filepath);
    }
  }
  if (closedir(dir) == -1) {
    throw std::system_error(errno, std::generic_category(),
                            absl::StrCat("close(", directory->path, ")"));
  }
}

} // namespace

Project::Project(absl::string_view rootpath) throw(std::system_error)
    : root_(absl::make_unique<Directory>(rootpath)) {
  BuildDirectory(root_.get());
}

} // namespace eole
} // namespace xdk
