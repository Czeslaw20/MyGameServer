#pragma once
#include <string>
#include "Ichannel.h"
#include "stdout_channel.h"

class stdin_channel : public Ichannel
{

public:
    stdout_channel *m_out = NULL;

    stdin_channel();
    virtual ~stdin_channel();

    virtual std::string ReadFd() override;

    virtual void WriteFd(std::string _output) override;
    virtual int GetFd() override;
    virtual void data_process(std::string _input) override;

    virtual bool init() override;
    virtual void fini() override;
};
