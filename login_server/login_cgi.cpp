#include <string>
#include "user_opt.h"
#include "CJsonObject.hpp"
#include "cJSON.h"

#include <fcgi_stdio.h>

using namespace std;

int main()
{
    while (FCGI_Accept() >= 0)
    {
        string username;
        string password;

        int json_len = atoi(getenv("CONTENT_LENGTH"));
        char *buf = (char *)calloc(1UL, json_len);

        fread(buf, 1, json_len, stdin);
        string json_string(buf);

        neb::CJsonObject json(json_string);
        json.Get("username", username);
        json.Get("password", password);

        printf("Content-Type:application/json\r\n");

        neb::CJsonObject reply;
        if (CheckUser(username.c_str(), password.c_str()) == true)
        {
            //reply ok to client
            reply.Add("login_result", "OK");
        }
        else
        {
            //reply failed to client
            reply.Add("login_result", "Failed");
        }
        string reply_string = reply.ToFormattedString();
        printf("Content-Length:%d\r\n\r\n", reply_string.size());
        printf("%s\r\n", reply_string.c_str());

        free(buf);
    }
}