load("//:WORKSPACE.bzl", "COPTS")

cc_binary(
    name = "eole",
    srcs = ["main.cc"],
    copts = COPTS,
    deps = [
        "@cmark_gfm//:cmark_extensions",
        "@cmark_gfm//:cmark_static",
        "@com_google_absl//absl/strings",
        "@xdk_ltemplate//xdk/ltemplate",
        "@xdk_lua//xdk/lua:state",
    ],
)
