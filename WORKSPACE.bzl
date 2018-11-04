load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

def dependencies():
    git_repository(
        name = "xdk_ltemplate",
        commit = "2fe4d99",
        remote = "https://github.com/Xadeck/ltemplate.git",
    )
    new_git_repository(
        name = "cmark_gfm",
        build_file = "BUILD.cmark-gfm",
        remote = "https://github.com/github/cmark-gfm.git",
        tag = "0.28.3.gfm.19",
    )
    git_repository(
        name = "com_github_gflags_gflags",
        commit = "7e70988",
        remote = "https://github.com/gflags/gflags.git",
    )
    git_repository(
        name = "com_google_glog",
        commit = "e364e75",
        remote = "https://github.com/google/glog.git",
    )

COPTS = [
    "-Wall",
    "-Werror",
]
