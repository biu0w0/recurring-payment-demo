workspace(name = "com_github_biu0w0_recurring_payment_demo")

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Protobuf Rules for Bazel
# See https://github.com/bazelbuild/rules_proto
http_archive(
    name = "rules_proto",
    sha256 = "bc12122a5ae4b517fa423ea03a8d82ea6352d5127ea48cb54bc324e8ab78493c",
    strip_prefix = "rules_proto-af6481970a34554c6942d993e194a9aed7987780",
    urls = [
        "https://github.com/bazelbuild/rules_proto/archive/af6481970a34554c6942d993e194a9aed7987780.tar.gz",
    ],
)
load("@rules_proto//proto:repositories.bzl", "rules_proto_dependencies", "rules_proto_toolchains")
rules_proto_dependencies()
rules_proto_toolchains()

# gRPC Rules for Bazel
# See https://github.com/grpc/grpc/blob/master/src/cpp/README.md#make
http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "1b9b77f894d4d2cca73f534f4bee574588e5ed7f31794715073e4957fb790cee",
    urls = [
        "https://github.com/grpc/grpc/archive/58602e20a3f3e48f24a4114c757099b25b947f7b.tar.gz",
    ],
    strip_prefix = "grpc-58602e20a3f3e48f24a4114c757099b25b947f7b",
)
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps")
grpc_deps()
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")
grpc_extra_deps()

# mysql connector c++
#http_archive(
#    name = "mysql_connector",
#    sha256 = "c2724fb302717971198fbe9ee88eab11b567245a38930fa09e850d33e51ff52a",
#    strip_prefix = "mysql-connector-c++/8.0.26",
#    url = "https://mirrors.ustc.edu.cn/homebrew-bottles/mysql-connector-c%2B%2B-8.0.26.arm64_big_sur.bottle.tar.gz",
#)
local_repository(
    name = "mysql_connector",
    path = "/opt/homebrew/opt/mysql-connector-c++",
)