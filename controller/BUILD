package(default_visibility = ["//visibility:public"])

load("@rules_proto//proto:defs.bzl", "proto_library")
load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")

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
        "@com_google_absl//absl/status",
        "@com_google_absl//absl/synchronization",
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
        "@com_google_absl//absl/time",
        "@glog",
    ],
)
