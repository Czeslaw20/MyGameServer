#include "ZinxTimer.h"
#include <sys/timerfd.h>

ZinxTimerChannel::ZinxTimerChannel()
{
}
ZinxTimerChannel::~ZinxTimerChannel()
{
}

//创建定时器文件描述符
bool ZinxTimerChannel::Init()
{
    bool bRet = false;
    //创建文件描述符
    int iFd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (iFd >= 0)
    {
        //设置定时周期
        struct itimerspec period = {{1, 0}, {1, 0}};
        if (timerfd_settime(iFd, 0, &period, NULL) == 0)
        {
            bRet = true;
            m_Timerfd = iFd;
        }
    }
    return bRet;
}

//读取超时次数
bool ZinxTimerChannel::ReadFd(std::string &_input)
{
    bool bRet = false;
    char buf[8] = {0};
    if (sizeof(buf) == read(m_Timerfd, buf, sizeof(buf)))
    {
        bRet = true;
        _input.assign(buf, sizeof(buf));
    }
    return bRet;
}

bool ZinxTimerChannel::WriteFd(std::string &_output)
{
    return false;
}

//关闭文件描述符
void ZinxTimerChannel::Fini()
{
    close(m_Timerfd);
    m_Timerfd = -1;
}

//返回当前的定时器文件描述符
int ZinxTimerChannel::GetFd()
{
    return m_Timerfd;
}

std::string ZinxTimerChannel::GetChannelInfo()
{
    return "TimeFd";
}

class output_hello : public AZinxHandler
{
    virtual IZinxMsg *InternelHandle(IZinxMsg &_oInput)
    {
        auto pchannel = ZinxKernel::Zinx_GetChannel_ByInfo("stdout");
        std::string output = "helloworld";
        ZinxKernel::Zinx_SendOut(output, *pchannel);
        return nullptr;
    }

    virtual AZinxHandler *GetNextHandler(IZinxMsg &_oNextMsg)
    {
        return nullptr;
    }
};

output_hello *pout_hello = new output_hello();

//返回处理超时事件的对象
AZinxHandler *ZinxTimerChannel::GetInputNextStage(BytesMsg &_oInput)
{
    return &TimerOutMng::GetInstance();
}

TimerOutMng::TimerOutMng()
{
    //创建10个齿
    for (int i = 0; i < 10; i++)
    {
        list<TimerOutProc *> tmp;
        m_timer_wheel.push_back(tmp);
    }
}

TimerOutMng TimerOutMng::single;
IZinxMsg *TimerOutMng::InternelHandle(IZinxMsg &_oInput)
{
    unsigned long iTimeroutCount = 0; //八字节

    GET_REF2DATA(BytesMsg, obytes, _oInput);
    obytes.szData.copy((char *)&iTimeroutCount, sizeof(iTimeroutCount), 0);

    while (iTimeroutCount-- > 0)
    {
        //移动刻度
        cur_index++;
        cur_index %= 10;
        //遍历当前刻度所有节点，执行处理函数或圈数-1
        list<TimerOutProc *> m_cache;
        for (auto itr = m_timer_wheel[cur_index].begin(); itr != m_timer_wheel[cur_index].end();)
        {
            if ((*itr)->iCount <= 0)
            {
                //缓存待处理的超时节点
                m_cache.push_back(*itr);
                //返回的是下一个元素的迭代器
                auto ptmp = *itr;
                itr = m_timer_wheel[cur_index].erase(itr);
                AddTask(ptmp);
            }
            else
            {
                (*itr)->iCount--;
                ++itr;
            }
        }

        //统一执行待处理超时任务
        for (auto task : m_cache)
        {
            task->Proc();
        }
    }

    return nullptr;
}

AZinxHandler *TimerOutMng::GetNextHandler(IZinxMsg &_oNextMsg)
{
    return nullptr;
}

void TimerOutMng::AddTask(TimerOutProc *_ptask)
{
    //给时间轮添加任务
    //计算当前人物需要放到哪个齿上
    int index = (_ptask->GetTimeSec() + cur_index) % 10;
    //把任务存到齿轮上
    m_timer_wheel[index].push_back(_ptask);
    //计算所需圈数
    _ptask->iCount = _ptask->GetTimeSec() / 10;
}

void TimerOutMng::DelTask(TimerOutProc *_ptask)
{
    //遍历时间轮所有齿，删掉任务
    for (auto &chi : m_timer_wheel) //注意，必须传入引用才能删除
    {
        for (auto task : chi)
        {
            if (task == _ptask)
            {
                chi.remove(_ptask);
                return;
            }
        }
    }
}