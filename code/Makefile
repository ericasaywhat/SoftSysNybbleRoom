FLAGS = -g -std=c99
GLIB_FLAGS = -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
LIBS = -lglib-2.0

main: multiclient_test.c
	gcc $(FLAGS) -o main multiclient_test.c main.c main.h $(GLIB_FLAGS) $(LIBS)
