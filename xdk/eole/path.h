#ifndef XDK_EOLE_PATH_H
#define XDK_EOLE_PATH_H

#include "absl/strings/str_cat.h"
#include <string>

namespace xdk {
namespace eole {

class Path {
public:
  static constexpr char Separator[] = "/";

  static std::string Join(absl::string_view path1);
  static std::string Join(absl::string_view path1, absl::string_view path2);

  template <typename... T>
  static std::string Join(absl::string_view path1, absl::string_view path2,
                          const T &... args) {
    return Join(path1, Join(path2, args...));
  }
};
} // namespace eole
} // namespace xdk

#endif
