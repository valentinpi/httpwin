#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>

char* httpwin_request(const char *url, const char *port, int *size)
{
    // Resolve external address
    struct addrinfo hints = {};
    struct addrinfo *addressinfo = NULL;
    SOCKET connectSocket = INVALID_SOCKET;
    // Receive and send
    const int recvbuflen = 1024 * 64;
    char recvbuf[recvbuflen];
    memset(recvbuf, 0, recvbuflen);
    int recvheaplen = 0;
    char *recvheap = NULL;
    // Tracking
    int iResult = 0;

    // ai: address info, AF: Address Family
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    iResult = getaddrinfo(url, port, &hints, &addressinfo);
    if (iResult != 0) {
        printf("getaddrinfo on %s failed: %d\n", url, iResult);
        goto cleanup;
    }
    printf("getaddrinfo succeeded\n");

    connectSocket = socket(addressinfo->ai_family, addressinfo->ai_socktype, addressinfo->ai_protocol);
    if (connectSocket == INVALID_SOCKET) {
        printf("socket failed: %d\n", WSAGetLastError());
        goto cleanup;
    }
    printf("socket succeeded\n");

    iResult = connect(connectSocket, addressinfo->ai_addr, (int) addressinfo->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("connect failed\n");
        goto cleanup;
    }
    printf("connect succeeded\n");
    freeaddrinfo(addressinfo);

    /* BEGIN WRITE REQUEST HEADER */

    char sendbuf[1024];
    // 22 bytes, excluding '\0'
    char *header = "GET / HTTP/1.1\r\nHost: ";
    char *footer = "\r\nAccept: */*\r\n\r\n";
    strncpy(sendbuf, header, strlen(header) + 1);
    strncat(sendbuf, url, 1024 - strlen(header) - strlen(footer) + 1);
    // 17 byte, excluding '\0'
    strncat(sendbuf, footer, strlen(footer) + 1);
    iResult = send(connectSocket, sendbuf, (int) strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        goto cleanup;
    }
    else {
        printf("Bytes sent: %d\n", iResult);
    }
    
    /* END WRITE REQUEST HEADER */

    iResult = shutdown(connectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        goto cleanup;
    }

    iResult = 0;
    while (iResult >= 0) {
        int previResult = iResult;
        iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
        printf("iResult: %d -> %d\n", previResult, iResult);

        if (iResult > 0) {
            if (recvheaplen == 0) {
                recvheaplen = iResult;
                recvheap = (char*) malloc(recvheaplen);
                memcpy(recvheap, recvbuf, iResult);
            }
            else {
                recvheaplen += iResult;
                char *recvheapnew = (char*) realloc(recvheap, recvheaplen);
                if (recvheapnew == NULL)
                {
                    printf("realloc failed\n");
                    goto cleanup;
                }
                else {
                    recvheap = recvheapnew;
                    memcpy(recvheap + recvbuflen - iResult, recvbuf, iResult);
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
            goto cleanup;
        }
    }

    if (recvheaplen == 0) {
        printf("No data sent");
        goto cleanup;
    }

    *size = recvheaplen;
    closesocket(connectSocket);
    return recvheap;

cleanup:
    closesocket(connectSocket);
    freeaddrinfo(addressinfo);
    free(recvheap);
    return NULL;
}
