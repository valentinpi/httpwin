#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "../src/httpwin.h"

void sigint_handler(int sig)
{
    printf("Terminating program. Signal: %d", sig);
    httpwin_terminate();
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: local-server <port>\n");
        return EXIT_FAILURE;
    }

    if (httpwin_init() != 0)
        return EXIT_FAILURE;

    signal(SIGINT, sigint_handler);

    while(1) {
        if (httpwin_serve(argv[1]) != 0) {
            httpwin_terminate();
            return EXIT_FAILURE;
        }
    }

    httpwin_terminate();

    return EXIT_SUCCESS;
}
