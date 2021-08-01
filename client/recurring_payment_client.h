#ifndef RECURRING_PAYMENT_DEMO_RECURRING_PAYMENT_CLIENT_H
#define RECURRING_PAYMENT_DEMO_RECURRING_PAYMENT_CLIENT_H

#include "protos/recurring_payment.grpc.pb.h"

using RecurringPayment::RecurringPaymentSvc;

namespace RecurringPaymentClient {
    class Client {
    public:
        explicit Client(const std::shared_ptr<grpc::Channel> &channel) : stub_(
                RecurringPayment::RecurringPaymentSvc::NewStub(channel)) {
            user_id = 0;
        }

        bool Login(unsigned long int uid);

        void ListContract(unsigned int limit, unsigned int offset);

        bool SignContract(std::string contract_code);

        bool TerminateContract(unsigned long int contract_id);

        std::string PrepareContract();

    private:
        std::unique_ptr<RecurringPaymentSvc::Stub> stub_;
        unsigned long int user_id;
    };

    class ClientException : public std::exception {
    public:
        explicit ClientException(std::string &msg) : msg(msg) {}

        const char *what() const throw() override {
            return msg.c_str();
        }

    private:
        std::string msg;
    };
}

#endif //RECURRING_PAYMENT_DEMO_RECURRING_PAYMENT_CLIENT_H