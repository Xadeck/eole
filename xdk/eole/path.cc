#include "xdk/eole/path.h"
#include "absl/strings/strip.h"

namespace xdk {
namespace eole {

constexpr char Path::Separator[];

std::string Path::Join(absl::string_view path1) { return std::string(path1); }

std::string Path::Join(absl::string_view path1, absl::string_view path2) {
  if (path1.empty()) {
    return std::string(path2);
  }
  if (path2.empty()) {
    return std::string(path1);
  }
  while (absl::ConsumeSuffix(&path1, Separator))
    ;
  while (absl::ConsumePrefix(&path2, Separator))
    ;
  return absl::StrCat(path1, Separator, path2);
}

} // namespace eole
} // namespace xdk
