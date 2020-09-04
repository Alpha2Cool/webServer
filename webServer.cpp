#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
using namespace std;


void handle_connect(int r_acc)
{
    //接收客户端法来的信息
    char buff[1024*50] = {0};
    int r_con = read(r_acc, buff, sizeof buff);
    if(r_con==-1) cout << "读取失败" << endl;
    else cout << "接收到客户端发送的请求: " << buff << endl; 

    //解析客户端请求的文件名
    char filename[20] = {0};
    sscanf(buff, "GET /%s", filename);
    cout << "解析出的文件名为: " << filename << endl;

    //响应客户端请求
    char* mime = nullptr;
    if(strstr(filename, ".html")){
        mime = "text/html";
    }else if(strstr(filename, ".png")){
        mime = "image/png";
    }

    char response[1024*50] = {0};
    sprintf(response, "HTTP/1.1 200 ok\r\nContent-Type: %s\r\n\r\n", mime);

    int responseLen = strlen(response);
    int filefd = open(filename, O_RDONLY);
    int fileLen = read(filefd, responseLen+response, sizeof(response)-responseLen);
    write(r_acc, response, responseLen+fileLen);
    close(filefd);
    sleep(1);
}

int main(int argc, char const *argv[])
{
    // 1.创建socket描述符
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    // 2.创建server协议地址簇
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);
    addr.sin_addr.s_addr = INADDR_ANY;
    // 3.绑定socket描述符和server协议地址簇
    int r = bind(socketfd, (sockaddr*)&addr, sizeof(addr));
    if(r==-1) cout << "绑定失败" << endl;
    else cout << "绑定成功" << endl; 
    // 4.监听
    int r_lis = listen(socketfd,10);
    if(r_lis==-1) cout << "监听失败" << endl;
    else cout << "监听成功" << endl; 
    // 5.等待连接
    while(1)
    {
        int r_acc = accept(socketfd,nullptr,nullptr);
        if(r_acc==-1) cout << "连接失败" << endl;
        else cout << "连接成功" << endl; 
        // 6.处理请求
        handle_connect(r_acc);
        close(r_acc);
    }
    return 0;
}
