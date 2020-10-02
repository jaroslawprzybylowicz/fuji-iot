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
    remote = "https://github.com/google/googletest",
    tag = "release-1.10.0",
)

git_repository(
    name = "com_google_absl",
    remote = "https://github.com/abseil/abseil-cpp.git",
    tag = "20200225.2",
)

new_git_repository(
    name = "paho_mqtt_c",
    build_file_content = all_content,
    commit = "d34c51214f4172f2e12bb17532c9f44f72a57dd4",
    remote = "https://github.com/eclipse/paho.mqtt.c.git",
    shallow_since = "1565130246 +0100",
)

new_git_repository(
    name = "paho_mqtt_cpp",
    build_file_content = all_content,
    commit = "0e44dd31ff725d66df4a928d50a26309626dcfd5",
    remote = "https://github.com/eclipse/paho.mqtt.cpp.git",
    shallow_since = "1570913123 -0400",
)

new_git_repository(
    name = "libjwt",
    build_file_content = all_content,
    commit = "c0b824eac696082dc2887c997bd256f5e60f8fe0",
    remote = "https://github.com/benmcollins/libjwt.git",
    shallow_since = "1579549527 +0000",
)

new_git_repository(
    name = "libjansson",
    build_file_content = all_content,
    commit = "71c4e8ec215afa225ac20eed269a14963cd37b50",
    remote = "https://github.com/akheron/jansson.git",
    shallow_since = "1543219804 +0100",
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

http_archive(
    name = "openssl",
    build_file_content = all_content,
    sha256 = "f6fb3079ad15076154eda9413fed42877d668e7069d9b87396d0804fdb3f4c90",
    strip_prefix = "openssl-1.1.1c",
    urls = ["https://www.openssl.org/source/openssl-1.1.1c.tar.gz"],
)

http_archive(
    name = "rules_proto",
    sha256 = "602e7161d9195e50246177e7c55b2f39950a9cf7366f74ed5f22fd45750cd208",
    strip_prefix = "rules_proto-97d8af4dc474595af3900dd85cb3a29ad28cc313",
    urls = [
        "https://mirror.bazel.build/github.com/bazelbuild/rules_proto/archive/97d8af4dc474595af3900dd85cb3a29ad28cc313.tar.gz",
        "https://github.com/bazelbuild/rules_proto/archive/97d8af4dc474595af3900dd85cb3a29ad28cc313.tar.gz",
    ],
)

##### gRPC Rules for Bazel
##### See https://github.com/grpc/grpc/blob/master/src/cpp/README.md#make
http_archive(
    name = "com_github_grpc_grpc",
    strip_prefix = "grpc-de893acb6aef88484a427e64b96727e4926fdcfd",
    urls = [
        "https://github.com/grpc/grpc/archive/de893acb6aef88484a427e64b96727e4926fdcfd.tar.gz",
    ],
)

# Rule repository
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

