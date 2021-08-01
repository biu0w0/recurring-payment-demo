load("@rules_cc//cc:defs.bzl", "cc_binary", "cc_library")

cc_binary(
    name = "recurring_payment_server",
    srcs = [
        "main.cc",
        "recurring_payment_impl.cc",
        "recurring_payment_impl.h",
        "contract_dao.cc",
        "contract_dao.h"
    ],
    deps = [
        "//protos:recurring_payment_cc_grpc",
        "@com_github_grpc_grpc//:grpc++",
    ],
)
