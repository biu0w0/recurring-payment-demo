#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "recurring_payment_impl.h"

using namespace std;

void RunServer(string &server_address, string &db_str) {
    RecurringPaymentImpl::Impl service;

    // 连接数据库
    service.ConnectDb(db_str);

    // 注册服务
    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials()); // 可替换为JWT/OAuth2.0凭据
    builder.RegisterService(&service);

    // 启动服务
    unique_ptr<grpc::Server> server(builder.BuildAndStart());
    cout << "Server listening on " << server_address << endl;
    server->Wait();
}

int main(int argc, char **argv) {
    if (argc < 2) {
        cout << "Usage: recurring_payment_server "
             << "0.0.0.0:50051> \"mysqlx://user:pwd@host:port/db?ssl-mode=disabled\""
             << endl;
        return 0;
    }

    string server_address = argv[1];
    string db_str = argv[2];

    try {
        RunServer(server_address, db_str);
    } catch (const exception &e) {
        cout << "uncaught exception: " << e.what() << endl;
    }

    return 0;
}
