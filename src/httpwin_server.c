#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int httpwin_server(const char *port)
{
    // Resolve local address
    struct addrinfo hints;
    struct addrinfo *addressinfo = NULL;
    SOCKET server = INVALID_SOCKET, client = INVALID_SOCKET;
    int iResult = 0;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, port, &hints, &addressinfo);
    if (iResult != 0) {
        printf("getaddrinfo on port %s failed: %d\n", port, iResult);
        goto cleanup;
    }
    printf("getaddrinfo succeeded\n");

    server = socket(addressinfo->ai_family, addressinfo->ai_socktype, addressinfo->ai_protocol);
    if (server == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        goto cleanup;
    }
    printf("socket succeeded\n");

    iResult = bind(server, addressinfo->ai_addr, (int) addressinfo->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        goto cleanup;
    }
    printf("bind succeeded\n");
    freeaddrinfo(addressinfo);

    if (listen(server, SOMAXCONN) == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        goto cleanup;
    }

    client = accept(server, NULL, NULL);
    if (client == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        goto cleanup;
    }

    const int recvbuflen = 1024;
    char recvbuf[1024];

    iResult = recv(client, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        printf("Bytes received: %d\n", iResult);

        iResult = send(client, recvbuf, iResult, 0);
        if (iResult == SOCKET_ERROR) {
            printf("send failed: %d\n", WSAGetLastError());
            goto cleanup;
        }

        printf("Bytes sent: %d\n", iResult);
    }
    else if (iResult == 0) {
        printf("Connection closed\n");
    }
    else {
        printf("recv failed: %d\n", WSAGetLastError());
        goto cleanup;
    }

    closesocket(client);
    closesocket(server);
    return 0;

cleanup:
    freeaddrinfo(addressinfo);
    closesocket(client);
    closesocket(server);
    return 1;
}
