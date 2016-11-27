a.out	: main.c
	g++ -g main.c

clean	:
	rm -f a.out
