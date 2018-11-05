#include "xdk/eole/project.h"
#include "xdk/eole/path.h"

#include "absl/memory/memory.h"
#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include <cstdlib>
#include <dirent.h>
#include <sys/stat.h>

namespace xdk {
namespace eole {
namespace {
void BuildDirectory(Project::Directory *directory) {
  DIR *dir = opendir(directory->path.c_str());
  dirent *entry = nullptr;
  while ((entry = readdir(dir)) != nullptr) {
    const absl::string_view filename = entry->d_name;
    if (filename == "." || filename == "..") {
      continue;
    }
    const std::string filepath = Path::Join(directory->path, filename);
    struct stat filestat;
    (void)stat(filepath.c_str(), &filestat);
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
  closedir(dir);
}

} // namespace

Project::Project(absl::string_view rootpath)
    : root_(absl::make_unique<Directory>(rootpath)) {
  BuildDirectory(root_.get());
}

} // namespace eole
} // namespace xdk
