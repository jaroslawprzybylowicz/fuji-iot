cc_test(
    name = "integration_test",
    srcs = ["integration_test.cc"],
    deps = [
        "//controller:fuji_ac_controller",
        "//protocol:fuji_ac_protocol_handler",
        "//sim:fuji_ac_unit_sim",
        "@com_google_absl//absl/types:optional",
        "@googletest//:gtest_main",
    ],
)
