#ifndef HTTPWIN_H
#define HTTPWIN_H

int httpwin_init();
char* httpwin_request(const char *url, const char *port, int *size);
int httpwin_serve(const char *port);
int httpwin_terminate();

#endif
