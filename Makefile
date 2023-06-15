a: pipe.c
	gcc pipe.c -o a

clean: a
	rm a

all: clean a
