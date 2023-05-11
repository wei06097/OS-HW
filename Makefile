a: replacement.c
	gcc replacement.c -o a

clean: a
	rm a

all: clean a
