// Documentation: https://docs.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: web-server <url>\n");
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
    // 22 byte
    strcpy(sendbuf, "GET / HTTP/1.1\r\nHost: ");
    strncat(sendbuf, argv[1], 1024 - 17);
    // 17 byte
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

    const int recvbuflen = 128;
    char recvbuf[recvbuflen];
    char *recvheap = NULL;
    int recvheaplen = 0;
    iResult = 1;
    while (iResult > 0) {
        printf("iResult: %d -> ", iResult);
        iResult = recv(connectSocket, recvbuf, recvbuflen, 0);
        printf("%d\n", iResult);
        if (iResult > 0) {
            printf("Bytes received: %d\n", iResult);
            if (recvheaplen <= 0) {
                recvheaplen += iResult;
                recvheap = (char*) malloc(recvheaplen);
                strcpy(recvheap, recvbuf);
            }
            else {
                recvheaplen += iResult;
                recvheap = (char*) realloc(recvheap, recvheaplen);
                memcpy(recvheap + (recvheaplen - iResult) * sizeof(char), recvbuf, iResult);
            }
        }
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    }

    FILE *file = fopen64("index.html", "wtS");
    fwrite(recvheap, sizeof(char), (size_t) recvheaplen, file);
    fclose(file);
    free(recvheap);

    closesocket(connectSocket);
    freeaddrinfo(result);
    WSACleanup();
    return EXIT_SUCCESS;
}

/* curl HTTP example */

// C:\Users\Valentin\Dev\web-server>curl -v example.com
// * Rebuilt URL to: example.com/
// *   Trying 2606:2800:220:1:248:1893:25c8:1946...
// * TCP_NODELAY set
// * Connected to example.com (2606:2800:220:1:248:1893:25c8:1946) port 80 (#0)
// > GET / HTTP/1.1
// > Host: example.com
// > User-Agent: curl/7.55.1
// > Accept: */*
// >
// < HTTP/1.1 200 OK
// < Accept-Ranges: bytes
// < Cache-Control: max-age=604800
// < Content-Type: text/html; charset=UTF-8
// < Date: Thu, 31 Oct 2019 16:45:35 GMT
// < Etag: "3147526947"
// < Expires: Thu, 07 Nov 2019 16:45:35 GMT
// < Last-Modified: Thu, 17 Oct 2019 07:18:26 GMT
// < Server: ECS (dcb/7EA6)
// < Vary: Accept-Encoding
// < X-Cache: HIT
// < Content-Length: 1256
// <
// <!doctype html>
// <html>
// <head>
//     <title>Example Domain</title>
// 
//     <meta charset="utf-8" />
//     <meta http-equiv="Content-type" content="text/html; charset=utf-8" />
//     <meta name="viewport" content="width=device-width, initial-scale=1" />
//     <style type="text/css">
//     body {
//         background-color: #f0f0f2;
//         margin: 0;
//         padding: 0;
//         font-family: -apple-system, system-ui, BlinkMacSystemFont, "Segoe UI", "Open Sans", "Helvetica Neue", Helvetica, Arial, sans-serif;
// 
//     }
//     div {
//         width: 600px;
//         margin: 5em auto;
//         padding: 2em;
//         background-color: #fdfdff;
//         border-radius: 0.5em;
//         box-shadow: 2px 3px 7px 2px rgba(0,0,0,0.02);
//     }
//     a:link, a:visited {
//         color: #38488f;
//         text-decoration: none;
//     }
//     @media (max-width: 700px) {
//         div {
//             margin: 0 auto;
//             width: auto;
//         }
//     }
//     </style>
// </head>
// 
// <body>
// <div>
//     <h1>Example Domain</h1>
//     <p>This domain is for use in illustrative examples in documents. You may use this
//     domain in literature without prior coordination or asking for permission.</p>
//     <p><a href="https://www.iana.org/domains/example">More information...</a></p>
// </div>
// </body>
// </html>
// * Connection #0 to host example.com left intact
//
