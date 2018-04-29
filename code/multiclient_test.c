//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux

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

void remove_disconnected_user(GHashTable* hash, struct sockaddr_in address) {
    char key_string[KEY_MAX_LENGTH];
    strcat(strcpy(key_string, KEY_PREFIX), inet_ntoa(address.sin_addr));
    char* copy = g_strdup(key_string);
    gboolean ret = g_hash_table_remove(hash, copy);
    if (ret) {
        printf(RED "User successfully removed from hashtable.\n" RESET);
    } else {
        printf(RED "Oops! User could not be found in hashtable.\n" RESET);
    }
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
    int master_socket , addrlen , new_socket , client_socket[MAX_CLIENTS] ,
          max_clients = MAX_CLIENTS , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;
    Value *value;
    char buffer[1025];  //data buffer of 1K
    fd_set readfds; //set of socket descriptors

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

            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )
            {
                perror("send");
            }

            printf(BLU "Welcome message sent successfully!\n" RESET);

            for (i = 0; i < max_clients; i++) {
                if( client_socket[i] == 0 ) {
                    client_socket[i] = new_socket;
                    printf(BLU "Adding to list of sockets as %d...\n" RESET, i);
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
                if ((valread = read( sd , buffer, MAX_BUFFER_SIZE)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);
                    printf(RED "Host disconnected on socket %i, ip %s , port %d \n" RESET, sd, inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                    remove_disconnected_user(hash, address);
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

                    if (strncmp(buffer, "!name", 5) == 0) {
                        char separator = ' ';
                        char * const tempName = strchr(buffer, separator);
                        if(tempName != NULL) {
                          *tempName = '\0';
                        }

                        // printf("New name : %s end\n", tempName+1);
                        char newName[100];
                        strcpy(newName, tempName+1);

                        change_name(hash, newName, inet_ntoa(address.sin_addr), messageToServer, messageToCaller, messageToOthers);
                        printf(GRN "COMMAND DETECTED : %s\n", buffer);
                        int callers[2];
                        callers[0] = i;
                        callers[1] = i+1;
                        respond_to_group(client_socket, callers, 2, messageToServer, messageToCaller, messageToOthers);

                        continue;
                    }
                    
                    else {
                        buffer[valread] = '\0';
                        char* username = retrieve_username(hash, inet_ntoa(address.sin_addr));
                        char* username_copy;
                        strcpy(username_copy, username);
                        char* new_message = strcat(strcat(username_copy, " says: "), buffer);
                        // printf("new_message is: %s\n", new_message);
                        respond(client_socket, i, new_message, "", new_message);
                        memset(new_message, 0 , 512);
                        memset(buffer, 0 , MAX_BUFFER_SIZE);
                    }
                }
            }
        }
    }
    return 0;
}

