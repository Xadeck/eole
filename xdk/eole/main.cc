#include "absl/strings/string_view.h"
#include "extensions/cmark-gfm-core-extensions.h"
#include "src/cmark-gfm.h"
#include "src/node.h"
#include "xdk/ltemplate/ltemplate.h"
#include "xdk/lua/lua.hpp"
#include "xdk/lua/state.h"

#include <iostream>
#include <string>

int main(int argc, char **argv) {
  xdk::lua::State L;
  lua_newtable(L);
  absl::string_view source = argv[1];
  if (int error =
          xdk::ltemplate::dostring(L, source.data(), source.size(), "arg")) {
    std::cerr << lua_tostring(L, -1) << "\n";
    return error;
  }
  lua_getfield(L, -1, "_");
  absl::string_view markdown = lua_tostring(L, -1);

  cmark_parser *parser = cmark_parser_new(CMARK_OPT_DEFAULT);
  cmark_parser_feed(parser, markdown.data(), markdown.size());
  cmark_node *node = cmark_parser_finish(parser);
  cmark_parser_free(parser);
  char *const output = cmark_render_html(node, CMARK_OPT_DEFAULT, nullptr);
  std::string html(output);
  cmark_node_mem(node)->free(output);
  cmark_node_free(node);

  std::cout << html;
  return 0;
}
