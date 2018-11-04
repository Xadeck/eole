#ifndef XDK_EOLE_PROJECT_H
#define XDK_EOLE_PROJECT_H

#include "absl/strings/string_view.h"
#include "absl/types/optional.h"
#include <memory>
#include <string>
#include <vector>

namespace xdk {
namespace eole {

class Project {
public:
  struct File {
    class Generator {
    public:
      virtual ~Generator() {}
      virtual const char *Name() const = 0;
      virtual bool CanGenerate(absl::string_view filepath) const = 0;
    };
    File(absl::string_view path, const Generator *generator)
        : path(path), generator(generator) {}

    const std::string path;
    const Generator *generator;
  };

  struct Directory {
    explicit Directory(absl::string_view path) : path(path) {}
    const std::string path;

    absl::optional<std::string> config;
    std::vector<std::unique_ptr<Directory>> directories;
    std::vector<File> files;
  };
  class Builder {
  public:
    explicit Builder(absl::string_view rootpath);
    Builder &AddFileGenerator(const File::Generator *generator);

    Project Build() const;

  private:
    void BuildDirectory(Directory *directory) const;
    const std::string rootpath_;
    std::vector<const File::Generator *> generators_;
  };

  const Directory *root() { return root_.get(); }

private:
  friend class Builder;
  std::unique_ptr<Directory> root_;
};
} // namespace eole
} // namespace xdk

#endif
