buffer: buffer.c buffer.h
	gcc buffer.c buffer.h -pthread -o buffer

clean: buffer
	rm buffer

all: clean buffer