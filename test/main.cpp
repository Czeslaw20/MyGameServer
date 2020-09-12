#include <zinx.h>
using namespace std;
#include "StdInOutChannel.h"
#include "EchoRole.h"
#include <ZinxTCP.h>

//创建标准输出通道类
class TestStdout : public Ichannel
{
    virtual bool Init() override
    {
        return true;
    }

    virtual bool ReadFd(std::string &_input) override
    {
        return false;
    }

    virtual bool WriteFd(std::string &_output) override
    {
        cout << _output << endl;
        return true;
    }

    virtual void Fini() override
    {
    }

    virtual int GetFd() override
    {
        //标准输出是1
        return 1;
    }

    virtual std::string GetChannelInfo() override
    {
        return "stdout";
    }

    virtual AZinxHandler *GetInputNextStage(BytesMsg &_oInput) override
    {
        return nullptr;
    }
};
TestStdout *poOut = new TestStdout();

//2-写功能处理类
class Echo : public AZinxHandler
{
    virtual IZinxMsg *InternelHandle(IZinxMsg &_oInput) override
    {
        //回显到标准输出
        GET_REF2DATA(BytesMsg, input, _oInput);

        //cout << input.szData << endl;
        Ichannel *poSendOut = ZinxKernel::Zinx_GetChannel_ByInfo("stdin");
        if (poSendOut != NULL)
        {
            ZinxKernel::Zinx_SendOut(input.szData, *poSendOut);
        }

        return nullptr;
    }

    virtual AZinxHandler *GetNextHandler(IZinxMsg &_oNextMsg) override
    {
        return nullptr;
    }
};
Echo *poEcho = new Echo();

class ExitFramework : public AZinxHandler
{
    virtual IZinxMsg *InternelHandle(IZinxMsg &_oInput) override
    {
        GET_REF2DATA(BytesMsg, obyte, _oInput);
        if (obyte.szData == "exit")
        {
            ZinxKernel::Zinx_Exit();
            return NULL;
        }
        //创建交给下一个环节处理的数据
        return new BytesMsg(obyte);
    }

    virtual AZinxHandler *GetNextHandler(IZinxMsg &_oNextMsg) override
    {
        return poEcho;
    }
};
ExitFramework *poExit = new ExitFramework();

class AddDate : public AZinxHandler
{
    virtual IZinxMsg *InternelHandle(IZinxMsg &_oInput) override
    {
        GET_REF2DATA(BytesMsg, oBytes, _oInput);
        //string szNew = "xiao ming shuo:" + oBytes.szData;
        time_t tmp;
        time(&tmp);
        string szNew = string(ctime(&tmp)) + oBytes.szData;
        BytesMsg *pret = new BytesMsg(oBytes);
        pret->szData = szNew;
        return pret;
    }

    virtual AZinxHandler *GetNextHandler(IZinxMsg &_oNextMsg) override
    {
        return poEcho;
    }
};
AddDate *poAddDate = new AddDate();

class CmdHandler : public AZinxHandler
{
    int status = 0;
    virtual IZinxMsg *InternelHandle(IZinxMsg &_oInput) override
    {
        //判断输入是否是open或close，如果是，执行不同操作
        GET_REF2DATA(BytesMsg, obytes, _oInput);
        if (obytes.szData == "open")
        {
            ZinxKernel::Zinx_Add_Channel(*poOut);
            return NULL;
        }
        else if (obytes.szData == "close")
        {
            ZinxKernel::Zinx_Del_Channel(*poOut);
            return NULL;
        }
        else if (obytes.szData == "date")
        {
            status = 1;
            return nullptr;
        }
        else if (obytes.szData == "cleardate")
        {
            status = 0;
        }

        return new BytesMsg(obytes);
    }

    //根据消息不同选择不同的处理者
    virtual AZinxHandler *GetNextHandler(IZinxMsg &_oNextMsg) override
    {
        GET_REF2DATA(BytesMsg, oBytes, _oNextMsg);
        if (oBytes.szData == "exit")
        {
            return poExit;
        }
        else
        {
            if (status == 0)
            {
                return poEcho;
            }
            else
            {
                return poAddDate;
            }
        }
    }
};
CmdHandler *poCmd = new CmdHandler();

//3-写通道类
class TestStdin : public Ichannel
{
    virtual bool Init() override
    {
        return true;
    }

    virtual bool ReadFd(std::string &_input) override
    {
        cin >> _input;
        return true;
    }

    virtual bool WriteFd(std::string &_output) override
    {
        return false;
    }

    virtual void Fini() override
    {
    }

    virtual int GetFd() override
    {
        //标准输入是0
        return 0;
    }

    virtual std::string GetChannelInfo() override
    {
        return "stdin";
    }

    virtual AZinxHandler *GetInputNextStage(BytesMsg &_oInput) override
    {
        return poCmd;
    }
};

int main()
{
    //1-初始化框架
    ZinxKernel::ZinxKernelInit();

    //4-将通道对象添加到框架
    ZinxKernel::Zinx_Add_Channel(*(new StdInChannel()));
    ZinxKernel::Zinx_Add_Channel(*(new StdOutChannel()));

    ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(51111, new MyFact())));

    ZinxKernel::Zinx_Add_Role(*(new EchoRole()));
    ZinxKernel::Zinx_Add_Role(*(new DatePreRole()));
    ZinxKernel::Zinx_Add_Role(*(new OutputCtrl()));

    //5-运行框架
    ZinxKernel::Zinx_Run();

    ZinxKernel::ZinxKernelFini();
    return 0;
}