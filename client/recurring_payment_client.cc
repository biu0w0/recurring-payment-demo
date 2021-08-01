#include <iostream>
#include <grpcpp/grpcpp.h>

#include "client/recurring_payment_client.h"
#include "protos/recurring_payment.grpc.pb.h"

using namespace std;
using RecurringPaymentClient::Client;
using RecurringPaymentClient::ClientException;
using RecurringPayment::RecurringPaymentSvc;
using RecurringPayment::GetValidContractListReq;
using RecurringPayment::GetValidContractListRsp;

// 登录
bool Client::Login(long unsigned int uid) {
    user_id = uid;
    return true;
}

// 协议列表
void Client::ListContract(unsigned int limit, unsigned int offset) {
    if (user_id == 0) {
        string msg = "请先调用`login`指令登录";
        throw ClientException(msg);
    }

    // 请求RPC
    GetValidContractListReq req;
    req.set_user_id(user_id);
    req.set_limit(limit);
    req.set_offset(offset);
    GetValidContractListRsp rsp;
    grpc::ClientContext ctx;
    grpc::Status status = stub_->GetValidContractList(&ctx, req, &rsp);
    if (!status.ok()) {
        string msg = "RPC调用失败: [" + to_string(rsp.err_code()) + "] " + rsp.err_msg();
        throw ClientException(msg);
    }
    if (rsp.err_code() != 0) {
        string msg = "[" + to_string(rsp.err_code()) + "] " + rsp.err_msg();
        throw ClientException(msg);
    }

    // 打印协议列表
    rsp.contract_list();
}

// 签约协议
bool Client::SignContract(string contract_code) {
    if (user_id == 0) {
        string msg = "请先调用`login`指令登录";
        throw ClientException(msg);
    }

    cout << "开发中" << endl;
    return false;
}

// 解约协议
bool Client::TerminateContract(unsigned long int contract_id) {
    if (user_id == 0) {
        string msg = "请先调用`login`指令登录";
        throw ClientException(msg);
    }

    cout << "开发中" << endl;
    return false;
}

// 准备协议
string Client::PrepareContract() {
    if (user_id == 0) {
        string msg = "请先调用`login`指令登录";
        throw ClientException(msg);
    }

    cout << "开发中" << endl;
    return "";
}
