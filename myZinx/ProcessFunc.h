#pragma once
#include <string>
#include "Ichannel.h"
#include "ZinxHandler.h"

class ProcessFunc : public ZinxHandler
{
public:
    Ichannel *m_out = NULL;
    ProcessFunc();
    virtual ~ProcessFunc();

    virtual ZinxMsg *internel_handle(ZinxMsg *_inputMsg) override;
    virtual ZinxHandler *GetNext(ZinxMsg *_next_input) override;
};
