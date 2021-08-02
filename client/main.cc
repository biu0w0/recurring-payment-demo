#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>
#include "recurring_payment_client.h"

using namespace std;

// 打印可用命令
void PrintCommandList() {
    cout << "可用命令列表:" << endl;
    cout << "\thelp\t打印指令列表" << endl;
    cout << "\tlogin\t登录，绑定user_id" << endl;
    cout << "\tlist\t查看已签约协议列表（仅列出有效协议）" << endl;
    cout << "\tsign\t签约协议" << endl;
    cout << "\tterm\t解约协议" << endl;
    cout << "\tprepare\t模拟商户准备签约协议，获取待签约协议token" << endl;
    cout << "\tquit\t退出程序" << endl;
}

int Interact(RecurringPaymentClient::Client &client) {
    string cmd;
    try {
        cout << "输入指令: ";
        cin >> cmd;

        if (cmd == "exit" || cmd == "quit") {
            return 1;
        }

        if (cmd == "help") {
            PrintCommandList();
            return 0;
        }

        if (cmd == "login") {
            string input;
            cout << "请输入user_id: ";
            cin >> input;
            unsigned long user_id = strtoul(input.c_str(), nullptr, 10);
            if (user_id == 0) {
                cout << "输入有误，应为纯数字" << endl;
                return 0;
            }
            cout << (client.Login(user_id) ? "登录成功" : "登录失败") << endl;
            return 0;
        }

        if (cmd == "list") {
            client.ListContract(20, 0);
            return 0;
        }

        if (cmd == "sign") {
            string contract_token;
            cout << "请输入待签约协议号: ";
            cin >> contract_token;
            cout << (client.SignContract(contract_token) ? "签约成功" : "签约失败") << endl;
            return 0;
        }

        if (cmd == "term") {
            string input;
            cout << "请输入要解约的协议ID: ";
            cin >> input;
            unsigned long contract_id = strtoul(input.c_str(), nullptr, 10);
            if (contract_id == 0) {
                cout << "输入有误，应为纯数字" << endl;
                return 0;
            }
            cout << (client.TerminateContract(contract_id) ? "解约成功" : "解约失败") << endl;
            return 0;
        }

        if (cmd == "prepare") {
            string contract_token = client.PrepareContract();
            if (contract_token.empty()) {
                cout << "准备签约失败" << endl;
                return 0;
            }
            cout << "待签约协议号: " << contract_token << endl;
            cout << "你可以使用`sign`指令完成签约" << endl;
            return 0;
        }

        cout << "未知指令: " << cmd << endl;
        PrintCommandList();
    } catch (exception &e) {
        cout << e.what() << endl;
    }
    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        cout << "Usage: recurring_payment_client <server_ip>:<server_port>" << endl;
        return 0;
    }

    string server_address = argv[1];
    cout << "当前服务端: " << server_address << endl;

    // DEMO不使用凭据，正式可选用JWT/AccessToken(OAuth2.0)作为凭据
    const shared_ptr<grpc::ChannelCredentials> &credentials = grpc::InsecureChannelCredentials();
    RecurringPaymentClient::Client client(grpc::CreateChannel(server_address, credentials));

    PrintCommandList();
    while (Interact(client) == 0) {
        cout << endl;
    }

    return 0;
}
