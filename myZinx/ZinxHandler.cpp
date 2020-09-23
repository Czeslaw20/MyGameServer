#include "ZinxHandler.h"

ZinxHandler::ZinxHandler()
{
}

ZinxHandler::~ZinxHandler()
{
}

void ZinxHandler::handle(ZinxMsg *_inputMsg)
{
    //1 当前环节处理
    auto pNextMsg = internel_handle(_inputMsg);
    if (pNextMsg != nullptr)
    {
        //2 获取下一个环节
        ZinxHandler *pNextHandler = GetNext(pNextMsg);
        if (pNextHandler != nullptr)
        {
            //3 下一个环节处理
            pNextHandler->handle(pNextMsg);
        }
        delete pNextMsg;
    }
}
