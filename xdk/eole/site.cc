#include "xdk/eole/site.h"
#include "absl/memory/memory.h"
#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "absl/types/optional.h"
#include "xdk/eole/filesystem.h"
#include "xdk/eole/path.h"
#include "xdk/jude/do.h"
#include "xdk/lua/rawfield.h"
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
constexpr char kPath[] = "path";

int getblock(lua_State *L) {
  if (lua_gettop(L) == 0) {
    // TODO: find a way to share that constant with jude.
    lua_pushstring(L, "_");
  }
  luaL_checkstring(L, -1);
  lua_pushvalue(L, lua_upvalueindex(1));
  lua_insert(L, -2);
  lua_rawget(L, -2);
  return 1;
}

int extend(lua_State *L) {
  auto *next = reinterpret_cast<absl::optional<std::string> *>(
      lua_touserdata(L, lua_upvalueindex(1)));
  *next = luaL_checkstring(L, -1);
  return 0;
}

struct Builder {
  std::string src_path;
  std::string dst_path;

  void BuildDirectory(lua_State *L, const std::string &rel_path) const
      throw(std::system_error) {
    const std::string path = Path::Join(src_path, rel_path);
    // Create a sandbox around the table at the top of the stack,
    // with path available as a variable.
    lua::newsandbox(L, -1);
    lua::rawsetfieldstring(L, -2, kPath, rel_path.c_str());
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
    // Then process every file in unspecified order intentionally so that
    // there is no temptation to rely on order, since we may parallelize
    // later.
    for (const auto &file : listing.files) {
      BuildFile(L, Path::Join(rel_path, file));
    }
    for (const auto &directory : listing.directories) {
      BuildDirectory(L, Path::Join(rel_path, directory));
    }
    lua_pop(L, 1);
  }

  void BuildFile(lua_State *L, const std::string &rel_path) const {
    // On the stack should be the sandbox for the directory.  Add a sandbox
    // around it, with path available as a variable (thus overriding that of
    // directory).
    lua::newsandbox(L, -1);
    lua::rawsetfieldstring(L, -1, kPath, rel_path.c_str());
    // Start with an empty set of blocks.
    lua_newtable(L);
    for (absl::optional<std::string> next = rel_path; next;) {
      // Read the file first, so `next` is no longer needed.
      const auto source = std::make_pair(ReadFile(*next), *next);
      // Set getblock to use the blocks table.
      lua_pushcclosure(L, getblock, 1);
      lua::rawsetfield(L, -2, "getblock");
      // Reset `next` and set `extend` to be able to set it again.
      next = absl::nullopt;
      lua_pushlightuserdata(L, &next);
      lua_pushcclosure(L, extend, 1);
      lua::rawsetfield(L, -2, "extend");
      // Execute the source, which may or not reset `next`.  After, the
      // stack has again the sandbox and the (new) blocks table), so the loop
      // can continue if needed.
      if (int error =
              jude::dostring(L, source.first.data(), source.first.size(),
                             source.second.c_str())) {
        throw std::runtime_error(lua_tostring(L, -1));
      }
    }
    // Get the unamed block and save it.
    lua_getfield(L, -1, kDstName);
    if (lua_isstring(L, -1)) {
      const std::string path = Path::Join(dst_path, rel_path);
      Filesystem::Mkdir(Path::Dirname(path));
      Filesystem::Write(path, lua_tostring(L, -1));
    }
    lua_pop(L, 3); // sandbox, blocks, unammed block.
  }

  std::string ReadFile(const std::string &rel_path) const {
    return Filesystem::Read(Path::Join(src_path, rel_path));
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
