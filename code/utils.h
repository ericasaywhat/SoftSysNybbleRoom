#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>    //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <assert.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "wumpus.h"

#define MAX_SERVER_MSG_LENGTH 512
#define KEY_PREFIX ("nybbles_")
#define KEY_MAX_LENGTH (256)
#define MAX_CLIENTS 30
#define MAX_SERVER_MSG_LENGTH 512
#define MAX_USERNAME_SIZE 10
#define MAX_BUFFER_SIZE 1024
#define PORT   3000
#define KEY_COUNT (1024*1024)

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

typedef struct data_struct_s {
    char *name;
    char ip[KEY_MAX_LENGTH];
    char key_string[KEY_MAX_LENGTH];
    int socket_file_descriptor;
} Value;

void copy_over_ip(Value *value, char* ip);