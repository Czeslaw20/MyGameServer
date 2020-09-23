#pragma once
#include <string>
#include "Ichannel.h"
#include "stdout_channel.h"
#include "ProcessFunc.h"

class stdin_channel : public Ichannel
{

public:
    stdin_channel();
    virtual ~stdin_channel();
    ProcessFunc *m_proc = NULL;

    virtual std::string ReadFd() override;

    virtual void WriteFd(std::string _output) override;
    virtual int GetFd() override;

    virtual bool init() override;
    virtual void fini() override;

    virtual ZinxHandler *GetInputNextStage(ByteMsg *_byte) override;
};
