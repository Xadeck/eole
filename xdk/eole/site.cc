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
    const std::string path = Path::Join(src_path, rel_path);
    // Create a sandbox around the table at the top of the stack,
    // with path available as a variable.
    lua::newsandbox(L, -1);
    lua_pushliteral(L, "path");
    lua_pushstring(L, rel_path.c_str());
    lua_rawset(L, -3);
    // Get list of directories and files first.
    const auto listing = Filesystem::LsDir(path);
    // If a site file exists, then execute it, stopping upon errors.
    // The execution thus has access to path and will populate sandbox.
    if (listing.files.find(kSiteName) != listing.files.end()) {
      // TODO: pass the relname for error messages.
      if (int error = luaL_loadfile(L, Path::Join(path, kSiteName).c_str())) {
        throw std::runtime_error(lua_tostring(L, -1));
      }
      lua_pushvalue(L, -2);
      lua_setupvalue(L, -2, 1);
      if (int error = lua_pcall(L, 0, 0, 0)) {
        throw std::runtime_error(lua_tostring(L, -1));
      }
    }
    // Then process every file in unspecified order intentionally so that there
    // is no temptation to rely on order, since we may parallelize later.
    for (const auto &file : listing.files) {
      BuildFile(L, Path::Join(rel_path, file));
    }
    for (const auto &directory : listing.directories) {
      BuildDirectory(L, Path::Join(rel_path, directory));
    }
    lua_pop(L, 1);
  }

  void BuildFile(lua_State *L, const std::string &rel_path) const {
    const std::string source = Filesystem::Read(Path::Join(src_path, rel_path));
    // On the stack should be the sandbox for the directory.  Add a sandbox
    // around it, with path available as a variable (thus overriding that of
    // directory).
    lua::newsandbox(L, -1);
    lua_pushliteral(L, "path");
    lua_pushstring(L, rel_path.c_str());
    lua_rawset(L, -3);
    if (int error =
            jude::dostring(L, source.data(), source.size(), rel_path.c_str())) {
      throw std::runtime_error(lua_tostring(L, -1));
    }
    // On the stack there is the sandbox and the blocks table.
    // Get the unamed block and save it.
    lua_getfield(L, -1, kDstName);
    if (lua_isstring(L, -1)) {
      const std::string path = Path::Join(dst_path, rel_path);
      Filesystem::Mkdir(Path::Dirname(path));
      Filesystem::Write(path, lua_tostring(L, -1));
    }
    lua_pop(L, 3); // sandbox, blocks, unammed block.
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
