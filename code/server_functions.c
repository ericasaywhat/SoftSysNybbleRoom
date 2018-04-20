#include <stdio.h>
#include <string.h>   //strlen
#include <sys/socket.h> //send

// #define MAX_CLIENTS 30 //currently defined in main.h - we should pull this stuff out into another .h file.
#include "main.h"





//messageToServer, messageToCaller and messageToOther need to be malloc-ed with a maximum size. They'll be freed when the server shuts down?

//Sends one message to caller and one message to others.
int respond(int* client_socket, int caller, char* messageToServer, char * messageToCaller, char * messageToOthers) {
    
    printf("%s", messageToServer);

    if(caller>-1){
        send(client_socket[caller], messageToCaller, strlen(messageToCaller), 0);
    }
    int j;
    for (j = 0; j < MAX_CLIENTS; j++) {
        if (client_socket[j] != 0 && j != caller) {
            send(client_socket[j] , messageToOthers , strlen(messageToOthers) , 0 );
        }

    }
}

int change_name(char* newName, char* ipAddress, void* pointerToHashmap, char* messageToServer, char* messageToCaller, char *messageToOthers) {
    // if(isValidIP(ipAddress) && isValidName(newName)){
    //     pointerToHashmap.insert(ipAddress,newName); //pseudocode
        strcpy(messageToServer, "name change detected.\n");
        strcpy(messageToCaller, "name changed successfully\n\0");
        strcpy(messageToOthers, "someone else changed their name\n\0");

        // return 0;
    // } else {
    //     return 1;
    // }
}

int run_test(char* messageToServer, char* messageToCaller, char* messageToOthers) {
    strcpy(messageToServer, ""); //empty message means nothing comes out.
    // strcpy(messageToServer, "someone typed a test function!\n");
    strcpy(messageToCaller, "you typed a test function!\n\0");
    strcpy(messageToOthers, "someone else typed a test function!\n\0");
}
