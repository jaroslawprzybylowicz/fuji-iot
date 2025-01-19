load("@rules_proto//proto:defs.bzl", "proto_library")
load("@grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")
load("@grpc//bazel:grpc_build_system.bzl", "grpc_proto_library")

package(default_visibility = ["//visibility:public"])

proto_library(
    name = "fuji_ac_controller_proto",
    srcs = ["fuji_ac_controller.proto"],
)

cc_proto_library(
    name = "fuji_ac_controller_cc_proto",
    deps = [":fuji_ac_controller_proto"],
)

cc_library(
    name = "fuji_ac_controller",
    srcs = ["fuji_ac_controller.cc"],
    hdrs = ["fuji_ac_controller.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":fuji_ac_controller_cc_proto",
        ":fuji_ac_serial_interface",
        "//protocol:fuji_ac_protocol_handler",
        "//protocol:fuji_ac_state",
        "@abseil-cpp//absl/status",
        "@abseil-cpp//absl/synchronization",
        "@glog",
    ],
)

cc_library(
    name = "fuji_ac_serial_interface",
    hdrs = ["fuji_ac_serial_interface.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//protocol:fuji_ac_protocol_handler",
    ],
)

cc_library(
    name = "fuji_ac_serial_reader",
    srcs = ["fuji_ac_serial_reader.cc"],
    hdrs = ["fuji_ac_serial_reader.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":fuji_ac_serial_interface",
        "@abseil-cpp//absl/time",
        "@glog",
    ],
)

cc_grpc_library(
    name = "fuji_ac_controller_cc_grpc",
    srcs = [":fuji_ac_controller_proto"],
    grpc_only = True,
    deps = [":fuji_ac_controller_cc_proto"],
)

cc_binary(
    name = "fuji_ac_server",
    srcs = ["fuji_ac_server.cc"],
    deps = [
        ":fuji_ac_controller",
        ":fuji_ac_controller_cc_grpc",
        ":fuji_ac_serial_reader",
        "//sim:fuji_ac_unit_sim",
        "@grpc//:grpc++",
        "@glog",
    ],
)

cc_binary(
    name = "fuji_ac_controller_client",
    srcs = ["fuji_ac_controller_client.cc"],
    deps = [
        ":fuji_ac_controller_cc_grpc",
        "@grpc//:grpc++",
        "@abseil-cpp//absl/strings:str_format",
        "@glog",
    ],
)
