#include <stdio.h>
#include <string.h>   //strlen
#include <sys/socket.h> //send

#include <glib.h>
#include <glib/gstdio.h>

// #define MAX_CLIENTS 30 //currently defined in main.h - we should pull this stuff out into another .h file.
#include "main.h"
#define MAX_SERVER_MSG_LENGTH 512

#define KEY_PREFIX ("nybbles_")
#define KEY_MAX_LENGTH (256)

typedef struct data_struct_s {
    char ip[KEY_MAX_LENGTH];
    char key_string[KEY_MAX_LENGTH];
    char *name;
    int socket_file_descriptor;
} Value;

int valueInArray(int* array, int array_size, int value) {
    int i;
    for (i = 0; i < array_size; ++i) {
        if (array[i] == value) {
            return 1;
        }
    }
    return 0;
}

//messageToServer, messageToCaller and messageToOther need to be malloc-ed with a maximum size. They'll be freed when the server shuts down?

//Sends one message to caller and one message to others.
int respond(int* client_socket, int caller, char* messageToServer, char * messageToCaller, char * messageToOthers) {
    
    printf("%s", messageToServer);

    int ret = 0;

    if(caller>-1){
        ret = send(client_socket[caller], messageToCaller, strlen(messageToCaller), 0);
        if (ret == -1) {
            return -1;
        }
    }

    int j;
    for (j = 0; j < MAX_CLIENTS; j++) {
        if (client_socket[j] != 0 && j != caller) {
            ret = send(client_socket[j] , messageToOthers , strlen(messageToOthers) , 0 );
            if (ret == -1) {
                return -1;
            }
        }
    }
    return ret;
}

//Sends one message to each socket in callerGroup and another socket to everyone else. Bad time complexity.
int respondToGroup (int* client_socket, int* callerGroup, int groupSize, char* messageToServer, char * messageToCaller, char * messageToOthers) {
    printf("%s", messageToServer);

    int i;
    for (i = 0; i<groupSize; i++){
        send(client_socket[callerGroup[i]], messageToCaller, strlen(messageToCaller), 0);
    }

    int j;
    for (j = 0; j < MAX_CLIENTS; j++) {
        int isCaller =  valueInArray(callerGroup, groupSize, j);
        if (client_socket[j] != 0 && isCaller == 0) {
            send(client_socket[j] , messageToOthers , strlen(messageToOthers) , 0 );
        }

    }

}

int change_name(GHashTable* hash, char* newName, char* ip, char* messageToServer, char* messageToCaller, char *messageToOthers) {
    char key_string[KEY_MAX_LENGTH]; // TODO move KEY_MAX_LENGTH to this file
    strcat(strcpy(key_string, KEY_PREFIX), ip);
    printf("#### REPLACING KEY_STRING VALUE AT : %s\n", key_string);
    char* copy = g_strdup(key_string);
    gpointer ret = g_hash_table_lookup(hash, copy);
    
    if (ret != NULL) {
        Value *value = (Value *)ret;
        value->name = newName;
        g_hash_table_replace(hash, copy, value);

        strcpy(messageToServer, "name change detected.\n");
        strcpy(messageToCaller, "name changed successfully\n\0");
        strcpy(messageToOthers, "someone else changed their name\n\0");

        printf("#### LOOKUP %s : %s\n", newName, ((Value *)g_hash_table_lookup(hash, copy))->name);
    } else {
        puts("User was not in the map yet");
    }

    
}

int run_test(char* messageToServer, char* messageToCaller, char* messageToOthers) {
    strcpy(messageToServer, ""); //empty message means nothing comes out.
    // strcpy(messageToServer, "someone typed a test function!\n");
    strcpy(messageToCaller, "you typed a test function!\n\0");
    strcpy(messageToOthers, "someone else typed a test function!\n\0");
}
