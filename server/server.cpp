#include <stdio.h>
#include <tchar.h>
#include <sdkddkver.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

SOCKET clientSocket[1024];
int count = 0;
DWORD communication(LPVOID n)
{
    //通信
    char buff[256];
    int r;
    int i = (int)n;
    while (1)
    {
        memset(buff, 0, 256);
        r = recv(clientSocket[i-1], buff, 255, NULL);
        if (r > 0)
        {
            printf("%d >> %s\n", i, buff);
            for (int j = 0; j < count; j++)
            {
                send(clientSocket[j], buff, strlen(buff), NULL);
            }
        }

    }

}

int main()
{
    //1 请求协议版本
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2),&wsaData);
    //判断请求版本是否成功
    if (LOBYTE(wsaData.wVersion) != 2 ||
        HIBYTE(wsaData.wVersion) != 2)
    {
        printf("请求版本失败!\n");
        return -1;
    }
    printf("请求版本成功!\n");

    //2 创建socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    //4 绑定
    int r = bind(serverSocket, (sockaddr*)&addr, sizeof(addr));
    if (r == -1)
    {
        printf("bind失败!\n");
        return -1;
    }
    printf("bind成功\n");

    //5 监听
    r = listen(serverSocket, 10);
    if (r == -1)
    {
        printf("listen失败!\n");
        return -1;
    }
    printf("listen成功!\n");

    //6 等待客户端连接
    SOCKADDR_IN cAddr = { 0 };
    int len = sizeof cAddr;
    int i = 0;
    while (i < 1024)
    {
        clientSocket[i++] = accept(serverSocket, (sockaddr*)&cAddr, &len);
        count++;
        if (clientSocket[i-1] == SOCKET_ERROR)
        {
            printf("错误的客户端!\n");
            closesocket(serverSocket);
            WSACleanup();
            return -1;
        }
        printf("%s 连接进来了\n", inet_ntoa(cAddr.sin_addr));

        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)communication, (LPVOID)i, NULL,NULL);
    }
    return 0;
}

