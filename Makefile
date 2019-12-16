#CC_FLAGS = -g -O0 -std=c11 -Wall -Wextra
CC_FLAGS = -Os -std=c11 -Wall -Wextra
LINK = -lws2_32

web-server: $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))
	gcc $(CC_FLAGS) -o httpwin $< $(LINK)

obj/%.o: src/%.c
	gcc $(CC_FLAGS) -c -o $@ $<

clean:
	if exist httpwin.exe del httpwin.exe
	if exist obj\*.o del obj\*.o

.PHONY: clean