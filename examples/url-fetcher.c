// Documentation: https://docs.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock

#include <stdio.h>
#include <stdlib.h>

#include "../src/httpwin.h"

int main(int argc, char *argv[])
{
    if (argc < 3) {
        printf("Usage: url-fetch <url> <port>\n");
        return EXIT_FAILURE;
    }

    if (httpwin_init() != 0)
        return EXIT_FAILURE;
    
    int reqsize = 0;
    char *req = httpwin_request(argv[1], argv[2], &reqsize);

    if (req == NULL) {
        httpwin_terminate();
        return EXIT_FAILURE;
    }
    
    printf("reqsize: %d\n", reqsize);

    // TODO: Improve this
    int htmloffset = 0;
    for (int i = 0; i < reqsize; i++) {
        if (req[i] == '<') {
            htmloffset = i;
            i = reqsize;
        }
    }

    FILE *file = fopen64("index.html", "wtS");
    // Write the content of recvheap without the nullterminator to a file
    fwrite(req + htmloffset, sizeof(char), (size_t) reqsize - htmloffset, file);
    fclose(file);
    printf("Wrote index.html in directory of execution");

    free(req);
    httpwin_terminate();
    return EXIT_SUCCESS;
}
