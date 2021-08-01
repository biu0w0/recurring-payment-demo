#include <iostream>
#include <grpcpp/grpcpp.h>

#include "client/recurring_payment_client.h"
#include "protos/recurring_payment.grpc.pb.h"

using namespace std;
using RecurringPaymentClient::Client;
using RecurringPaymentClient::ClientException;
using RecurringPayment::RecurringPaymentSvc;
using RecurringPayment::PrepareContractReq;
using RecurringPayment::PrepareContractRsp;
using RecurringPayment::SignContractReq;
using RecurringPayment::SignContractRsp;
using RecurringPayment::TerminateContractReq;
using RecurringPayment::TerminateContractRsp;
using RecurringPayment::GetUserContractListReq;
using RecurringPayment::GetUserContractListRsp;

// 签名
string sign(const map<string, string> &params) {
    // 伪签名
    return "mock-signature";
}

// 登录
bool Client::Login(long unsigned int uid) {
    user_id = uid;
    return true;
}

// 协议列表
void Client::ListContract(unsigned int limit, unsigned int offset) {
    CheckLogin();

    // 请求RPC76
    GetUserContractListReq req;
    req.set_user_id(user_id);
    req.set_limit(limit);
    req.set_offset(offset);
    GetUserContractListRsp rsp;
    grpc::ClientContext ctx;
    grpc::Status status = stub_->GetUserContractList(&ctx, req, &rsp);
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
bool Client::SignContract(const string &contract_token) {
    CheckLogin();

    // 生成签名
    map<string, string> sign_map;
    sign_map["user_id"] = to_string(user_id);
    sign_map["contract_token"] = contract_token;
    string signature = sign(sign_map);

    // 请求RPC
    SignContractReq req;
    req.set_user_id(user_id);
    req.set_contract_token(contract_token);
    req.set_signature(signature);
    SignContractRsp rsp;
    grpc::ClientContext ctx;
    grpc::Status status = stub_->SignContract(&ctx, req, &rsp);
    if (!status.ok()) {
        string msg = "RPC调用失败: [" + to_string(rsp.err_code()) + "] " + rsp.err_msg();
        throw ClientException(msg);
    }
    if (rsp.err_code() != 0) {
        string msg = "[" + to_string(rsp.err_code()) + "] " + rsp.err_msg();
        throw ClientException(msg);
    }

    return true;
}

// 解约协议
bool Client::TerminateContract(unsigned long int contract_id) {
    CheckLogin();

    // 生成签名
    map<string, string> sign_map;
    sign_map["user_id"] = to_string(user_id);
    sign_map["contract_token"] = to_string(contract_id);
    string signature = sign(sign_map);

    // 请求RPC
    TerminateContractReq req;
    req.set_user_id(user_id);
    req.set_contract_id(contract_id);
    req.set_signature(signature);
    TerminateContractRsp rsp;
    grpc::ClientContext ctx;
    grpc::Status status = stub_->TerminateContract(&ctx, req, &rsp);
    if (!status.ok()) {
        string msg = "RPC调用失败: [" + to_string(rsp.err_code()) + "] " + rsp.err_msg();
        throw ClientException(msg);
    }
    if (rsp.err_code() != 0) {
        string msg = "[" + to_string(rsp.err_code()) + "] " + rsp.err_msg();
        throw ClientException(msg);
    }

    return true;
}

// 准备协议
string Client::PrepareContract() {
    CheckLogin();

    unsigned long mch_id = 1; // 商户ID
    unsigned long app_id = 1; // 应用ID
    unsigned long plan_id = 1; // 代扣方案ID
    string contract_code = "CC"; // 商户协议号
    string display_account = "会员A"; // 开通账户名称
    string callback_url = "http://localhost"; // 签约结果回调URl

    // 生成签名
    map<string, string> sign_map;
    string signature = sign(sign_map);

    // 请求RPC
    PrepareContractReq req;
    req.set_mch_id(mch_id);
    req.set_app_id(app_id);
    req.set_plan_id(plan_id);
    req.set_contract_code(contract_code);
    req.set_display_account(display_account);
    req.set_callback_url(callback_url);
    req.set_signature(signature);
    PrepareContractRsp rsp;
    grpc::ClientContext ctx;
    grpc::Status status = stub_->PrepareContract(&ctx, req, &rsp);
    if (!status.ok()) {
        string msg = "RPC调用失败: [" + to_string(rsp.err_code()) + "] " + rsp.err_msg();
        throw ClientException(msg);
    }
    if (rsp.err_code() != 0) {
        string msg = "[" + to_string(rsp.err_code()) + "] " + rsp.err_msg();
        throw ClientException(msg);
    }

    return rsp.contract_token();
}

// 检查登录状态
void Client::CheckLogin() const {
    if (user_id == 0) {
        string msg = "请先调用`login`指令登录";
        throw ClientException(msg);
    }
}
