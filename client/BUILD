load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_library(
    name = "recurring_payment_client_cc",
    srcs = [
        "recurring_payment_client.cc",
        "recurring_payment_client.h",
    ],
    defines = ["BAZEL_BUILD"],
    deps = [
        "//protos:recurring_payment_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
    ],
)

cc_binary(
    name = "recurring_payment_client",
    srcs = [
        "main.cc",
    ],
    defines = ["BAZEL_BUILD"],
    deps = [
        ":recurring_payment_client_cc",
        "//protos:recurring_payment_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
    ],
)
