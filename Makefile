a: vaddress.c
	gcc vaddress.c -o a

clean: a
	rm a

all: clean a
