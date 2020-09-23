#pragma once
#include "zinx.h"

class TcpDataChannel : public Ichannel
{
    int m_socket = -1;

public:
    TcpDataChannel(int _socket);
    virtual ~TcpDataChannel();

    //继承
    virtual bool Init() override;
    virtual bool ReadFd(std::string &_input) override;
    virtual bool WriteFd(std::string &_output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
};

class TcpDataChannelFactory
{
public:
    virtual TcpDataChannel *CreateChannel(int _fd) = 0;
};

class TcpListenChannel : public Ichannel
{
    TcpDataChannelFactory *m_factory = NULL;
    unsigned short m_port;
    int iListenFd = -1;

    //继承
    virtual bool Init() override;
    virtual bool ReadFd(std::string &_input) override;
    virtual bool WriteFd(std::string &_output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler *GetInputNextStage(BytesMsg &_oInput) override;

public:
    TcpListenChannel(unsigned short port, TcpDataChannelFactory *_pfact);
    virtual ~TcpListenChannel();
};
