#include "TcpDataChannel.h"

TcpDataChannel::TcpDataChannel(int _socket) : m_socket(_socket)
{
}

TcpDataChannel::~TcpDataChannel()
{
    if (m_socket >= 0)
    {
        close(m_socket);
    }
}

//继承
bool TcpDataChannel::Init()
{
    return true;
}

bool TcpDataChannel::ReadFd(std::string &_input)
{
    char szBuf[128] = {0};
    int irecvlen = -1;
    bool bRet = false;

    while (1)
    {
        irecvlen = recv(m_socket, szBuf, sizeof(szBuf), MSG_DONTWAIT);
        if (irecvlen == 0)
        {
            //链路断了
            SetChannelClose();
            break;
        }
        else if (irecvlen == -1)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                //链接没断，只是没数据了
                break;
            }
            //同链路断了
            SetChannelClose();
            break;
        }
        else
        {
            _input.append(szBuf, irecvlen);
            bRet = true;
        }
    }
    return bRet;
}

bool TcpDataChannel::WriteFd(std::string &_output)
{
    send(m_socket, _output.data(), _output.size(), 0);
    return true;
}

void TcpDataChannel::Fini()
{
    if (m_socket >= 0)
    {
        close(m_socket);
        m_socket = -1;
    }
}

int TcpDataChannel::GetFd()
{
    return m_socket;
}

std::string TcpDataChannel::GetChannelInfo()
{
    return "data_socket" + m_socket;
}

TcpDataChannel *TcpDataChannelFactoryCreateChannel(int _fd)
{
    return nullptr;
}

//继承
bool TcpListenChannel::Init()
{
    bool bRet = false;

    int isocket = -1;

    isocket = socket(AF_INET, SOCK_STREAM, 0);
    if (isocket >= 0)
    {
        sockaddr_in stServer;
        stServer.sin_family = AF_INET;
        stServer.sin_addr.s_addr = INADDR_ANY;
        stServer.sin_port = htons(m_port);

        //设置允许重复绑定
        int iflag = 1;
        setsockopt(isocket, SOL_SOCKET, SO_REUSEADDR, &iflag, sizeof(iflag));
        if (bind(isocket, (struct sockaddr *)&stServer, sizeof(stServer)) == 0)
        {
            if (listen(isocket, 1000) == 0)
            {
                bRet = true;
                iListenFd = isocket;
            }
            else
            {
                perror(__FILE__ ":"
                                "__func__"
                                ":"
                                "listen");
            }
        }
        else
        {
            perror(__FILE__ ":"
                            "__func__"
                            ":"
                            "bind");
        }
    }
    else
    {
        perror(__FILE__ ":"
                        "__func__"
                        ":"
                        "socket");
    }

    return bRet;
}

bool TcpListenChannel::ReadFd(std::string &_input)
{
    bool bRet = false;
    struct sockaddr_in stClient;
    socklen_t socklen = sizeof(stClient);

    int iDataFd = accept(iListenFd, (struct sockaddr *)&stClient, &socklen);
    if (iDataFd >= 0)
    {
        auto pDataChannel = m_factory->CreateChannel(iDataFd);
        bRet = ZinxKernel::Zinx_Add_Channel(*pDataChannel);
    }
    else
    {
        perror(__FILE__ ":"
                        "__func__"
                        ":"
                        "accept");
    }
    return bRet;
}

bool TcpListenChannel::WriteFd(std::string &_output)
{
    return false;
}

void TcpListenChannel::Fini()
{
    if (iListenFd >= 0)
    {
        close(iListenFd);
    }
}

int TcpListenChannel::GetFd()
{
    return iListenFd;
}

std::string TcpListenChannel::GetChannelInfo()
{
    return "TCP_LISTEN";
}

AZinxHandler *TcpListenChannel::GetInputNextStage(BytesMsg &_oInput)
{
    return nullptr;
}

//第二个参数的内存被该类吸收
TcpListenChannel::TcpListenChannel(unsigned short port, TcpDataChannelFactory *_pfact) : m_port(port), m_factory(_pfact)
{
}

TcpListenChannel::~TcpListenChannel()
{
    if (m_factory != NULL)
    {
        delete m_factory;
    }
}