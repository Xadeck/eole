#ifndef XDK_EOLE_FIXTURE_H
#define XDK_EOLE_FIXTURE_H

#include <string>

#include "absl/strings/string_view.h"
#include "gmock/gmock.h"

namespace xdk {
namespace eole {

// Fixture represents a way to create a file hierarchy under TEST_TMPDIR
// directory, in a bazel test. A fixture is unique per TEST/TEST_F case. It is
// initially empty, and gets deleted at the end of the case.
class Fixture {
public:
  Fixture();
  const std::string &Dirpath() const { return dirpath_; }
  std::string Path(absl::string_view relative_filepath) const;

  void AddFile(absl::string_view relative_filepath,
               absl::string_view content) const;

private:
  friend std::ostream &operator<<(std::ostream &os, const Fixture &fixture);
  const std::string name_;
  const std::string dirpath_;
};

::testing::Matcher<const Fixture &>
HasFile(absl::string_view relative_path,
        ::testing::Matcher<absl::string_view> matcher);

} // namespace eole
} // namespace xdk

#endif
