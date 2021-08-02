#include "server/recurring_payment_impl.h"
#include "server/helper.h"
#include <mysqlx/xdevapi.h>
#include <regex>

#define contracts_table "recurring_payment_contracts"

using RecurringPaymentImpl::Impl;
using RecurringPaymentImpl::ImplException;
using RecurringPayment::ContractInfo;
using RecurringPayment::ContractState;

// 初始化数据库
void Impl::ConnectDb(const std::string &db_str) {
    db = new mysqlx::Session(db_str);
}

// (商户)准备扣费协议
Status Impl::PrepareContract(ServerContext *ctx, const PrepareContractReq *req, PrepareContractRsp *rsp) {
    try {
        // 校验并获取商户信息
        Helper::MchInfo mch = Helper::GetMchInfo(req->mch_id());
        if (mch.Id == 0) {
            throw ImplException(422, "输入商户ID有误");
        }

        // 校验并获取应用信息
        Helper::AppInfo app = Helper::GetAppInfo(req->mch_id(), req->app_id());
        if (app.Id == 0) {
            throw ImplException(422, "输入应用ID有误");
        }

        // 校验并获取扣费计划信息
        Helper::PlanInfo plan = Helper::GetPlanInfo(req->mch_id(), req->plan_id());
        if (plan.Id == 0) {
            throw ImplException(422, "输入扣费计划ID有误");
        }

        // todo 校验contract_code/display_account/callback_url格式
        if (req->contract_code().empty()) { throw ImplException(422, "参数contract_code不能为空"); }
        if (req->display_account().empty()) { throw ImplException(422, "参数display_account不能为空"); }
        if (req->callback_url().empty()) { throw ImplException(422, "参数callback_url不能为空"); }

        // 签约时限2小时
        time_t t = time(nullptr) + 7200;
        char time_str[19];
        strftime(time_str, 19, "%Y-%m-%d %H:%M:%S", localtime(&t));
        std::string expired_time = std::string(time_str, 19);

        // 创建扣费协议
        mysqlx::Result result = (*db).getDefaultSchema()
                .getTable(contracts_table)
                .insert("mch_id", "app_id", "plan_id", "expired_time",
                        "contract_code", "contract_state", "display_account")
                .values(mch.Id, app.Id, plan.Id, expired_time,
                        req->contract_code(), int(ContractState::Pending), req->display_account())
                .execute();
        unsigned long contract_id = result.getAutoIncrementValue();
        std::string contract_token = "CONTRACT|" + std::to_string(contract_id) + "|FAKE-HASH";

        // 整理返回
        rsp->set_contract_token(contract_token);
        rsp->set_expired_time(expired_time);
    } catch (const ImplException &e) {
        rsp->set_err_code(e.getCode());
        rsp->set_err_msg(e.getMessage());
    } catch (const std::exception &e) {
        std::cout << "PrepareContract exception: " << e.what() << std::endl;
        rsp->set_err_code(500);
        rsp->set_err_msg("Server Error");
    }
    return Status::OK;
}

// (用户)签约扣费协议
Status Impl::SignContract(ServerContext *ctx, const SignContractReq *req, SignContractRsp *rsp) {
    try {
        // 校验参数
        if (req->contract_token().empty()) {
            throw ImplException(422, "contract_token is required");
        }
        if (req->user_id() == 0) {
            throw ImplException(422, "user_id is invalid");
        }
        if (req->signature().empty()) {
            throw ImplException(422, "signature is required");
        }

        // todo 检查用户签名
        // if (req->signature() != expect_signature) {}

        // 解出待签约协议ID
        unsigned long contract_id = 0;
        std::regex re(R"(CONTRACT\|(\d+)\|FAKE-HASH)");
        std::smatch matches;
        if (!std::regex_match(req->contract_token(), matches, re) || matches.size() != 2) {
            throw ImplException(404, "contract not found");
        }
        contract_id = std::strtoul(matches[1].str().data(), nullptr, 10);

        // 当前时间
        time_t t = time(nullptr);
        char time_str[19];
        strftime(time_str, 19, "%Y-%m-%d %H:%M:%S", localtime(&t));
        std::string current_time = std::string(time_str, 19);

        // 获取待签约协议
        mysqlx::RowResult rows = (*db).getDefaultSchema()
                .getTable(contracts_table)
                .select("id", "user_id", "mch_id", "app_id", "plan_id",
                        "contract_code", "contract_state", "display_account",
                        "signed_time", "expired_time", "terminated_time")
                .where("id = :id AND user_id = 0 AND contract_state = :state AND expired_time > :now")
                .limit(1)
                .bind("id", contract_id)
                .bind("state", int(ContractState::Pending))
                .bind("now", current_time)
                .execute();
        mysqlx::Row row = rows.fetchOne();
        if (row.isNull()) {
            throw ImplException(404, "contract not found");
        }
        ContractInfo c;
        Helper::RowToContractInfo(row, c);

        // 校验协议状态：未签约、未绑定UID
        if (c.contract_state() != ContractState::Pending) {
            throw ImplException(422, "contract is not available");
        }
        if (c.user_id() != 0) {
            throw ImplException(422, "contract is not available");
        }

        // 校验协议过期
        if (!c.expired_time().empty()) {
            tm expired_tm{};
            strptime(c.expired_time().data(), "%Y-%m-%d %H:%M:%S", &expired_tm);
            if (mktime(&expired_tm) < time(nullptr)) {
                throw ImplException(422, "contract is expired");
            }
        }

        // todo 检查用户是否有相同plan_id的生效中协议
        // if (...) {}

        // 签署协议有效期
        t += 86400 * 365; // 一年
        strftime(time_str, 19, "%Y-%m-%d %H:%M:%S", localtime(&t));
        std::string expired_time = std::string(time_str, 19);

        // 更新协议
        mysqlx::Result result = (*db).getDefaultSchema()
                .getTable(contracts_table)
                .update()
                .set("user_id", req->user_id())
                .set("contract_state", int(ContractState::Valid))
                .set("signed_time", current_time)
                .set("expired_time", expired_time)
                .where("id = :id AND user_id = 0 AND contract_state = :state AND expired_time > :now")
                .limit(1)
                .bind("id", c.contract_id())
                .bind("state", int(ContractState::Pending))
                .bind("now", current_time)
                .execute();
        if (result.getAffectedItemsCount() == 0) {
            throw ImplException(503, "contract is not available");
        }
    } catch (const ImplException &e) {
        rsp->set_err_code(e.getCode());
        rsp->set_err_msg(e.getMessage());
    } catch (const std::exception &e) {
        std::cout << "SignContract exception: " << e.what() << std::endl;
        rsp->set_err_code(500);
        rsp->set_err_msg("Server Error");
    }
    return Status::OK;
}

// (商户/用户)终止扣费协议
Status Impl::TerminateContract(ServerContext *ctx, const TerminateContractReq *req, TerminateContractRsp *rsp) {
    try {
        // 校验参数
        if (req->user_id() == 0) {
            throw ImplException(422, "user_id is invalid");
        }
        if (req->contract_id() == 0) {
            throw ImplException(422, "contract_id is required");
        }
        if (req->signature().empty()) {
            throw ImplException(422, "signature is required");
        }

        // todo 检查用户签名
        // if (req->signature() != expect_signature) {}

        // 获取可解约协议
        mysqlx::RowResult rows = (*db).getDefaultSchema()
                .getTable(contracts_table)
                .select("id", "user_id", "mch_id", "app_id", "plan_id",
                        "contract_code", "contract_state", "display_account",
                        "signed_time", "expired_time", "terminated_time")
                .where("id = :id AND user_id = :uid AND contract_state = :state")
                .limit(1)
                .bind("id", req->contract_id())
                .bind("uid", req->user_id())
                .bind("state", int(ContractState::Valid))
                .execute();
        mysqlx::Row row = rows.fetchOne();
        if (row.isNull()) {
            throw ImplException(404, "contract not found");
        }
        ContractInfo c;
        Helper::RowToContractInfo(row, c);

        // 当前时间
        time_t t = time(nullptr);
        char time_str[19];
        strftime(time_str, 19, "%Y-%m-%d %H:%M:%S", localtime(&t));
        std::string current_time = std::string(time_str, 19);

        // 更新协议
        mysqlx::Result result = (*db).getDefaultSchema()
                .getTable(contracts_table)
                .update()
                .set("contract_state", int(ContractState::UserTerminated))
                .set("terminated_time", current_time)
                .where("id = :id AND user_id = :uid AND contract_state = :state")
                .limit(1)
                .bind("id", c.contract_id())
                .bind("uid", c.user_id())
                .bind("state", int(ContractState::Valid))
                .execute();
        if (result.getAffectedItemsCount() == 0) {
            throw ImplException(503, "contract is not available");
        }
    } catch (const ImplException &e) {
        rsp->set_err_code(e.getCode());
        rsp->set_err_msg(e.getMessage());
    } catch (const std::exception &e) {
        std::cout << "TerminateContract exception: " << e.what() << std::endl;
        rsp->set_err_code(500);
        rsp->set_err_msg("Server Error");
    }
    return Status::OK;
}

// (用户)查看扣费服务列表
Status Impl::GetUserContractList(ServerContext *ctx, const GetUserContractListReq *req,
                                 GetUserContractListRsp *rsp) {
    try {
        // 校验当前用户是否合法
        if (req->user_id() == 0) {
            throw ImplException(422, "user_id is invalid");
        }

        // 获取当前用户有效协议
        mysqlx::RowResult rows = (*db).getDefaultSchema()
                .getTable(contracts_table)
                .select()
                .where("user_id = :user_id AND contract_state = :contract_state")
                .limit(req->limit())
                .offset(req->offset())
                .bind("user_id", req->user_id())
                .bind("contract_state", int(ContractState::Valid))
                .execute();

        // 整理返回
        rsp->set_total(rows.count()); // todo
        rsp->set_limit(req->limit());
        rsp->set_offset(req->offset());
        while (mysqlx::Row row = rows.fetchOne()) {
            ContractInfo *c = rsp->add_contract_list();
            Helper::RowToContractInfo(row, *c);
        }
    } catch (const ImplException &e) {
        rsp->set_err_code(e.getCode());
        rsp->set_err_msg(e.getMessage());
    } catch (const std::exception &e) {
        std::cout << "GetUserContractList exception: " << e.what() << std::endl;
        rsp->set_err_code(500);
        rsp->set_err_msg("Server Error");
    }
    return Status::OK;
}
