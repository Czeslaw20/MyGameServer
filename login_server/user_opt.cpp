#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string>

using namespace std;

void AddUser(const char *_name, const char *_password)
{
    char buf[1024] = {0};
    sprintf(buf, "./add_user.sh %s %s", _name, _password);
    system(buf);
}

void AddUserMd5(const char *_name, const char *_password)
{
    char buf[1024] = {0};
    sprintf(buf, "echo '%s:%s' >> userfile", _name, _password);
    system(buf);
}

bool CheckUser(const char *_name, const char *_password_dgst)
{
    bool bRet = false;

    if (fork() > 0)
    {
        int iStatus = 0;
        wait(&iStatus);
        if (iStatus == 0)
        {
            bRet = true;
        }
    }
    else
    {
        execlp("./check_user.sh", "./check_user.sh", _name, _password_dgst, NULL);
    }

    return bRet;
}

void DelUser(const char *_name)
{
    char buf[1024] = {0};
    //注意要把脚本文件权限改为755，否则用不了！！！！！
    sprintf(buf, "./del_user.sh %s", _name);
    system(buf);
}

string create_container()
{
    string ret;
    FILE *pf = popen("./create_room.sh", "r");
    if (pf != NULL)
    {
        char buf[1024];
        fread(buf, 1, sizeof(buf), pf);
        ret.append(buf);
        ret.pop_back();
        pclose(pf);
    }
    return ret;
}

bool check_room(const char *_room_no)
{
    bool bRet = false;

    if (fork() > 0)
    {
        int iStatus = 0;
        wait(&iStatus);
        if (iStatus == 0)
        {
            bRet = true;
        }
    }
    else
    {
        execlp("./check_room.sh", "./check_room.sh", _room_no, NULL);
    }
    return bRet;
}