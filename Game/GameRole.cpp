#include "GameRole.h"
#include "GameMsg.h"
#include <iostream>
#include "msg.pb.h"
#include "AOIWorld.h"
#include "GameChannel.h"
#include <algorithm>
#include <random>
#include "ZinxTimer.h"
#include "RandomName.h"
#include <fstream>
#include <hiredis/hiredis.h>

using namespace std;

RandomName random_name;

//创建游戏世界全局对象
static AOIWorld world(0, 400, 0, 400, 20, 20);
//static使作用域变窄，只在当前文件中可见

void GameRole::ProcTalkMsg(string _content)
{
    //发给所有人
    auto role_list = ZinxKernel::Zinx_GetAllRole();
    for (auto pRole : role_list)
    {
        auto pGameRole = dynamic_cast<GameRole *>(pRole);
        auto pmsg = CreateTalkBroadCast(_content);
        ZinxKernel::Zinx_SendOut(*pmsg, *(pGameRole->m_pProto));
    }
}

void GameRole::ProcMoveMsg(float _x, float _y, float _z, float _v)
{
    //1.跨网格处理
    //获取原来的邻居S1
    auto s1 = world.GetSrdPlayers(this);
    //摘出旧格子，更新坐标，添加新格子，获取新邻居
    world.DelPlayer(this);
    x = _x;
    y = _y;
    z = _z;
    v = _v;
    world.AddPlayer(this);
    auto s2 = world.GetSrdPlayers(this);
    //遍历s2，若元素不属于s1，视野出现
    for (auto single_player : s2)
    {
        if (find(s1.begin(), s1.end(), single_player) == s1.end())
        {
            //视野出现
            ViewAppear(dynamic_cast<GameRole *>(single_player));
        }
    }
    //遍历s1，若元素不属于s2，视野消失
    for (auto single_player : s1)
    {
        if (find(s2.begin(), s2.end(), single_player) == s2.end())
        {
            //视野消失
            ViewLost(dynamic_cast<GameRole *>(single_player));
        }
    }

    //2.广播新位置给周围玩家

    //遍历周围玩家发送
    auto srd_list = world.GetSrdPlayers(this);
    for (auto single : srd_list)
    {
        //组成待发送的报文
        pb::BroadCast *pMsg = new pb::BroadCast();
        auto pPos = pMsg->mutable_p();
        pPos->set_x(_x);
        pPos->set_y(_y);
        pPos->set_z(_z);
        pPos->set_v(_v);
        pMsg->set_pid(iPid);
        pMsg->set_tp(4);
        pMsg->set_username(szName);
        auto pRole = dynamic_cast<GameRole *>(single);
        ZinxKernel::Zinx_SendOut(*(new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg)), *(pRole->m_pProto));
    }
}

void GameRole::ViewAppear(GameRole *_pRole)
{
    //向自己发对方的200消息
    auto pmsg = _pRole->CreatSelfPosition();
    ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
    //向参数玩家发自己的200消息
    pmsg = CreatSelfPosition();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_pProto));
}

void GameRole::ViewLost(GameRole *_pRole)
{
    //向自己发对方的201消息
    auto pmsg = _pRole->CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
    //向参数玩家发自己的201消息
    pmsg = CreateIDNameLogoff();
    ZinxKernel::Zinx_SendOut(*pmsg, *(_pRole->m_pProto));
}

GameMsg *GameRole::CreateIDNameLogin()
{

    pb::SyncPid *pmsg = new pb::SyncPid();
    pmsg->set_pid(iPid);
    pmsg->set_username(szName);
    GameMsg *pRet = new GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);

    return pRet;
}

GameMsg *GameRole::CreatSrdPlayers()
{
    pb::SyncPlayers *pMsg = new pb::SyncPlayers();
    auto srd_list = world.GetSrdPlayers(this);
    for (auto single : srd_list)
    {
        auto pPlayer = pMsg->add_ps();
        auto pRole = dynamic_cast<GameRole *>(single);
        pPlayer->set_pid(pRole->iPid);
        pPlayer->set_username(pRole->szName);
        auto pPosition = pPlayer->mutable_p();
        pPosition->set_x(pRole->x);
        pPosition->set_y(pRole->y);
        pPosition->set_z(pRole->z);
        pPosition->set_v(pRole->v);
    }

    GameMsg *pret = new GameMsg(GameMsg::MSG_TYPE_SRD_POSITION, pMsg);
    return pret;
}

GameMsg *GameRole::CreatSelfPosition()
{
    pb::BroadCast *pMsg = new pb::BroadCast();
    pMsg->set_pid(iPid);
    pMsg->set_username(szName);
    pMsg->set_tp(2);

    auto pPosition = pMsg->mutable_p();
    pPosition->set_x(x);
    pPosition->set_y(y);
    pPosition->set_z(z);
    pPosition->set_v(v);

    GameMsg *pret = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg);
    return pret;
}

GameMsg *GameRole::CreateIDNameLogoff()
{
    pb::SyncPid *pmsg = new pb::SyncPid();
    pmsg->set_pid(iPid);
    pmsg->set_username(szName);
    GameMsg *pRet = new GameMsg(GameMsg::MSG_TYPE_LOGOFF_ID_NAME, pmsg);

    return pRet;
}

GameMsg *GameRole::CreateTalkBroadCast(string _content)
{
    pb::BroadCast *pmsg = new pb::BroadCast();
    pmsg->set_pid(iPid);
    pmsg->set_username(szName);
    pmsg->set_tp(1);
    pmsg->set_content(_content);
    GameMsg *pRet = new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pmsg);

    return pRet;
}

static default_random_engine random_engine(time(NULL));

GameRole::GameRole()
{
    szName = random_name.GetName();
    uniform_int_distribution<unsigned> u(100, 150);
    x = u(random_engine);
    z = u(random_engine);
    // x = 100 + random_engine() % 50;
    // z = 100 + random_engine() % 50;
}

GameRole::~GameRole()
{
    random_name.Release(szName);
}

class ExitTimer : public TimerOutProc
{
    //继承
    virtual void Proc() override
    {
        ZinxKernel::Zinx_Exit();
    }
    virtual int GetTimeSec() override
    {
        return 20;
    }
};

static ExitTimer g_exit_timer;
//继承
bool GameRole::Init()
{
    if (ZinxKernel::Zinx_GetAllRole().size() <= 0)
    {
        TimerOutMng::GetInstance().DelTask(&g_exit_timer);
    }

    //添加自己到游戏世界
    bool bRet = false;
    //设置玩家ID为当前连接fd
    iPid = m_pProto->m_channel->GetFd();

    bRet = world.AddPlayer(this);

    if (bRet == true)
    {
        //向自己发送ID和名称
        auto pmsg = CreateIDNameLogin();
        ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
        //向自己发送周围玩家的位置
        pmsg = CreatSrdPlayers();
        ZinxKernel::Zinx_SendOut(*pmsg, *m_pProto);
        //向周围玩家发送自己的位置
        auto srd_list = world.GetSrdPlayers(this);
        for (auto single : srd_list)
        {
            pmsg = CreatSelfPosition();
            auto pRole = dynamic_cast<GameRole *>(single);
            ZinxKernel::Zinx_SendOut(*pmsg, *(pRole->m_pProto));
        }
    }

    //记录当前姓名到redis的game_name
    //1 连接redis
    auto contest = redisConnect("127.0.0.1", 6379);
    if (contest != NULL)
    {
        freeReplyObject(redisCommand(contest, "lpush game_name %s", szName.c_str()));
        redisFree(contest);
    }

    //2 发送lpush命令

    // ofstream name_record("/tmp/name_record", ios::app);
    // name_record << szName << endl;

    return bRet;
}

//处理游戏相关的用户请求
UserData *GameRole::ProcMsg(UserData &_poUserData)
{
    //测试：打印消息内容
    GET_REF2DATA(MultiMsg, input, _poUserData);
    for (auto single : input.m_Msgs)
    {
        // cout << "type is " << single->enMsgType << endl;
        // cout << "id: " << dynamic_cast<pb::SyncPid *>(single->pMsg)->pid() << endl;
        // cout << "username: " << dynamic_cast<pb::SyncPid *>(single->pMsg)->username() << endl;
        // DebugString方法不知道为什么会出错，因此进行了类型转换后打印

        auto NewPos = dynamic_cast<pb::Position *>(single->pMsg);
        switch (single->enMsgType)
        {
        case GameMsg::MSG_TYPE_CHAT_CONTENT:
            ProcTalkMsg(dynamic_cast<pb::Talk *>(single->pMsg)->content());
            break;
        case GameMsg::MSG_TYPE_NEW_POSITION:
            ProcMoveMsg(NewPos->x(), NewPos->y(), NewPos->z(), NewPos->v());
            break;
        default:
            break;
        }

        if (single->enMsgType == GameMsg::MSG_TYPE_NEW_POSITION)
        {
            //1.跨网格处理
            //2.广播新位置给周围玩家
            //取出新位置

            //遍历周围玩家发送
            auto srd_list = world.GetSrdPlayers(this);
            for (auto single : srd_list)
            {
                //组成待发送的报文
                pb::BroadCast *pMsg = new pb::BroadCast();
                auto pPos = pMsg->mutable_p();
                pPos->set_x(NewPos->x());
                pPos->set_y(NewPos->y());
                pPos->set_z(NewPos->z());
                pPos->set_v(NewPos->v());
                pMsg->set_pid(iPid);
                pMsg->set_tp(4);
                pMsg->set_username(szName);
                auto pRole = dynamic_cast<GameRole *>(single);
                ZinxKernel::Zinx_SendOut(*(new GameMsg(GameMsg::MSG_TYPE_BROADCAST, pMsg)), *(pRole->m_pProto));
            }
        }
    }

    return nullptr;
}

void GameRole::Fini()
{
    //向周围玩家发送下线消息
    auto srd_list = world.GetSrdPlayers(this);
    for (auto single : srd_list)
    {
        auto pMsg = CreateIDNameLogoff();
        auto pRole = dynamic_cast<GameRole *>(single);
        ZinxKernel::Zinx_SendOut(*pMsg, *(pRole->m_pProto));
    }
    world.DelPlayer(this);

    //判断是否是最后一个玩家--->起定时器
    if (ZinxKernel::Zinx_GetAllRole().size() <= 1)
    {
        //起退出定时器
        TimerOutMng::GetInstance().AddTask(&g_exit_timer);
    }

    //从redis game_name中删掉当前姓名
    auto contest = redisConnect("127.0.0.1", 6379);
    if (contest != NULL)
    {
        freeReplyObject(redisCommand(contest, "lrem game_name 1 %s", szName.c_str()));
        redisFree(contest);
    }
}

int GameRole::GetX()
{
    return (int)x;
}

int GameRole::GetY()
{
    return (int)z;
}

//从文件中删掉当前姓名
// //1 从文件中读到所有姓名
// list<string> cur_name_list;
// ifstream input_stream("/tmp/name_record");
// string tmp;
// while (getline(input_stream, tmp))
// {
//     cur_name_list.push_back(tmp);
// }
// //2 删掉当前姓名

// //3 写入其余姓名
// ofstream output_stream("/tmp/name_record");
// for (auto name : cur_name_list)
// {
//     if (name != szName)
//     {
//         output_stream << name << endl;
//     }
// }
