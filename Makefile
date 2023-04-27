banker: banker.c
	gcc banker.c -pthread -o a

clean: a
	rm a

all: clean banker
