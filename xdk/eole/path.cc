#include "xdk/eole/path.h"
#include "absl/strings/str_split.h"
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

absl::string_view Path::Dirname(absl::string_view path) {
  auto i = path.find_last_of(Separator);
  if (i == absl::string_view::npos) {
    return ".";
  }
  if (i == 0) {
    return Separator;
  }
  if (i == path.size() - 1) {
    return Dirname(path.substr(0, i));
  }
  return path.substr(0, i);
}

absl::string_view Path::Basename(absl::string_view path) {
  auto i = path.find_last_of(Separator);
  if (i == absl::string_view::npos) {
    return path;
  }
  if (i == path.size() - 1) {
    if (i == 0) {
      return Separator;
    }
    return Basename(path.substr(0, i));
  }
  return path.substr(i + 1);
}

} // namespace eole
} // namespace xdk
