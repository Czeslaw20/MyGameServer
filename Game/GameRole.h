#pragma once
#include <zinx.h>
#include "AOIWorld.h"

using namespace std;

class GameMsg;
class GameProtocol;

class GameRole : public Irole, public Player
{
    float x = 0;
    float y = 0;
    float z = 0;
    float v = 0;

    int iPid = 0;
    string szName;

    GameMsg *CreateIDNameLogin();
    GameMsg *CreatSrdPlayers();
    GameMsg *CreatSelfPosition();
    GameMsg *CreateIDNameLogoff();

    GameMsg *CreateTalkBroadCast(string _content);

    void ProcTalkMsg(string _content);
    void ProcMoveMsg(float _x, float _y, float _z, float _v);

    void ViewAppear(GameRole *_pRole);
    void ViewLost(GameRole *_pRole);

public:
    GameRole();
    virtual ~GameRole();

    //继承
    virtual int GetX() override;
    virtual int GetY() override;

    virtual bool Init() override;
    virtual UserData *ProcMsg(UserData &_poUserData) override;
    virtual void Fini() override;
    GameProtocol *m_pProto = NULL;
};
