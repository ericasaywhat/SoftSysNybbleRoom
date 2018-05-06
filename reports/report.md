# NybbleRoom Project Report: Online Chat Room

#### Leon Lam, Erica Lee, and Emily Yeh

#### Spring 2018

## Goals

Our learning goals for this project are to learn how to use C in the context of network programming, clients, and servers, how to manage memory effectively, and how to be more knowledgeable C programmers.

In order to achieve these learning goals, we choose to implement an online chat room in C with a server that hosts multiple clients, which can speak to each other and play games together. We figure out what web applications with multiple concurrent users need, such as sockets and data structures used to store user information. We also figure out how to identify potential memory leaks and fix them. Along the way, we pick up numerous useful tricks for using C, which we use to make our code more efficient and easier to read.

## Implementation Details

We use the `select` method to implement our program. This method allows the program to monitor multiple sockets for activity, blocking until activity occurs. This is useful because our program requires that we wait on multiple clients to join and provide information.

```
activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
```

When a new client joins the chat room, `select` returns and a new connection is set up. First, the client's information is added to a GLib hashtable. To keep track of all of the clients in the chat room, we use several data structures. In the header file `utils.h`, we list macros and function declarations, as well as an important struct: `Value`.

```
typedef struct data_struct_s {
    char *name;
    char ip[KEY_MAX_LENGTH];
    char key_string[KEY_MAX_LENGTH];
    int socket_file_descriptor;
} Value;
```

`Value` is the data structure we use to keep track of the client's name, IP address, the hashtable key associated with the client, and the client's socket file descriptor. In turn, each `Value` is stored as the value of a key-value pair in the GLib hashtable, where the key is a string of characters that is unique to each client. In this case, we define the key as a combination of the string `nybbles_` and the client's IP address, since each client's IP address should be unique.

Second, once the client has been added to the hashtable, they are able to start sending messages to other clients. In addition to sending messages, the client may also choose to change their username by entering `!name [new name]`. When a name change request is found, our program looks up the client in the hashtable and changes the stored name. Here is an example of output from this process:

```
Listener on port 3000
Waiting for connections...
ADDING NEW CONNECTION
SOCKET : 4, IP : 127.0.0.1, PORT : 33958 
IP : 127.0.0.1
KEY_STRING : nybbles_127.0.0.1
STORED KEY_STRING : nybbles_127.0.0.1
NEW SOCKET'S NAME : Emily 
...
Emily says: Hello!
Emily says: I'm going to change my name.
REPLACING KEY_STRING VALUE AT : nybbles_127.0.0.1
Name change detected.
Example says: Hello! I changed my name!
```

The client may also choose to play a game with another client using the `!rps [other client's name]` command. [INSERT RPS EXPLANATION HERE]

Finally, once a client disconnects, they are removed from the hashtable and the socket associated with them is freed. We choose to implement this behavior, as opposed to leaving the client in the hashtable in case they return to the chat room later, because it is more space efficient. Should a client return to the chat room, they must go through the same process that they did when they first joined, where their information is stored in the hashtable again.

## Preventing Memory Leaks



## How to Run the Chat Room

Our program's server is able to host clients via `telnet`. For example, if there are multiple people connected to the same network, one person can run our program using the following command:

```
make main && ./main
```

Then, other people can join their server by using the following command:

```
telnet [server IP address] 3000
```

The new clients will then be able to talk to each other via their terminals.

## Reflection



## Resources

We would like to thank _Head First C_ by David and Dawn Griffiths and _ThinkOS_ by our instructor, Allen Downey, for being helpful resources with regards to how servers can be implemented, as well as being generally comprehensive guides to programming in C. We would like to thank Github user silv3rm00n for the code they provided in [this article](https://www.binarytides.com/multiple-socket-connections-fdset-select-linux/), which served as the foundation of our project. We would like to thank Stack Overflow for answering almost every possible debugging question we had. Lastly and mostly, we would like to thank the Software Systems teaching team (Allen and Serena) for supporting us in our endeavors and guiding us in the right direction always.
