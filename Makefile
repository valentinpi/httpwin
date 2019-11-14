CC_FLAGS = -g -O0 -std=c11 -Wall -Wextra
LINK = -lws2_32

web-server: $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))
	gcc $(CC_FLAGS) -o web-server $< $(LINK)

obj/%.o: src/%.c
	gcc $(CC_FLAGS) -c -o $@ $<

clean:
	rm web-server obj/*.o

.PHONY: clean