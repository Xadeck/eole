#include "xdk/eole/site.h"
#include "xdk/lua/state.h"

#include <iostream>
#include <string>

int main(int argc, char **argv) {
  // Check exactly one argument is given.
  if (argc != 2) {
    std::cerr << "Missing directory.\n";
  }
  const std::string root(argv[1]);
  xdk::lua::State L;
  lua_newtable(L);
  xdk::eole::Site::Build(L, root);
  std::cout << "site generated under " << root << "\n";
  return 0;
}
