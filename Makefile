CC_FLAGS = -g -O0 -std=c11 -Wall -Wextra
LINK = -lws2_32

web-server: obj/main.obj
	gcc $(CC_FLAGS) -o web-server obj/main.obj $(LINK)

obj/main.obj: src/main.c
	gcc $(CC_FLAGS) -c -o obj/main.obj src/main.c 
