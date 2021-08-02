#include <grpcpp/grpcpp.h>
#include "protos/recurring_payment.grpc.pb.h"
#include <mysqlx/xdevapi.h>

using grpc::Status;
using grpc::ServerContext;
using RecurringPayment::RecurringPaymentSvc;
using RecurringPayment::PrepareContractReq;
using RecurringPayment::PrepareContractRsp;
using RecurringPayment::SignContractReq;
using RecurringPayment::SignContractRsp;
using RecurringPayment::TerminateContractReq;
using RecurringPayment::TerminateContractRsp;
using RecurringPayment::GetUserContractListReq;
using RecurringPayment::GetUserContractListRsp;

namespace RecurringPaymentImpl {
    class Impl final : public RecurringPaymentSvc::Service {
    public:
        // 初始化数据库
        void ConnectDb(const std::string &db_str);

        // (商户)准备扣费协议
        Status PrepareContract(ServerContext *ctx, const PrepareContractReq *req, PrepareContractRsp *rsp) override;

        // (用户)签约扣费协议
        Status SignContract(ServerContext *ctx, const SignContractReq *req, SignContractRsp *rsp) override;

        // (商户/用户)终止扣费协议
        Status TerminateContract(ServerContext *ctx, const TerminateContractReq *req,
                                 TerminateContractRsp *rsp) override;

        // (用户)查看有效扣费服务列表
        Status GetUserContractList(ServerContext *ctx, const GetUserContractListReq *req,
                                   GetUserContractListRsp *rsp) override;

    private:
        mysqlx::Session *db;
    };

    class ImplException : public std::exception {
    public:
        ImplException(int err_code, const std::string &err_msg) {
            code = err_code;
            msg = err_msg;
        }

        const char *what() const throw() override {
            return msg.c_str();
        }

        int getCode() const {
            return code;
        }

        std::string getMessage() const {
            return msg;
        }

    protected:
        int code;
        std::string msg;
    };
}