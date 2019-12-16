// Documentation: https://docs.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: httpwin <url>\n");
        return EXIT_FAILURE;
    }

    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return EXIT_FAILURE;
    }
    printf("WSAStartup succeeded\n");

    struct addrinfo hints;
    ZeroMemory(&hints, sizeof(hints));
    // Address family will be left unspecified, IP address will be either IPv4 or IPv6
    // ai: address info, AF: Address Family
    hints.ai_family = AF_UNSPEC;
    // TCP stream socket
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    struct addrinfo *result = NULL;
    iResult = getaddrinfo(argv[1], "80", &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo on %s failed: %d\n", argv[1], iResult);
        WSACleanup();
        return EXIT_FAILURE;
    }
    printf("getaddrinfo succeded\n");

    SOCKET connectSocket = INVALID_SOCKET;
    connectSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (connectSocket == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return EXIT_FAILURE;
    }
    printf("socket succeeded\n");

    iResult = connect(connectSocket, result->ai_addr, (int) result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("connect failed\n");
        closesocket(connectSocket);
        connectSocket = INVALID_SOCKET;
        freeaddrinfo(result);
        WSACleanup();
        return EXIT_FAILURE;
    }
    printf("connect succeeded\n");

    char sendbuf[1024];
    // 22 byte, excluding '\0'
    strcpy(sendbuf, "GET / HTTP/1.1\r\nHost: ");
    strncat(sendbuf, argv[1], 1024 - 17);
    // 17 byte, excluding '\0'
    strcat(sendbuf, "\r\nAccept: */*\r\n\r\n");
    iResult = send(connectSocket, sendbuf, (int) strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }
    else {
        printf("Bytes sent: %d\n", iResult);
    }

    iResult = shutdown(connectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(connectSocket);
        freeaddrinfo(result);
        return EXIT_FAILURE;
    }

    const int recvbuflen = 1024;
    char recvbuf[recvbuflen + 1];
    int recvheaplen = 0;
    char *recvheap = NULL;
    iResult = 0;
    while (iResult >= 0) {
        int previResult = iResult;
        iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
        printf("iResult: %d -> %d\n", previResult, iResult);

        if (iResult > 0) {
            recvbuf[iResult] = '\0';
            if (recvheaplen == 0) {
                recvheaplen = iResult;
                recvheap = (char*) malloc(recvheaplen);
                recvheap[0] = '\0';
                strncpy(recvheap, recvbuf, iResult + 1);
            }
            else {
                recvheaplen += iResult;
                char *recvheapnew = (char*) realloc(recvheap, recvheaplen);
                if (recvheapnew == NULL)
                    printf("realloc failed\n");
                else {
                    recvheap = recvheapnew;
                    strncat(recvheap, recvbuf, recvbuflen + 1);
                }
            }
        }
        else if (iResult == 0) {
            printf("Connection closed\n");
            iResult--;
            break;
        }
        else {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(connectSocket);
            freeaddrinfo(result);
            WSACleanup();
            return 1;
        }
    }

    if (recvheaplen == 0) {
        printf("No data sent");
        closesocket(connectSocket);
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // TODO: Improve this
    int htmloffset = 0;
    for (int i = 0; i < recvheaplen; i++) {
        if (recvheap[i] == '<') {
            htmloffset = i;
            i = recvheaplen;
        }
    }

    printf("recvheaplen: %d, strlen(recvheap): %I64d\n", recvheaplen, strlen(recvheap));

    FILE *file = fopen64("index.html", "wtS");
    // Write the content of recvheap without the nullterminator to a file
    fwrite(recvheap + htmloffset, sizeof(char), (size_t) recvheaplen - htmloffset, file);
    fclose(file);
    free(recvheap);
    printf("Wrote index.html in directory of execution");

    closesocket(connectSocket);
    freeaddrinfo(result);
    WSACleanup();
    return EXIT_SUCCESS;
}
