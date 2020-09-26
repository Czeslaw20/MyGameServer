#include <string>
#include <stdlib.h>
#include "user_opt.h"
#include <fcgi_stdio.h> //一定写在最后一行

using namespace std;

int main()
{
    while (FCGI_Accept() >= 0)
    {
        char buf[1024] = {0};
        fread(buf, 1, sizeof(buf), stdin);

        string orig_string(buf);

        int and_pos = orig_string.find("\n");

        string username = orig_string.substr(9, and_pos - 10);
        //注意浏览器提交表单时不是用&连接的，而是换行
        //user和password后面均有两个字符，很可能是\r\n
        //取username时注意不要把换行也取了,用户名后面要少取一个，密码后面少取两个
        string password = orig_string.substr(and_pos + 10, orig_string.size() - and_pos - 12);

        printf("Content-Type=text\r\n\r\n");
        if (CheckUser(username.c_str(), NULL) == true)
        {
            //user exist
            printf("User Exist\r\n");
        }
        else
        {
            AddUser(username.c_str(), password.c_str());

            // printf("%s", orig_string.c_str());

            printf("Success\r\n");
        }
    }
}