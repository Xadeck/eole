#include "xdk/eole/project.h"
#include "xdk/eole/path.h"
#include "xdk/lua/sandbox.h"
#include <iostream>

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
constexpr char kConfigFilename[] = "config.lua";

void BuildDirectory(lua_State *L,
                    Project::Directory *directory) throw(std::system_error) {
  // Create a sandbox around the table at the top of the stack.
  lua::newsandbox(L, -1);
  // If a config file exists, then execute it, stopping upon errors.
  std::string configpath = Path::Join(directory->path, kConfigFilename);
  struct stat stat;
  if (::stat(configpath.c_str(), &stat) == 0) {
    if (int error = luaL_loadfile(L, configpath.c_str())) {
      throw std::runtime_error(lua_tostring(L, -1));
    }
    lua_pushvalue(L, -2);
    lua_setupvalue(L, -2, 1);
    if (int error = lua_pcall(L, 0, 0, 0)) {
      throw std::runtime_error(lua_tostring(L, -1));
    }
  }
  // Traverse all files.
  DIR *dir = opendir(directory->path.c_str());
  if (dir == nullptr) {
    throw std::system_error(errno, std::generic_category(),
                            absl::StrCat("opendir(", directory->path, ")"));
  }
  dirent *entry = nullptr;
  while ((entry = readdir(dir)) != nullptr) {
    const absl::string_view name = entry->d_name;
    if (absl::StartsWith(name, ".")) {
      continue;
    }
    const std::string path = Path::Join(directory->path, name);
    struct stat stat;
    if (::stat(path.c_str(), &stat) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              absl::StrCat("stat(", path, ")"));
    }
    if (S_ISDIR(stat.st_mode)) {
      auto subdirectory = absl::make_unique<Project::Directory>(path);
      BuildDirectory(L, subdirectory.get());
      directory->directories.push_back(std::move(subdirectory));
      continue;
    }
    if (S_ISREG(stat.st_mode)) {
      if (name != kConfigFilename) {
        directory->files.emplace_back(path);
      }
    }
  }
  if (closedir(dir) == -1) {
    throw std::system_error(errno, std::generic_category(),
                            absl::StrCat("close(", directory->path, ")"));
  }
  lua_pop(L, 1);
}

} // namespace

Project::Project(lua_State *L,
                 absl::string_view rootpath) throw(std::system_error)
    : root_(absl::make_unique<Directory>(rootpath)) {
  BuildDirectory(L, root_.get());
}

} // namespace eole
} // namespace xdk
