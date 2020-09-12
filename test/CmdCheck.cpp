#include "CmdCheck.h"
#include "CmdMsg.h"
#include "EchoRole.h"
using namespace std;

CmdCheck *CmdCheck::poSingle = new CmdCheck();

CmdCheck::CmdCheck()
{
}

CmdCheck::~CmdCheck()
{
}

UserData *CmdCheck::raw2request(std::string _szInput)
{
    if (_szInput == "exit")
    {
        ZinxKernel::Zinx_Exit();
        return NULL;
    }

    auto pret = new CmdMsg();
    pret->szUserData = _szInput;

    if (_szInput == "open")
    {
        pret->isCmd = true;
        pret->isOpen = true;
    }
    if (_szInput == "close")
    {
        pret->isCmd = true;
        pret->isOpen = false;
    }
    if (_szInput == "date")
    {
        pret->isCmd = true;
        pret->needDatePre = true;
    }
    if (_szInput == "cleardate")
    {
        pret->isCmd = true;
        pret->needDatePre = false;
    }

    return pret;
}

std::string *CmdCheck::response2raw(UserData &_oUserData)
{
    GET_REF2DATA(CmdMsg, output, _oUserData);
    return new string(output.szUserData);
}

Irole *CmdCheck::GetMsgProcessor(UserDataMsg &_oUserDataMsg)
{
    szOutChannel = _oUserDataMsg.szInfo;
    if (szOutChannel == "stdin")
    {
        szOutChannel = "stdout";
    }

    //根据命令不同，交给不同的处理role对象
    auto rolelist = ZinxKernel::Zinx_GetAllRole();

    auto pCmdMsg = dynamic_cast<CmdMsg *>(_oUserDataMsg.poUserData);
    //读取当前消息是否是命令
    bool isCmd = pCmdMsg->isCmd;

    Irole *pRetRole = NULL;

    for (auto prole : rolelist)
    {
        if (isCmd)
        {
            auto pOutCtrl = dynamic_cast<OutputCtrl *>(prole);
            if (pOutCtrl != NULL)
            {
                pRetRole = pOutCtrl;
                break;
            }
        }
        else
        {
            auto pDate = dynamic_cast<DatePreRole *>(prole);
            if (pDate != NULL)
            {
                pRetRole = pDate;
                break;
            }
        }
    }
    return pRetRole;
}

Ichannel *CmdCheck::GetMsgSender(BytesMsg &_oBytes)
{

    //指定数据要通过标准输出输出
    // return ZinxKernel::Zinx_GetChannel_ByInfo("stdout");

    return ZinxKernel::Zinx_GetChannel_ByInfo(szOutChannel);
}
