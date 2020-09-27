package(default_visibility = ["//visibility:public"])

load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_proto_library")

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
