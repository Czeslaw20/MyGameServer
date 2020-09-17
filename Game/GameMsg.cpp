#include "GameMsg.h"
#include "msg.pb.h"

//已知消息内容创建消息对象
GameMsg::GameMsg(MSG_TYPE _type, google::protobuf::Message *_pMsg) : enMsgType(_type), pMsg(_pMsg)
{
}
//将字节流内容转换成消息结构
GameMsg::GameMsg(MSG_TYPE _type, string _stream) : enMsgType(_type)
{
    //通过简单工厂构造具体的消息对象

    switch (_type)
    {
    case GameMsg::MSG_TYPE_LOGIN_ID_NAME:
        pMsg = new pb::SyncPid();
        break;
    case GameMsg::MSG_TYPE_CHAT_CONTENT:
        pMsg = new pb::Talk();
        break;
    case GameMsg::MSG_TYPE_NEW_POSITION:
        pMsg = new pb::Position();
        break;
    case GameMsg::MSG_TYPE_BROADCAST:
        pMsg = new pb::BroadCast();
        break;
    case GameMsg::MSG_TYPE_LOGOFF_ID_NAME:
        pMsg = new pb::SyncPid();
        break;
    case GameMsg::MSG_TYPE_SRD_POSITION:
        pMsg = new pb::SyncPlayers();
        break;
    default:
        break;
    }

    //将参数解析成消息对象内容
    pMsg->ParseFromString(_stream);
}

string GameMsg::serialize()
{
    string ret;
    pMsg->SerializeToString(&ret);
    return ret;
}

GameMsg::~GameMsg()
{
    if (pMsg != NULL)
    {
        delete pMsg;
    }
}