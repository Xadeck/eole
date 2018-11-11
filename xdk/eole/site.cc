#include "xdk/eole/site.h"
#include "absl/memory/memory.h"
#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "xdk/eole/filesystem.h"
#include "xdk/eole/path.h"
#include "xdk/jude/do.h"
#include "xdk/lua/sandbox.h"
#include <cstdlib>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <iostream>
#include <sys/stat.h>

namespace xdk {
namespace eole {
namespace {
constexpr char kDstName[] = "_";
constexpr char kSiteName[] = "site.lua";

struct Builder {
  std::string src_path;
  std::string dst_path;

  void BuildDirectory(lua_State *L, const std::string &rel_path) const
      throw(std::system_error) {
    // Create a sandbox around the table at the top of the stack.
    // Adds a `directory` table so that site file, if any, can surface data.
    lua::newsandbox(L, -1);
    lua_pushliteral(L, "site");
    lua_newtable(L);
    lua_rawset(L, -3);
    // If a site file exists, then execute it, stopping upon errors.
    // The execution will populate sandbox.
    const std::string site_rel_path = Path::Join(rel_path, kSiteName);
    const std::string site_path = Path::Join(src_path, site_rel_path);
    struct stat stat;
    if (::stat(site_path.c_str(), &stat) == 0 && S_ISREG(stat.st_mode)) {
      // TODO: pass the relname for error messages.
      if (int error = luaL_loadfile(L, site_path.c_str())) {
        throw std::runtime_error(lua_tostring(L, -1));
      }
      lua_pushvalue(L, -2);
      lua_setupvalue(L, -2, 1);
      if (int error = lua_pcall(L, 0, 0, 0)) {
        throw std::runtime_error(lua_tostring(L, -1));
      }
    }
    // Recursively traverse the directory.
    const std::string path = Path::Join(src_path, rel_path);
    DIR *const dir = opendir(path.c_str());
    if (dir == nullptr) {
      throw std::system_error(errno, std::generic_category(),
                              absl::StrCat("opendir(", path, ")"));
    }
    dirent *entry = nullptr;
    std::vector<lua_State *> threads;
    while ((entry = readdir(dir)) != nullptr) {
      const absl::string_view name = entry->d_name;
      if (absl::StartsWith(name, ".")) {
        continue;
      }
      const std::string entry_rel_path = Path::Join(rel_path, name);
      const std::string entry_path = Path::Join(src_path, entry_rel_path);
      struct stat stat;
      if (::stat(entry_path.c_str(), &stat) == -1) {
        throw std::system_error(errno, std::generic_category(),
                                absl::StrCat("stat(", entry_path, ")"));
      }
      if (S_ISDIR(stat.st_mode) && name != kDstName) {
        BuildDirectory(L, entry_rel_path);
        continue;
      }
      if (S_ISREG(stat.st_mode) && name != kSiteName) {
        BuildFile(L, entry_rel_path);
      }
    }
    if (closedir(dir) == -1) {
      throw std::system_error(errno, std::generic_category(),
                              absl::StrCat("close(", path, ")"));
    }
    lua_pop(L, 1);
  }

  void BuildFile(lua_State *L, const std::string &rel_path) const {
    const std::string source = Input(rel_path);
    // On the stack should be the sandbox for the directory.
    // It is readonly for dostring.
    if (int error =
            jude::dostring(L, source.data(), source.size(), rel_path.c_str())) {
      throw std::runtime_error(lua_tostring(L, -1));
    }
    // Get the unamed block and save it.
    lua_getfield(L, -1, kDstName);
    if (lua_isstring(L, -1)) {
      Output(rel_path, lua_tostring(L, -1));
    }
    lua_pop(L, 2);
  }

  std::string Input(absl::string_view rel_path) const {
    const std::string path = Path::Join(src_path, rel_path);
    std::ifstream ifs(path);
    return std::string{std::istreambuf_iterator<char>(ifs),
                       std::istreambuf_iterator<char>()};
  }

  void Output(absl::string_view rel_path, absl::string_view contents) const {
    const std::string path = Path::Join(dst_path, rel_path);
    Filesystem::Mkdir(Path::Dirname(path));
    std::fstream fs(path, fs.binary | fs.trunc | fs.out);
    fs << contents;
  }
};

} // namespace

void Site::Build(lua_State *L,
                 absl::string_view root_path) throw(std::system_error,
                                                    std::runtime_error) {
  Builder builder;
  builder.src_path = std::string(root_path);
  builder.dst_path = Path::Join(root_path, kDstName);
  builder.BuildDirectory(L, ".");
}

} // namespace eole
} // namespace xdk
