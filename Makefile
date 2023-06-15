sort: sort.c
	gcc sort.c -pthread -o sort

clean: sort
	rm sort

all: clean sort
