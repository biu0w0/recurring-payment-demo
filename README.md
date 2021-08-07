# 使用说明

[TOC]

## 简介

扣费服务的Demo，支持模拟用户侧签约扣费服务、解约扣费服务、查看已签约服务列表。

采用Bazel管理依赖编译，程序入口文件：

- 服务端：`//server:recurring_payment_server`
- 客户端：`//client:recurring_payment_client`

本文档是使用文档，设计文档见[DESIGN.md](DESIGN.md)。

## 服务端使用说明

### 数据库初始化

服务端需要使用MySQL数据库，需提前创建数据表`recurring_payment_contracts`，可直接导入附带SQL文件：`migrations/recurring_payment_contracts.sql`。

### 编译

Bazel编译指令：`bazel build //server:recurring_payment_server `

输出可执行文件路径：`bazel-bin/server/recurring_payment_server`

### 启动参数

```shell
./recurring_payment_server <监听IP>:<监听端口> <mysqlx连接配置字符串>
```

- **监听IP**：仅限本机访问填写`127.0.0.1`，任意主机访问填写`0.0.0.0`
- **监听端口**：默认服务端口为`50051`
- **mysqlx连接配置格式**：`mysqlx://user:pwd@host:port/db?ssl-mode=disabled`

示例启动指令：

```shell
> ./recurring_payment_server 0.0.0.0:50051 "mysqlx://dbusr:dbpwd@127.0.0.1:3306/recurring_payment_demo?ssl-mode=disabled"
< Server listening on 0.0.0.0:50051
```

## 客户端使用说明

### 编译

Bazel编译指令：`bazel build //client:recurring_payment_client `

输出可执行文件路径：`bazel-bin/client/recurring_payment_client`

### 启动参数

```shell
./recurring_payment_client <服务端IP>:<服务端端口>
```

示例启动指令：

```shell
> ./recurring_payment_client 127.0.0.1:50051
< 当前服务端: 127.0.0.1:50051
  可用命令列表:
  	help	打印指令列表
  	login	登录，绑定user_id
  	list	查看已签约协议列表（仅列出有效协议）
  	sign	签约协议
  	term	解约协议
  	prepare	模拟商户准备签约协议，获取待签约协议token
  	quit	退出程序
  输入指令:
```

### 打印指令列表

指令：`help`

列出客户端支持的指令。

### 登录

指令：`login`

Demo客户端未实现完整的登录鉴权，可以使用`login`指令直接设置当前用户`user_id`。示例：

```shell
输入指令: login
请输入user_id: 10086
登录成功
```

真实项目应从客户端的登录态模块获取用户信息。

### 获取待签约协议token

指令：`prepare`

签约流程简述为：商户配置扣费计划>商户准备签约协议>用户完成签约。

Demo中可以使用`prepare`命令模拟商户准备签约协议，获取待签约协议token。示例：

```shell
输入指令: prepare
待签约协议号: CONTRACT|9|FAKE-HASH
你可以使用`sign`指令完成签约
```

### 签约协议

指令：`sign`

输入待签约协议号（token），使用用户秘钥签名，确认完成签约。示例：

```shell
输入指令: sign
请输入待签约协议号: CONTRACT|9|FAKE-HASH
签约成功
```

### 查看已签约协议列表

指令：`list`

获取并列出当前登录用户已签约协议列表。示例：

```
输入指令: list
----- ID 9 -----
协议编号	CC20210807175618
服务名称	腾讯视频会员方案1
应用名称	小程序1
当前状态	Valid
签约时间
开通账号	会员A
详情	这里是扣费服务的简介
```

仅展示有效协议，即已解约、已失效、被平台吊销等状态的协议除外。

### 解约协议

指令：`term`

输入`list`命令中查询到的协议ID，可以从用户侧发起解约。示例：

```shell
输入指令: term
请输入要解约的协议ID: 9
解约成功
```

### 退出程序

指令：`quit`

退出当前客户端程序。

## 附录

### Bazel C++ macOS ARM64架构编译问题

macOS平台编译可使用Homebrew安装Bazel编译。

采用Apple Silicon（M1芯片等）的Mac机型属于ARM64架构，Bazel 4.1.0-homebrew在macOS ARM64架构下编译C++会报错：

```shell
cc_toolchain_suite '@local_config_cc//:toolchain' does not contain a toolchain for cpu 'darwin_arm64'
```

解决方法：

项目目录执行`vim $(bazel info output_base)/external/local_config_cc/BUILD`，替换原有`cc_toolchain_suite`块内容为：

```
cc_toolchain_suite(
    name = "toolchain",
    toolchains = {
        "darwin|clang": ":cc-compiler-darwin",
        "darwin_arm64|clang": ":cc-compiler-darwin", # FIX Apple Silicon
        "darwin": ":cc-compiler-darwin",
        "darwin_arm64": ":cc-compiler-darwin", # FIX Apple Silicon
        "armeabi-v7a|compiler": ":cc-compiler-armeabi-v7a",
        "armeabi-v7a": ":cc-compiler-armeabi-v7a",
    },
)

# FIX Apple Silicon
cc_toolchain(
    name = "cc-compiler-darwin_arm64",
    toolchain_identifier = "local",
    toolchain_config = ":local",
    all_files = ":compiler_deps",
    ar_files = ":compiler_deps",
    as_files = ":compiler_deps",
    compiler_files = ":compiler_deps",
    dwp_files = ":empty",
    linker_files = ":compiler_deps",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 1,
    module_map = ":module.modulemap",
)
```

再次执行`bazel build`或`bazel run`命令即可。

