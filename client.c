#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
int main()
{
    //1.创建用于通信的套接字
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket");
        exit(0);
    }

    //客户端的端口是自动分配的
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8899); //服务器监听的端口，网络字节序
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr);

    int ret = connect(fd, (struct sockaddr *)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("connect");
        exit(0);
    }

    //4.通信
    int i = 0;
    while (1)
    {
        char recvBuf[1024];
        scanf("%s", recvBuf);
        // unsigned int recvBuf[1024];
        //写数据

        // scanf("%02x", recvBuf);

        // printf(recvBuf, "data:%s\n");
        // //sprintf(recvBuf, "data:%d\n", i++);
        write(fd, recvBuf, strlen(recvBuf) + 1);
        // write(fd, recvBuf, strlen((const char *)recvBuf));

        //**************************//
        //写数据时长度strlen需要加1
        //因为要把结束标志‘\0’写进去
        //如果不写，服务器读字符串时，这个位置的数据是随机的
        //会随机导致乱码
        //**************************//

        // read(fd, recvBuf, sizeof(recvBuf));
        // printf("recv buf:%s\n", recvBuf);
        // sleep(1);
    }

    //释放资源
    close(fd);

    return 0;
}
//0801120474657374