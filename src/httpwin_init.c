#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

WSADATA wsaData;

int httpwin_init()
{
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    printf("WSAStartup succeeded\n");

    return 0;
}

int httpwin_terminate()
{
    return WSACleanup();
}
