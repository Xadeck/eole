load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")

def dependencies():
    git_repository(
        name = "xdk_jude",
        commit = "10c08e2",
        remote = "https://github.com/Xadeck/jude.git",
    )
    new_git_repository(
        name = "cmark_gfm",
        build_file = "BUILD.cmark-gfm",
        remote = "https://github.com/github/cmark-gfm.git",
        tag = "0.28.3.gfm.19",
    )

COPTS = [
    "-Wall",
    "-Werror",
]
