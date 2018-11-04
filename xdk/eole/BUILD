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

cc_library(
    name = "project",
    srcs = ["project.cc"],
    hdrs = ["project.h"],
    copts = COPTS,
    deps = [
        "@com_google_absl//absl/memory",
        "@com_google_absl//absl/strings",
        "@com_google_absl//absl/types:optional",
    ],
)

cc_test(
    name = "project_test",
    srcs = ["project_test.cc"],
    copts = COPTS,
    data = glob(["testdata/**"]),
    deps = [
        ":project",
        "@com_github_gflags_gflags//:gflags",
        "@com_google_glog//:glog",
        "@com_google_googletest//:gtest",
    ],
)
