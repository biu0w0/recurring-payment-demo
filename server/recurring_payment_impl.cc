#include "server/recurring_payment_impl.h"

using RecurringPaymentImpl::Impl;

// (商户)准备扣费协议
Status Impl::PrepareContract(ServerContext *ctx, const PrepareContractReq *req, PrepareContractRsp *rsp) {
    rsp->set_err_code(418);
    rsp->set_err_msg("not implemented");
    return Status::OK;
}

// (用户)签约扣费协议
Status Impl::SignContract(ServerContext *ctx, const SignContractReq *req, SignContractRsp *rsp) {
    rsp->set_err_code(418);
    rsp->set_err_msg("not implemented");
    return Status::OK;
}

// (商户/用户)终止扣费协议
Status Impl::TerminateContract(ServerContext *ctx, const TerminateContractReq *req, TerminateContractRsp *rsp) {
    rsp->set_err_code(418);
    rsp->set_err_msg("not implemented");
    return Status::OK;
}

// (用户)查看有效扣费服务列表
Status Impl::GetValidContractList(ServerContext *ctx, const GetValidContractListReq *req,
                                  GetValidContractListRsp *rsp) {
    rsp->set_err_code(418);
    rsp->set_err_msg("not implemented");
    return Status::OK;
}
