a: io.c
	gcc io.c -o a

clean: a
	rm a

all: clean a
