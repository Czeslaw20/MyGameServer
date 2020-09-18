#include "GameChannel.h"
#include "GameMsg.h"
#include "msg.pb.h"
#include "AOIWorld.h"
#include "ZinxTimer.h"
#include "RandomName.h"

extern RandomName random_name;

void daemonlize()
{
    //1 fork
    int iPid = fork();
    if (iPid < 0)
    {
        exit(-1);
    }
    if (iPid > 0)
    {
        //2 父进程退出
        exit(0);
    }
    //3 子进程 设置会话ID
    setsid();
    //4 子进程 设置指向路径

    //5 子进程 重定向3个文件描述符到/dev/null
    int nullfd = open("/dev/null", O_RDWR);
    if (nullfd >= 0)
    {
        dup2(nullfd, 0);
        dup2(nullfd, 1);
        dup2(nullfd, 2);

        close(nullfd);
    }

    //进程监控
    while (1)
    {
        int pid = fork();
        if (pid < 0)
        {
            exit(-1);
        }

        //父进程等子进程退出
        if (pid > 0)
        {
            int iStatus = 0;
            wait(&iStatus);
            if (iStatus == 0)
            {
                //20s正常关闭时，不要fork新进程，直接退出
                //正常退出的退出状态为0
                exit(0);
            }
        }
        //子进程跳出循环执行游戏业务
        else
        {
            break;
        }
    }
}

int main()
{
    daemonlize();
    random_name.LoadFile();

    ZinxKernel::ZinxKernelInit();

    //添加监听通道
    ZinxKernel::Zinx_Add_Channel(*(new ZinxTCPListen(8899, new GameConnFact())));
    ZinxKernel::Zinx_Add_Channel(*(new ZinxTimerChannel()));
    ZinxKernel::Zinx_Run();

    ZinxKernel::ZinxKernelFini();
    return 0;
}

// class myPlayer : public Player
// {
// public:
//     myPlayer(int _x, int _y, string _name) : x(_x), y(_y), name(_name) {}
//     int x;
//     int y;
//     string name;
//     virtual int GetX() override
//     {
//         return x;
//     }

//     virtual int GetY() override
//     {
//         return y;
//     }
// };

// pb::SyncPid *pmsg = new pb::SyncPid();
// pmsg->set_pid(1);
// pmsg->set_username("test");

// GameMsg gm(GameMsg::MSG_TYPE_LOGIN_ID_NAME, pmsg);
// auto output = gm.serialize();

// for (auto byte : output)
// {
//     printf("%02X ", byte);
// }
// puts("");

// char buf[] = {0x08, 0x01, 0x12, 0x04, 0x74, 0x65, 0x73, 0x74};
// string input(buf, sizeof(buf));

// auto ingm = GameMsg(GameMsg::MSG_TYPE_LOGIN_ID_NAME, input);
// cout << dynamic_cast<pb::SyncPid *>(ingm.pMsg)->pid() << endl;
// cout << dynamic_cast<pb::SyncPid *>(ingm.pMsg)->username() << endl;
//--------------------------------------------------------------
// AOIWorld w(20, 200, 50, 230, 6, 6);
// myPlayer p1(60, 107, "1");
// myPlayer p2(91, 118, "2");
// myPlayer p3(147, 133, "3");
// w.AddPlayer(&p1);
// w.AddPlayer(&p2);
// w.AddPlayer(&p3);

// auto srd_list = w.GetSrdPlayers(&p1);
// for (auto single : srd_list)
// {
//     cout << dynamic_cast<myPlayer *>(single)->name << endl;
// }
//---------------------------------------------------------------
