#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>


int httpwin_init()
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }
    printf("WSAStartup succeeded\n");

    printf("\nwsaData:\n");
    printf("  iMaxSockets:    %u\n", wsaData.iMaxSockets);
    printf("  iMaxUdpDg:      %u\n", wsaData.iMaxUdpDg);
    //printf("%s\n", wsaData.lpVendorInfo);
    printf("  szDescription:  %s\n", wsaData.szDescription);
    printf("  szSystemStatus: %s\n", wsaData.szSystemStatus);
    printf("  wHighVersion:   %d\n", wsaData.wHighVersion);
    printf("  wVersion:       %d\n", wsaData.wVersion);
    printf("\n");

    return 0;
}

int httpwin_terminate()
{
    return WSACleanup();
}
