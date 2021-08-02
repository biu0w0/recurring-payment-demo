#include "server/helper.h"

#include <mysqlx/xdevapi.h>

namespace Helper {
    UserInfo GetUserInfo(unsigned long user_id) {
        UserInfo user;
        if (user_id == 0) {
            return user;
        }

        user.Id = user_id;
        user.Name = "用户" + std::to_string(user_id);
        return user;
    }

    MchInfo GetMchInfo(unsigned long mch_id) {
        MchInfo mch;
        if (mch_id == 0) {
            return mch;
        }

        mch.Id = mch_id;
        mch.Name = "商户" + std::to_string(mch_id);
        return mch;
    }

    AppInfo GetAppInfo(unsigned long mch_id, unsigned long app_id) {
        AppInfo app;
        if (app_id == 0) {
            return app;
        }

        app.Id = app_id;
        app.Name = "小程序" + std::to_string(app_id);
        return app;
    }

    PlanInfo GetPlanInfo(unsigned long mch_id, unsigned long plan_id) {
        PlanInfo plan;
        if (plan_id == 0) {
            return plan;
        }

        plan.Id = plan_id;
        plan.Name = "腾讯视频会员方案" + std::to_string(plan_id);
        plan.Desc = "这里是扣费服务的简介";
        return plan;
    }

    // void GetContractInfoDbFieldList(mysqlx::Row &row, RecurringPayment::ContractInfo *c) {}

    void RowToContractInfo(mysqlx::Row &row, RecurringPayment::ContractInfo &c) {
        mysqlx::Value v_id = row.get(0);
        mysqlx::Value v_user_id = row.get(1);
        mysqlx::Value v_mch_id = row.get(2);
        mysqlx::Value v_app_id = row.get(3);
        mysqlx::Value v_plan_id = row.get(4);
        mysqlx::Value v_contract_code = row.get(5);
        mysqlx::Value v_contract_state = row.get(6);
        mysqlx::Value v_display_account = row.get(7);
        mysqlx::Value v_signed_time = row.get(8);
        mysqlx::Value v_expired_time = row.get(9);
        mysqlx::Value v_terminated_time = row.get(10);

        if (!v_id.isNull()) { c.set_contract_id(uint64_t(v_id)); }
        if (!v_contract_code.isNull()) { c.set_contract_code(mysqlx::string(v_contract_code)); }
        if (!v_display_account.isNull()) { c.set_display_account(mysqlx::string(v_display_account)); }
        if (!v_signed_time.isNull()) { c.set_signed_time(mysqlx::string(v_signed_time)); }
        if (!v_expired_time.isNull()) { c.set_expired_time(mysqlx::string(v_expired_time)); }
        if (!v_terminated_time.isNull()) { c.set_terminated_time(mysqlx::string(v_terminated_time)); }
        if (!v_contract_state.isNull()) {
            c.set_contract_state(RecurringPayment::ContractState(int(v_contract_state)));
        }

        if (!v_user_id.isNull()) {
            UserInfo user = GetUserInfo(uint64_t(v_user_id));
            c.set_user_id(user.Id);
        }
        if (!v_mch_id.isNull()) {
            MchInfo mch = GetMchInfo(uint64_t(v_mch_id));
            c.set_mch_id(mch.Id);
            c.set_mch_name(mch.Name);
            if (!v_app_id.isNull()) {
                AppInfo app = GetAppInfo(mch.Id, uint64_t(v_app_id));
                c.set_app_id(app.Id);
                c.set_app_name(app.Name);
            }
            if (!v_plan_id.isNull()) {
                PlanInfo plan = GetPlanInfo(mch.Id, uint64_t(v_plan_id));
                c.set_plan_id(plan.Id);
                c.set_plan_name(plan.Name);
                c.set_plan_desc(plan.Desc);
            }
        }
    }
}