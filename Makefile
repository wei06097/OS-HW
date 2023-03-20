shell: shell.c
	gcc shell.c -o shell

wait: wait.c
	gcc wait.c -o wait
	
clean: shell
	rm shell

all: clean shell
