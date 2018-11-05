#ifndef XDK_EOLE_PROJECT_H
#define XDK_EOLE_PROJECT_H

#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include <memory>
#include <string>
#include <system_error>
#include <vector>

namespace xdk {
namespace eole {

class Project {
public:
  struct File {
    File(absl::string_view path) : path(path) {}

    const std::string path;
  };

  struct Directory {
    explicit Directory(absl::string_view path) : path(path) {}
    const std::string path;

    absl::optional<std::string> config;
    std::vector<std::unique_ptr<Directory>> directories;
    std::vector<File> files;
  };

  explicit Project(absl::string_view rootpath) throw(std::system_error);
  const Directory *root() const { return root_.get(); }

private:
  std::unique_ptr<Directory> root_;
};
} // namespace eole
} // namespace xdk

#endif
