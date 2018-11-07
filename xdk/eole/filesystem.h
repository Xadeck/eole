#ifndef XDK_EOLE_FILESYSTEM_H
#define XDK_EOLE_FILESYSTEM_H

#include "absl/strings/string_view.h"
#include <system_error>

namespace xdk {
namespace eole {

void Mkdir(absl::string_view dirpath) throw(std::system_error);

} // namespace eole
} // namespace xdk

#endif
