load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "mysql_connector",
    srcs = ["lib/libmysqlcppconn8.2.8.0.26.dylib"],
    hdrs = glob(["include/**"]),
    includes = ["include"],
    visibility = ["//visibility:public"],
)
