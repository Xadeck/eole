#ifndef XDK_EOLE_SITE_H
#define XDK_EOLE_SITE_H

#include "absl/strings/string_view.h"
#include "xdk/lua/lua.hpp"
#include <memory>
#include <string>
#include <system_error>
#include <vector>

namespace xdk {
namespace eole {

class Site {
public:
  // Evaluate every file as a jude function and output the result under _
  // subdirectory.
  static void Build(lua_State *L,
                    absl::string_view root_path) throw(std::system_error,
                                                       std::runtime_error);
};

} // namespace eole
} // namespace xdk

#endif
