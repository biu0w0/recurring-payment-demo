#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "recurring_payment_impl.h"

void RunServer(std::string &server_address) {
    RecurringPaymentImpl::Impl service;

    grpc::ServerBuilder builder;
    // DEMO不使用凭据，正式可选用JWT/AccessToken(OAuth2.0)作为凭据
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "Server listening on " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char **argv) {
    if (argc < 2) {
        std::cout << "Usage: recurring_payment_server <listen_ip>:<listen_port>" << std::endl;
        return 0;
    }

    std::string server_address = argv[1];

    RunServer(server_address);

    return 0;
}
