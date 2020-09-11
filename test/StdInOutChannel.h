#pragma once
#include <zinx.h>

class StdInChannel : public Ichannel
{
public:
    StdInChannel();
    virtual ~StdInChannel();

    virtual bool Init() override;
    virtual bool ReadFd(std::string &_input) override;
    virtual bool WriteFd(std::string &_output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler *GetInputNextStage(BytesMsg &_oInput) override;
};

class StdOutChannel : public Ichannel
{
public:
    virtual bool Init() override;
    virtual bool ReadFd(std::string &_input) override;
    virtual bool WriteFd(std::string &_output) override;
    virtual void Fini() override;
    virtual int GetFd() override;
    virtual std::string GetChannelInfo() override;
    virtual AZinxHandler *GetInputNextStage(BytesMsg &_oInput) override;
};
