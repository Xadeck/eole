#ifndef XDK_EOLE_FIXTURE_H
#define XDK_EOLE_FIXTURE_H

#include <string>

#include "absl/strings/string_view.h"

namespace xdk {
namespace eole {

class Fixture {
public:
  Fixture();
  const std::string &Dirpath() const { return dirpath_; }
  std::string Path(absl::string_view relative_filepath) const;

  void AddFile(absl::string_view relative_filepath,
               absl::string_view content) const;

private:
  std::string dirpath_;
};

} // namespace eole
} // namespace xdk

#endif
