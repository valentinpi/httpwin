#include <stdio.h>
#include <stdlib.h>

#include "../src/httpwin.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: local-server <port>\n");
        return EXIT_FAILURE;
    }

    httpwin_init();

    if (httpwin_server(argv[1]) != 0) {
        httpwin_terminate();
        return EXIT_FAILURE;
    }

    httpwin_terminate();

    return EXIT_SUCCESS;
}
