load("//:WORKSPACE.bzl", "COPTS")

cc_binary(
    name = "main",
    srcs = ["main.cc"],
    copts = COPTS,
    deps = [
        "@cmark_gfm//:cmark_extensions",
        "@cmark_gfm//:cmark_static",
        "@com_google_absl//absl/strings",
        "@xdk_jude//xdk/jude:do",
        "@xdk_lua//xdk/lua:state",
    ],
)

cc_library(
    name = "path",
    srcs = ["path.cc"],
    hdrs = ["path.h"],
    copts = COPTS,
    deps = [
        "@com_google_absl//absl/strings",
    ],
)

cc_test(
    name = "path_test",
    srcs = ["path_test.cc"],
    copts = COPTS,
    deps = [
        ":path",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "filesystem",
    srcs = ["filesystem.cc"],
    hdrs = ["filesystem.h"],
    copts = COPTS,
    deps = [
        ":path",
    ],
)

cc_test(
    name = "filesystem_test",
    srcs = ["filesystem_test.cc"],
    copts = COPTS,
    deps = [
        ":filesystem",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "fixture",
    testonly = 1,
    srcs = ["fixture.cc"],
    hdrs = ["fixture.h"],
    copts = COPTS,
    deps = [
        ":filesystem",
        ":path",
        "@com_google_googletest//:gtest",
    ],
)

cc_test(
    name = "fixture_test",
    srcs = ["fixture_test.cc"],
    copts = COPTS,
    deps = [
        ":filesystem",
        ":fixture",
        ":path",
        "@com_google_googletest//:gtest_main",
    ],
)

cc_library(
    name = "site",
    srcs = ["site.cc"],
    hdrs = ["site.h"],
    copts = COPTS,
    deps = [
        ":filesystem",
        ":path",
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:optional",
        "@lua",
        "@xdk_jude//xdk/jude:do",
        "@xdk_lua//xdk/lua:popper",
        "@xdk_lua//xdk/lua:rawfield",
        "@xdk_lua//xdk/lua:sandbox",
    ],
)

cc_test(
    name = "site_test",
    srcs = ["site_test.cc"],
    copts = COPTS,
    deps = [
        ":fixture",
        ":site",
        "@com_google_googletest//:gtest_main",
        "@xdk_lua//xdk/lua:matchers",
        "@xdk_lua//xdk/lua:stack",
        "@xdk_lua//xdk/lua:state",
    ],
)

cc_binary(
    name = "eole",
    srcs = ["eole.cc"],
    copts = COPTS,
    deps = [
        ":site",
        "@xdk_lua//xdk/lua:state",
    ],
)
