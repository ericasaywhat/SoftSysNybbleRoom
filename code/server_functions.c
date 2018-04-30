#include "utils.h"

/**
 * Copies the user's IP address into a value.
 */
void copy_over_ip(Value *value, char* ip) {
    for (int i = 0; i < strlen(ip); i++) {
        value->ip[i] = ip[i];
    }
}

int val_in_array(int* array, int array_size, int value) {
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
int respond_to_group (int* client_socket, int* callerGroup, int groupSize, char* messageToServer, char * messageToCaller, char * messageToOthers) {
    printf("%s", messageToServer);

    int i;
    for (i = 0; i<groupSize; i++){
        send(client_socket[callerGroup[i]], messageToCaller, strlen(messageToCaller), 0);
    }

    int j;
    for (j = 0; j < MAX_CLIENTS; j++) {
        int isCaller =  val_in_array(callerGroup, groupSize, j);
        if (client_socket[j] != 0 && isCaller == 0) {
            send(client_socket[j] , messageToOthers , strlen(messageToOthers) , 0 );
        }

    }
}

int change_name(GHashTable* hash, char* tempName, char* ip, char* messageToServer, char* messageToCaller, char *messageToOthers) {
    char* newName = g_strdup(tempName);
    char* key_string = malloc(sizeof(char) * KEY_MAX_LENGTH);
    strcat(strcpy(key_string, KEY_PREFIX), ip);
    printf(GRN "REPLACING KEY_STRING VALUE AT : %s\n" RESET, key_string);
    // char* copy = g_strdup(key_string);
    gpointer ret = g_hash_table_lookup(hash, key_string);
    
    if (ret != NULL) {
        Value *value = (Value *)ret;
        newName[strcspn(newName, "\n")-1] = 0;
        value->name = newName;
        g_hash_table_replace(hash, key_string, value);

        strcpy(messageToServer, "Name change detected.\n\0");
        strcpy(messageToCaller, "Name changed successfully!\n\0");
        strcpy(messageToOthers, "Someone else changed their name!\n\0");

        printf(GRN "LOOKUP %s : %s\n" RESET, value->name, ((Value *)g_hash_table_lookup(hash, key_string))->name);
    } else {
        printf(RED "Oops! The user was not in the map yet.\n" RESET);
    }    
}

void handle_name_change(GHashTable* hash, char* buffer, struct sockaddr_in address, int* client_socket, int i) {
    char * messageToServer = malloc(sizeof(char)*MAX_SERVER_MSG_LENGTH);
    char * messageToCaller = malloc(sizeof(char)*MAX_SERVER_MSG_LENGTH);
    char * messageToOthers = malloc(sizeof(char)*MAX_SERVER_MSG_LENGTH);
    char newName[100];
    int callers[2];
    char separator = ' ';
    char * const tempName = strchr(buffer, separator);
    if(tempName != NULL) {
      *tempName = '\0';
    }
    strcpy(newName, tempName+1);
    change_name(hash, newName, inet_ntoa(address.sin_addr), messageToServer, messageToCaller, messageToOthers);

    callers[0] = i;
    callers[1] = i+1;
    respond_to_group(client_socket, callers, 2, messageToServer, messageToCaller, messageToOthers);
    free(messageToServer);
    free(messageToCaller);
    free(messageToOthers);
}

/**
 * Sets up a new connection and puts the resulting value into the hashtable.
 */
void setup_new_connection(GHashTable* hash, int new_socket, struct sockaddr_in address, Value *value) {
    char* query_name;
    char* new_name = malloc(sizeof(char) * MAX_USERNAME_SIZE); // TODO free this later
    char key_string[KEY_MAX_LENGTH];
    char* dump;

    printf(GRN "ADDING NEW CONNECTION\n" RESET);
    printf(GRN "SOCKET : %d, IP : %s, PORT : %d \n" RESET, new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    copy_over_ip(value, inet_ntoa(address.sin_addr)); // store IP address
    printf(GRN "IP : %s\n" RESET, value->ip);
    strcat(strcpy(key_string, KEY_PREFIX), value->ip);
    printf(GRN "KEY_STRING : %s\n" RESET, key_string);
    snprintf(value->key_string, KEY_MAX_LENGTH, "%s", key_string);
    printf(GRN "STORED KEY_STRING : %s\n" RESET, value->key_string);

    query_name = "Hi there! Please choose your username (up to 10 characters): ";
    send(new_socket, query_name , strlen(query_name) , 0 );
    read(new_socket, new_name, MAX_USERNAME_SIZE); // get name value

    new_name[strcspn(new_name, "\n")-1] = 0;

    printf(GRN "NEW SOCKET'S NAME : %s \n" RESET, new_name);

    value->name = new_name;
    value->socket_file_descriptor = new_socket;
    char* copy = g_strdup(key_string);
    g_hash_table_insert(hash, copy, value);

    printf(GRN "NEW CONNECTION ADDED SUCCESSFULLY!\n" RESET);
}

int run_test(char* messageToServer, char* messageToCaller, char* messageToOthers) {
    strcpy(messageToServer, ""); //empty message means nothing comes out.
    // strcpy(messageToServer, "someone typed a test function!\n");
    strcpy(messageToCaller, "you typed a test function!\n\0");
    strcpy(messageToOthers, "someone else typed a test function!\n\0");
}
