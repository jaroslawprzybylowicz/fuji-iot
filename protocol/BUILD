cc_library(
    name = "fuji_types",
    srcs = ["fuji_types.cc"],
    hdrs = ["fuji_types.h"],
)

cc_library(
    name = "fuji_frame",
    srcs = ["fuji_frame.cc"],
    hdrs = ["fuji_frame.h"],
    visibility = ["//visibility:public"],
    deps = [":fuji_register"],
)

cc_library(
    name = "fuji_register",
    srcs = ["fuji_register.cc"],
    hdrs = ["fuji_register.h"],
    deps = [":fuji_types"],
)

cc_library(
    name = "fuji_ac_state",
    srcs = ["fuji_ac_state.cc"],
    hdrs = ["fuji_ac_state.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":fuji_register",
        ":fuji_types",
    ],
)

cc_library(
    name = "fuji_ac_protocol_handler",
    srcs = ["fuji_ac_protocol_handler.cc"],
    hdrs = ["fuji_ac_protocol_handler.h"],
    visibility = ["//visibility:public"],
    deps = [
        ":fuji_ac_state",
        ":fuji_frame",
        "@com_google_absl//absl/types:optional",
        "@googletest//:gtest_prod",
    ],
)

cc_test(
    name = "fuji_frame_test",
    srcs = ["fuji_frame_test.cc"],
    deps = [
        ":fuji_frame",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "fuji_register_test",
    srcs = ["fuji_register_test.cc"],
    deps = [
        ":fuji_register",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "fuji_ac_protocol_handler_test",
    srcs = ["fuji_ac_protocol_handler_test.cc"],
    deps = [
        ":fuji_ac_protocol_handler",
        "@googletest//:gtest_main",
    ],
)
