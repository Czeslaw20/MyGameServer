#pragma once
#include <zinx.h>
using namespace std;

class GameChannel;
class GameRole;
class GameProtocol : public Iprotocol
{
    string szLast;

public:
    GameChannel *m_channel = NULL;
    GameRole *m_Role = NULL;
    GameProtocol();
    virtual ~GameProtocol();

    //继承
    virtual UserData *raw2request(std::string _szInput) override;
    virtual std::string *response2raw(UserData &_oUserData) override;
    virtual Irole *GetMsgProcessor(UserDataMsg &_oUserDataMsg) override;
    virtual Ichannel *GetMsgSender(BytesMsg &_oBytes) override;
};