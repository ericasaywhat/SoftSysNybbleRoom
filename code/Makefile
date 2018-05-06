FLAGS = -g -w -std=c99
GLIB_FLAGS = -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
LIBS = -lglib-2.0

main: main.c
	gcc $(FLAGS) -o main main.c wumpus.c wumpus.h utils.h $(GLIB_FLAGS) $(LIBS)
