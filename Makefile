a.out	: main.c
	gcc -std=c99 -g main.c

clean	:
	rm -f a.out
