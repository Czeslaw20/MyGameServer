#include "EchoRole.h"
#include "CmdMsg.h"
#include "CmdCheck.h"
using namespace std;

EchoRole::EchoRole()
{
}

EchoRole::~EchoRole()
{
}

bool EchoRole::Init()
{
    return true;
}

UserData *EchoRole::ProcMsg(UserData &_poUserData)
{
    //读出要回显的内容
    // GET_REF2DATA(CmdMsg, input, _poUserData);
    // auto szData = input.szUserData;

    //写出去
    GET_REF2DATA(CmdMsg, input, _poUserData);
    CmdMsg *pout = new CmdMsg(input);
    ZinxKernel::Zinx_SendOut(*pout, *(CmdCheck::GetInstance()));
    return nullptr;
}

void EchoRole::Fini()
{
}

bool OutputCtrl::Init()
{
    Irole *pRetRole = NULL;
    //设置下一个处理环节是date前缀对象
    for (auto pRole : ZinxKernel::Zinx_GetAllRole())
    {
        auto pDate = dynamic_cast<DatePreRole *>(pRole);
        if (pDate != NULL)
        {
            pRetRole = pDate;
            break;
        }
    }
    if (pRetRole != NULL)
    {
        SetNextProcessor(*pRetRole);
    }
    return true;
}

UserData *OutputCtrl::ProcMsg(UserData &_poUserData)
{
    //读取用户设置的开关标志
    GET_REF2DATA(CmdMsg, input, _poUserData);
    if (input.isOpen == true)
    {
        if (pOut != NULL)
        {
            //开输出
            ZinxKernel::Zinx_Add_Channel(*(pOut));
            pOut = NULL;
        }
    }
    else
    {
        //关输出
        auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
        ZinxKernel::Zinx_Del_Channel(*pchannel);
        pOut = pchannel;
    }
    //开或关输出
    return new CmdMsg(input);
}

void OutputCtrl::Fini()
{
}

bool DatePreRole::Init()
{
    Irole *pRetRole = NULL;
    //设置下一个处理环节是echo对象
    for (auto pRole : ZinxKernel::Zinx_GetAllRole())
    {
        auto pEcho = dynamic_cast<EchoRole *>(pRole);
        if (pEcho != NULL)
        {
            pRetRole = pEcho;
            break;
        }
    }
    if (pRetRole != NULL)
    {
        SetNextProcessor(*pRetRole);
    }

    return true;
}

UserData *DatePreRole::ProcMsg(UserData &_poUserData)
{
    //如果用户数据是命令，改变标志位
    CmdMsg *pret = NULL;
    GET_REF2DATA(CmdMsg, input, _poUserData);
    if (input.isCmd)
    {
        needAdd = input.needDatePre;
    }
    else
    {
        //如果是需要回显的内容，添加字符串并交给回显功能对象
        if (needAdd == true)
        {
            time_t tmp;
            time(&tmp);
            string szNew = string(ctime(&tmp)) + input.szUserData;
            pret = new CmdMsg(input);
            pret->szUserData = szNew;
        }
        else
        {
            pret = new CmdMsg(input);
        }
    }

    return pret;
}

void DatePreRole::Fini()
{
}
