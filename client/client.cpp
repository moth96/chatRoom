#include <stdio.h>
#include <tchar.h>
#include <sdkddkver.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <graphics.h>

SOCKET serverSocket;

void recvAndShow()
{
    int i = 0;
    char buff[256];
    int r;
    while (1)
    {
        memset(buff, 0, 256);
        r = recv(serverSocket, buff, 255, NULL);
        if (r > 0)
        {
            outtextxy(0, i * 20, buff);
            i++;
        }
    }
}

int main()
{
    initgraph(300, 300, SHOWCONSOLE);
    //1 请求协议版本
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    //判断请求版本是否成功
    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2)
    {
        printf("请求版本失败!\n");
        return -1;
    }
    printf("请求版本成功!\n");

    //2 创建socket
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        printf("创建socket失败!\n");
        return -1;
    }
    printf("创建socket成功!\n");

    //3 创建协议地址族
    SOCKADDR_IN addr = { 0 };
    //1 协议
    addr.sin_family = AF_INET;
    //2 主机 电脑
    addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    //3 网络端口 应用
    addr.sin_port = htons(10086);

    //6 连接服务器
    int r = connect(serverSocket, (sockaddr*)&addr, sizeof addr);
    if (r == -1)
    {
        printf("无法连接服务器!\n");
        return -1;
    }
    printf("连接服务器成功!\n");

    //通信
    CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)recvAndShow, NULL, NULL, NULL);

    char buff[256];
    while (1)
    {
        memset(buff, 0, 256);
        printf(">>:");
        scanf("%s", buff);
        r = send(serverSocket, buff, strlen(buff), NULL);
        if (r > 0)
        {
            printf("发送成功...\n");
        }
    }
    return 0;
}

