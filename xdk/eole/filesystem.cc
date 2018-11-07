#include "xdk/eole/filesystem.h"
#include "xdk/eole/path.h"
#include <cstdlib>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <sys/stat.h>

namespace xdk {
namespace eole {

void Filesystem::Mkdir(absl::string_view dirpath) throw(std::system_error) {
  if (dirpath == "/" || dirpath.empty()) {
    return;
  }
  Mkdir(Path::Dirname(dirpath));
  if (mkdir(std::string(dirpath).c_str(),
            S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
    if (errno != EEXIST) {
      throw std::system_error(errno, std::generic_category(),
                              absl::StrCat("mkdir(", dirpath, ")"));
    }
  }
}

} // namespace eole
} // namespace xdk
