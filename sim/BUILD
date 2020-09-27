cc_library(
    name = "fuji_ac_unit_sim",
    srcs = ["fuji_ac_unit_sim.cc"],
    hdrs = ["fuji_ac_unit_sim.h"],
    visibility = ["//visibility:public"],
    deps = [
        "//protocol:fuji_frame",
        "@com_google_absl//absl/types:optional",
        "@glog",
	"@googletest//:gtest_prod",
    ],
)

cc_test(
    name = "fuji_ac_unit_sim_test",
    srcs = ["fuji_ac_unit_sim_test.cc"],
    deps = [
        ":fuji_ac_unit_sim",
        "@googletest//:gtest_main",
    ],
)
