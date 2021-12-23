.POSIX:
.SUFFIXES:

CFLAGS = -I/usr/local/include/luajit-2.0/
LDFLAGS = -pagezero_size 10000  -lluajit
OBJ = main.o linebuffer.o handler.o

.PHONY: run clean all
all: main
run: main
	./main 
clean:
	$(RM) *.o main

main: $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $(OBJ)


.SUFFIXES: .c .o
.o: 
	$(CC) $(CFLAGS) -c $<
