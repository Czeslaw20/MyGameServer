#include <iostream>
#include "zinxkernel.h"
#include "stdin_channel.h"
#include "FIFO_channel.h"
#include "ProcessFunc.h"
#include "ZinxHandler.h"
#include <algorithm>

using namespace std;

class testMsg : public ZinxMsg
{
public:
    string data;
};

class output_next_letter : public ZinxHandler
{
    virtual ZinxMsg *internel_handle(ZinxMsg *_inputMsg) override
    {
        auto pmsg = dynamic_cast<testMsg *>(_inputMsg);
        printf("%c\n", pmsg->data[0] + 1);
        return nullptr;
    }

    virtual ZinxHandler *GetNext(ZinxMsg *_next_input) override
    {
        return nullptr;
    }
};
output_next_letter step3;

class output_ascii : public ZinxHandler
{
    virtual ZinxMsg *internel_handle(ZinxMsg *_inputMsg) override
    {
        auto pmsg = dynamic_cast<testMsg *>(_inputMsg);
        printf("%d\n", pmsg->data[0]);
        //原样交给下一个环节
        testMsg *pret = new testMsg();
        pret->data = pmsg->data;
        return pret;
    }

    virtual ZinxHandler *GetNext(ZinxMsg *_next_input) override
    {
        return &step3;
    }
};
output_ascii step2;

class trancUpper : public ZinxHandler
{
    virtual ZinxMsg *internel_handle(ZinxMsg *_inputMsg) override
    {
        auto pmsg = dynamic_cast<testMsg *>(_inputMsg);
        string output("a"); //要先有空间，否则迭代器无效
        transform(pmsg->data.begin(), pmsg->data.end(), output.begin(), ::toupper);
        cout << output << endl;

        //new新的消息对象返回
        testMsg *pret = new testMsg();
        pret->data = output;
        return pret;
    }

    virtual ZinxHandler *GetNext(ZinxMsg *_next_input) override
    {
        return &step2;
    }
};
trancUpper step1;

void test_chain()
{
    string temp;
    cin >> temp;
    testMsg init_msg;
    init_msg.data = temp;
    step1.handle(&init_msg);
}

int main()
{
    // test_chain();
    stdin_channel in_channel;
    stdout_channel out_channel;

    ProcessFunc pf;

    pf.m_out = &out_channel;
    in_channel.m_proc = &pf;

    zinxkernel &kernel = zinxkernel::GetInstance();
    kernel.AddChannel(&in_channel);
    kernel.AddChannel(&out_channel);

    kernel.run();
}