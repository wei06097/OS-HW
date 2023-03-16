shell: shell.c
	gcc shell.c -o shell

clean : shell
	rm shell

all: clean shell
