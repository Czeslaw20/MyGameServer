#include "Ichannel.h"
#include "zinxkernel.h"

Ichannel::Ichannel()
{
}

Ichannel::~Ichannel()
{
}

void Ichannel::flushout()
{
    WriteFd(m_buffer);
    m_buffer.clear();
}

//输出数据->将参数记录到缓存
void Ichannel::data_sendout(std::string _output)
{
    m_buffer.append(_output.begin(), _output.end());
    //修改epoll监听类型->添加out方向的监听
    zinxkernel::GetInstance().ModChannel_AddOut(this);
}

ZinxMsg *Ichannel::internel_handle(ZinxMsg *_inputMsg)
{
    //判断输入消息方向是什么
    sysIODicMsg *pmsg = dynamic_cast<sysIODicMsg *>(_inputMsg);
    if (pmsg != nullptr)
    {
        if (pmsg->m_DIC == sysIODicMsg::IO_IN)
        {
            auto input_data = ReadFd();
            return new ByteMsg(input_data, *pmsg);
        }
        else
        {
            //来自业务处理的输出
            auto pout = dynamic_cast<ByteMsg *>(_inputMsg);
            if (pout != NULL)
            {
                data_sendout(pout->content);
            }
        }
    }
    return nullptr;
}

ZinxHandler *Ichannel::GetNext(ZinxMsg *_next_input)
{
    auto pbyte = dynamic_cast<ByteMsg *>(_next_input);
    if (pbyte != nullptr)
    {
        return GetInputNextStage(pbyte);
    }
    return nullptr;
}