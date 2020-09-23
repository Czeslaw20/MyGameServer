#include "stdout_channel.h"
#include <iostream>

using namespace std;

stdout_channel::stdout_channel()
{
}

stdout_channel::~stdout_channel()
{
}

std::string stdout_channel::ReadFd()
{
    return std::string();
}

void stdout_channel::WriteFd(std::string _output)
{
    cout << _output << endl;
}

int stdout_channel::GetFd()
{
    return 1;
}

bool stdout_channel::init()
{
    return true;
}

void stdout_channel::fini()
{
}

ZinxHandler *stdout_channel::GetInputNextStage(ByteMsg *_byte)
{
    return nullptr;
}