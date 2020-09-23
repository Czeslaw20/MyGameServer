#pragma once
#include "Ichannel.h"

class stdout_channel : public Ichannel
{
public:
    stdout_channel();
    virtual ~stdout_channel();

    //继承
    virtual std::string ReadFd() override;
    virtual void WriteFd(std::string _output) override;
    virtual int GetFd() override;

    virtual bool init() override;
    virtual void fini() override;

    virtual ZinxHandler *GetInputNextStage(ByteMsg *_byte) override;
};