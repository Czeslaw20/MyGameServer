#include "TcpDataChannel.h"
#include "zinx.h"

class Echo : public AZinxHandler
{
    virtual IZinxMsg *InternelHandle(IZinxMsg &_oInput) override
    {
        GET_REF2DATA(BytesMsg, input, _oInput);
        auto channel = ZinxKernel::Zinx_GetChannel_ByInfo(input.szInfo);
        ZinxKernel::Zinx_SendOut(input.szData, *channel);
        return nullptr;
    }

    virtual AZinxHandler *GetNextHandler(IZinxMsg &_oNextMsg) override
    {
        return nullptr;
    }
};
Echo echo;

class myTcpData : public TcpDataChannel
{
public:
    myTcpData(int _fd) : TcpDataChannel(_fd) {}
    virtual AZinxHandler *GetInputNextStage(BytesMsg &_oInput) override
    {
        return &echo;
    }
};

class myFact : public TcpDataChannelFactory
{
    virtual TcpDataChannel *CreateChannel(int _fd) override
    {
        return new myTcpData(_fd);
    }
};

int main()
{
    ZinxKernel::ZinxKernelInit();

    ZinxKernel::Zinx_Add_Channel(*(new TcpListenChannel(23333, new myFact())));
    ZinxKernel::Zinx_Run();
    ZinxKernel::ZinxKernelFini();
}