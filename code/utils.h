#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
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
#include "main.h"

#define MAX_SERVER_MSG_LENGTH 512
#define KEY_PREFIX ("nybbles_")
#define KEY_MAX_LENGTH (256)
#define MAX_CLIENTS 30
#define MAX_SERVER_MSG_LENGTH 512
#define MAX_USERNAME_SIZE 10
#define MAX_BUFFER_SIZE 1024
#define PORT   3000
#define KEY_COUNT (1024*1024)

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"