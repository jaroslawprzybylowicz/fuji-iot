load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository", "new_git_repository")
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

all_content = """filegroup(name = "all", srcs = glob(["**"]), visibility = ["//visibility:public"])"""

http_archive(
    name = "rules_cc",
    strip_prefix = "rules_cc-master",
    urls = ["https://github.com/bazelbuild/rules_cc/archive/master.zip"],
)

git_repository(
    name = "googletest",
    commit = "703bd9caab50b139428cea1aaff9974ebee5742e",
    remote = "https://github.com/google/googletest",
    shallow_since = "1570114335 -0400",
)

git_repository(
    name = "com_google_absl",
    commit = "c51510d1d87ebce8615ae1752fd5aca912f6cf4c",
    remote = "https://github.com/abseil/abseil-cpp.git",
    shallow_since = "1587584588 -0400",
)

git_repository(
    name = "glog",
    commit = "96a2f23dca4cc7180821ca5f32e526314395d26a",
    remote = "https://github.com/google/glog.git",
    shallow_since = "1553223106 +0900",
)

git_repository(
    name = "com_github_gflags_gflags",
    commit = "e171aa2d15ed9eb17054558e0b3a6a413bb01067",
    remote = "https://github.com/gflags/gflags.git",
    shallow_since = "1541971260 +0000",
)

git_repository(
     name = "rules_proto",
     commit = "cfdc2fa31879c0aebe31ce7702b1a9c8a4be02d2",
     remote = "https://github.com/bazelbuild/rules_proto.git",
)

http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "61272ea6d541f60bdc3752ddef9fd4ca87ff5ab18dd21afc30270faad90c8a34",
    strip_prefix = "grpc-de893acb6aef88484a427e64b96727e4926fdcfd",
    urls = [
        "https://github.com/grpc/grpc/archive/de893acb6aef88484a427e64b96727e4926fdcfd.tar.gz",
    ],
)

http_archive(
    name = "rules_foreign_cc",
    strip_prefix = "rules_foreign_cc-master",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/master.zip",
)

load("@rules_foreign_cc//:workspace_definitions.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")

rules_proto_dependencies()

rules_proto_toolchains()

load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")

grpc_deps()

# Not mentioned in official docs... mentioned here https://github.com/grpc/grpc/issues/20511
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")

grpc_extra_deps()
