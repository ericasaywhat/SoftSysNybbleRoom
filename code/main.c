#include "server_functions.c"

/**
 * Copies the user's IP address into a value.
 */
void copy_over_ip(Value *value, char* ip) {
    for (int i = 0; i < strlen(ip); i++) {
        value->ip[i] = ip[i];
    }
}

/**
 * Frees the hashtable and all values inside of it.
 */
void free_everything(GHashTable* hash) {
    g_hash_table_destroy(hash);
    // TODO: Free any dynamically allocated values?
    // TODO: Free everything else
}

void print_entry(gpointer key, gpointer value, gpointer userdata) {
    Value* v = (Value *)value;
    char* c = (char *)key;
    printf("%s : %s\n", c, v->name);
}

char* retrieve_username(GHashTable* hash, char* ip) {
    char key_string[KEY_MAX_LENGTH];
    strcat(strcpy(key_string, KEY_PREFIX), ip);
    char* copy = g_strdup(key_string);
    gpointer ret = g_hash_table_lookup(hash, copy);

    if (ret != NULL) {
        Value *value = (Value *)ret;
        return value->name;
    } else {
        puts("name not found");
        return NULL;
    }
}

int retrieve_socket_fd(GHashTable* hash, char* ip) {
    char key_string[KEY_MAX_LENGTH];
    strcat(strcpy(key_string, KEY_PREFIX), ip);
    char* copy = g_strdup(key_string);
    gpointer ret = g_hash_table_lookup(hash, copy);

    if (ret != NULL) {
        Value *value = (Value *)ret;
        return value->socket_file_descriptor;
    } else {
        puts("socket_fd not found");
        return NULL;
    }
}

void signal_handler(int sig) {
    char  c;
    signal(sig, SIG_IGN);
    printf(RED "\nAre you sure you want to quit? [y/n] " RESET);
    c = getchar();
    if (c == 'y' || c == 'Y'){
        printf(RED "Okay! Closing server...\n" RESET);
        printf(WHT " \n" RESET); // Resets color in terminal
        // TODO free everything
        exit(0);
    }
    else{
        signal(SIGINT, signal_handler);
    }
    getchar(); // Get new line character
}

int main(int argc , char *argv[]) {
    // game_start();
    int opt = TRUE;
    int chatting = 1;
    int master_socket , addrlen , new_socket , client_socket[MAX_CLIENTS] ,
          max_clients = MAX_CLIENTS , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;
    Value *value;
    char buffer[1025];  //data buffer of 1K
    fd_set readfds; //set of socket descriptors
    int rps_players = 0;

    GHashTable* hash = g_hash_table_new(g_str_hash, g_str_equal);

    char *message = "Welcome to the SoftSys NybbleRoom v1.0! \r\n";

    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
          sizeof(opt)) < 0 ) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    printf(BLU "Listener on port %d\n" RESET, PORT);

    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    printf(BLU "Waiting for connections...\n" RESET);
    signal(SIGINT, signal_handler);
    while(TRUE) {
        FD_ZERO(&readfds); // clear socket set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
        for ( i = 0 ; i < max_clients ; i++) {
            sd = client_socket[i];
            if (sd > 0) {
                FD_SET( sd , &readfds);
            }
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

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

            value = malloc(sizeof(Value)); // allocate a new value
            setup_new_connection(hash, new_socket, address, value);

            if(send(new_socket, message, strlen(message), 0) != strlen(message)) {
                perror("send");
            } else {
                printf(BLU "Welcome message sent successfully!\n" RESET);
            }

            for (i = 0; i < max_clients; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf(BLU "Adding to list of sockets as %d...\n" RESET, i);
                    break;
                }
            }
        }

        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];
            if (FD_ISSET(sd, &readfds)) {
                // User disconnected
                if ((valread = read( sd, buffer, MAX_BUFFER_SIZE)) == 0) {
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                    printf(RED "Host disconnected on socket %i, ip %s, port %d...\n" RESET, sd, inet_ntoa(address.sin_addr), ntohs(address.sin_port));
                    remove_disconnected_user(hash, address);
                    close(sd);
                    client_socket[i] = 0;
                } else {

                    // if (strncmp(buffer, "!wumpus", 7) == 0){
                    //     printf("wumpus command recognized\n");
                    //     game_start();
                    //     continue;
                    // }
                    // if (strncmp(buffer, "!test", 5) == 0){
                    //     run_test(messageToServer, messageToCaller, messageToOthers);
                    //     respond(client_socket, i, messageToServer, messageToCaller, messageToOthers);
                    //     continue;
                    // }
                    if (strncmp(buffer, "!name", 5) == 0) {
                        getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                        handle_name_change(hash, buffer, address, client_socket, i);
                        continue;
                    } else if (strncmp(buffer, "!rps", 4) == 0){
                        getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                        int p1Socket = retrieve_socket_fd(hash, inet_ntoa(address.sin_addr));
                        play_rps_request(hash, buffer, value->name, p1Socket);
                        // continue;
                    } else {
                        getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
                        buffer[valread] = '\0';
                        char* username = retrieve_username(hash, inet_ntoa(address.sin_addr));


                        printf("retrieve_username took IP %s and returned %s\n", inet_ntoa(address.sin_addr), username);
                        int size=g_hash_table_size(hash);
                        printf("SIZE OF TABLE: %d\n", size);



                        char message_to_send[MAX_SERVER_MSG_LENGTH];
                        memset(message_to_send, '\0', sizeof(message_to_send));
                        strcpy(message_to_send, username); // copy username in
                        strcat(strcat(message_to_send, " says: "), buffer);
                        // printf("new_message is: %s\n", message_to_send);
                        respond(client_socket, i, message_to_send, "", message_to_send);
                        memset(message_to_send, 0 , MAX_SERVER_MSG_LENGTH);
                        memset(buffer, 0 , MAX_BUFFER_SIZE);
                    }
                }
            }
        }
    }
    return 0;
}

