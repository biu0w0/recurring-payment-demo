load("@rules_cc//cc:defs.bzl", "cc_proto_library")
load("@rules_proto//proto:defs.bzl", "proto_library")
load("@com_github_grpc_grpc//bazel:cc_grpc_library.bzl", "cc_grpc_library")

package(default_visibility = ["//visibility:public"])

proto_library(
    name = "recurring_payment_proto",
    srcs = ["recurring_payment.proto"],
)

cc_proto_library(
    name = "recurring_payment_cc_proto",
    deps = [":recurring_payment_proto"],
)

cc_grpc_library(
    name = "recurring_payment_cc_grpc",
    srcs = [":recurring_payment_proto"],
    grpc_only = True,
    deps = [":recurring_payment_cc_proto"],
)
