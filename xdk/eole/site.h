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
  static void Build(lua_State *L,
                    absl::string_view root_path) throw(std::system_error);
};

} // namespace eole
} // namespace xdk

#endif
