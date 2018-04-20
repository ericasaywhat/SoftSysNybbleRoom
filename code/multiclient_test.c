//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
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


// #include "main.h" //currently included in server_functions.c
#include "server_functions.c"


#define MAX_CLIENTS 30
#define MAX_SERVER_MSG_LENGTH 512

#include <assert.h>

#include "hashmap.h"

#define TRUE   1
#define FALSE  0
#define PORT   3000

#define KEY_MAX_LENGTH (256)
#define KEY_PREFIX ("nybbles_")
#define KEY_COUNT (1024*1024)

typedef struct data_struct_s
{
    char ip[KEY_MAX_LENGTH];
    char key_string[KEY_MAX_LENGTH];
    char *name;
    int socket_file_descriptor;

} data_struct_t;

void copy_over_ip(data_struct_t *value, char* ip) {
    for (int i = 0; i < strlen(ip); i++) {
        value->ip[i] = ip[i];
    }
}

void free_everything(map_t *map, char *ip, data_struct_t *value) {
    int error;

    /* Free all of the values we allocated and remove them from the map */
    for (int index=0; index<KEY_COUNT; index+=1)
    {
        snprintf(ip, KEY_MAX_LENGTH, "%s%d", KEY_PREFIX, index);

        error = hashmap_get(map, ip, (void**)(&value));
        assert(error==MAP_OK);

        error = hashmap_remove(map, ip);
        assert(error==MAP_OK);

        free(value);
    }

    hashmap_free(map);
}

void setup_new_connection(map_t *map, int new_socket, struct sockaddr_in address, data_struct_t *value) {
    char* query_name;
    char new_name[1024];

    puts("#### NEW CONNECTION ####");
    printf("Socket fd is %d    IP is : %s    Port : %d \n" , new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

    copy_over_ip(value, inet_ntoa(address.sin_addr)); // store IP address
    snprintf(value->key_string, KEY_MAX_LENGTH, "%s%d", KEY_PREFIX, value->name); // store key

    query_name = "What do you want to be called? : ";
    send(new_socket, query_name , strlen(query_name) , 0 );
    read(new_socket, new_name, 1024); // get name value

    new_name[strcspn(new_name, "\n")-1] = 0;

    printf("New socket's name : %s \n", new_name);

    value->name = new_name;
    value->socket_file_descriptor = new_socket;
    hashmap_put(map, value->key_string, value);

    puts("#### NEW CONNECTION ADDED SUCCESSFULLY ####");
}

int main(int argc , char *argv[])
{
    // game_start();
    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[MAX_CLIENTS] ,
          max_clients = MAX_CLIENTS , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;
    map_t *map;
    data_struct_t *value;

    char buffer[1025];  //data buffer of 1K

    //set of socket descriptors
    fd_set readfds;

    //make the map
    map = hashmap_new();

    //a message
    char *message = "ECHO Daemon v1.0 \r\n";

    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
          sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while(TRUE)
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR)) {
            printf("select error");
        }

        //If something happened on the master socket then it's an incoming connection
        if (FD_ISSET(master_socket, &readfds)) {
            if ((new_socket = accept(master_socket,
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            value = malloc(sizeof(data_struct_t)); // allocate a new value
            setup_new_connection(map, new_socket, address, value);
            puts("hello");
            //TODO test value here

            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )
            {
                perror("send");
            }

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);

                    break;
                }
            }
        }
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];
            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);
                    printf("Host disconnected on socket %i, ip %s , port %d \n" , sd, inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                }

                //Handle the message that came in
                else
                {
                    char * messageToServer = malloc(sizeof(char)*MAX_SERVER_MSG_LENGTH);
                    char * messageToCaller = malloc(sizeof(char)*MAX_SERVER_MSG_LENGTH);
                    char * messageToOthers = malloc(sizeof(char)*MAX_SERVER_MSG_LENGTH);
                    //set the string terminating NULL byte on the end
                    //of the data read
                    if (strncmp(buffer, "!wumpus", 7) == 0){
                        printf("wumpus command recognized\n");
                        game_start();
                        continue;
                    }

                    if (strncmp(buffer, "!test", 5) == 0){
                        run_test(messageToServer, messageToCaller, messageToOthers);
                        respond(client_socket, i, messageToServer, messageToCaller, messageToOthers);
                        continue;
                    }

                    if (strncmp(buffer, "!name", 5) == 0){
                        // printf("wumpus command recognized\n");
                        void* p;
                        change_name("_", "_", p, messageToServer, messageToCaller, messageToOthers);
                        respond(client_socket, i, messageToServer, messageToCaller, messageToOthers);
                        continue;
                    }
                    
                    else {
                        buffer[valread] = '\0';
                        // printf("%s", buffer);
                        // int k;
                        // for (k = 0; k < max_clients; k++) {
                        //     if (client_socket[k] != 0 && k != i) {
                        //         send(client_socket[k] , buffer , strlen(buffer) , 0 );
                        //     }

                        // }

                        char* new_message = strcat(strcat(value->name, ": "), buffer);
                        printf("new_message is: %s\n", new_message);

                        respond(client_socket, i, new_message, "", new_message);
                    }




                }
            }
        }
    }
    return 0;
}
