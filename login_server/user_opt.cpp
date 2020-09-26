#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void AddUser(const char *_name, const char *_password)
{
    char buf[1024] = {0};
    sprintf(buf, "./add_user.sh %s %s", _name, _password);
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