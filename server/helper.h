#ifndef RECURRING_PAYMENT_DEMO_HELPER_H
#define RECURRING_PAYMENT_DEMO_HELPER_H

#include "protos/recurring_payment.grpc.pb.h"
#include <string>
#include <mysqlx/xdevapi.h>

namespace Helper {
    struct UserInfo {
        unsigned long Id{};
        std::string Name{};
    };

    struct MchInfo {
        unsigned long Id{};
        std::string Name{};
    };

    struct AppInfo {
        unsigned long Id{};
        std::string Name{};
    };

    struct PlanInfo {
        unsigned long Id{};
        std::string Name{};
        std::string Desc{};
    };

    UserInfo GetUserInfo(unsigned long user_id);

    MchInfo GetMchInfo(unsigned long mch_id);

    AppInfo GetAppInfo(unsigned long mch_id, unsigned long app_id);

    PlanInfo GetPlanInfo(unsigned long mch_id, unsigned long plan_id);

    void RowToContractInfo(mysqlx::Row &row, RecurringPayment::ContractInfo &c);
}

#endif //RECURRING_PAYMENT_DEMO_HELPER_H
