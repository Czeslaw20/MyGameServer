#pragma once
#include "Ichannel.h"
#include <string>

class FIFO_channel : public Ichannel
{
    std::string fifo_name;
    int m_fd = -1;
    bool bIsRead = true;

public:
    FIFO_channel *m_out = NULL;
    FIFO_channel(std::string _file, bool _isRead);
    virtual ~FIFO_channel();

    //继承
    virtual std::string ReadFd() override;

    virtual void WriteFd(std::string _output) override;
    virtual int GetFd() override;

    virtual bool init() override;
    virtual void fini() override;
    virtual ZinxHandler *GetInputNextStage(ByteMsg *_byte) override;
};
