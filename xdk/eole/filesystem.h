#ifndef XDK_EOLE_FILESYSTEM_H
#define XDK_EOLE_FILESYSTEM_H

#include "absl/strings/string_view.h"
#include <set>
#include <string>
#include <system_error>

namespace xdk {
namespace eole {

class Filesystem {
public:
  struct Listing {
    std::set<std::string> directories;
    std::set<std::string> files;
  };
  static void Mkdir(absl::string_view dirpath) throw(std::system_error);
  // Follows symlinks. Return sorted listing.
  static Listing LsDir(absl::string_view dirpath) throw(std::system_error);
};

} // namespace eole
} // namespace xdk

#endif
