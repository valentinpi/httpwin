# Make reference: https://www.gnu.org/software/make/manual/html_node/

#CC_FLAGS = -g -O0 -std=c11 -Wall -Wextra
CC_FLAGS = -Os -std=c11 -Wall -Wextra
LINK = -lws2_32

httpwin_obj := $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))

all: bin/url-fetcher bin/local-server

bin/url-fetcher: $(httpwin_obj)
	gcc $(CC_FLAGS) -o bin/url-fetcher examples/url-fetcher.c $(httpwin_obj) $(LINK)

bin/local-server: $(httpwin_obj)
	gcc $(CC_FLAGS) -o bin/local-server $(httpwin_obj) examples/local-server.c $(LINK)

obj/%.o: src/%.c
	gcc $(CC_FLAGS) -c -o $@ $<

obj/%.o: examples/%.c
	gcc $(CC_FLAGS) -c -o $@ $<

clean:
	if exist bin\url-fetcher.exe del bin\url-fetcher.exe
	if exist bin\local-server.exe del bin\local-server.exe
	if exist obj\*.o del obj\*.o

.PHONY: clean
