load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "helper",
    srcs = [
        "helper.cc",
        "helper.h",
    ],
    deps = [
        "//protos:recurring_payment_cc_grpc",
        "@mysql_connector//:mysql_connector",
    ],
)

cc_binary(
    name = "recurring_payment_server",
    srcs = [
        "main.cc",
        "recurring_payment_impl.cc",
        "recurring_payment_impl.h",
    ],
    deps = [
        ":helper",
        "//protos:recurring_payment_cc_grpc",
        "@mysql_connector//:mysql_connector",
        "@com_github_grpc_grpc//:grpc++",
    ],
)
