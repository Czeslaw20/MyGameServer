#pragma once
#include <zinx.h>
#include <list>
#include <vector>
using namespace std;

class ZinxTimerChannel : public Ichannel
{
    int m_Timerfd = -1;

public:
    ZinxTimerChannel();
    virtual ~ZinxTimerChannel();

    //创建定时器文件描述符
    virtual bool Init() override;
    //读取超时次数
    virtual bool ReadFd(std::string &_input) override;

    virtual bool WriteFd(std::string &_output) override;
    //关闭文件描述符
    virtual void Fini() override;
    //返回当前的定时器文件描述符
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler *GetInputNextStage(BytesMsg &_oInput) override;
};

class TimerOutProc
{
public:
    virtual void Proc() = 0;      //超时之后的处理
    virtual int GetTimeSec() = 0; //定时时间
    int iCount = -1;              //所剩圈数
};

class TimerOutMng : public AZinxHandler
{
    //list<TimerOutProc *> m_task_list;
    vector<list<TimerOutProc *>> m_timer_wheel;
    int cur_index = 0;
    static TimerOutMng single;
    TimerOutMng();

public:
    //处理超时事件，遍历所有超时任务
    virtual IZinxMsg *InternelHandle(IZinxMsg &_oInput) override;
    virtual AZinxHandler *GetNextHandler(IZinxMsg &_oNextMsg) override;

    void AddTask(TimerOutProc *_ptask);
    void DelTask(TimerOutProc *_ptask);
    static TimerOutMng &GetInstance()
    {
        return single;
    }
};
