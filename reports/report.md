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

#### Username Changes

Once the client has been added to the hashtable, they are able to start sending messages to other clients. In addition to sending messages, the client may also choose to change their username by entering `!name [new name]`. When a name change request is found, our program looks up the client in the hashtable and changes the stored name. Here is an example of output from this process:

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

#### Rock-Paper-Scissors

One client, Player 1, may also choose to play a game of rock-paper-scissors with another client, Player 2. using the `!rps [other client's name]` command. The server sends a text prompt to Player 2 to let them know that they are now in a game of rock-paper-scissors. Player 1 and Player 2 are then prompted to input a move (`r` for rock, `p` for paper, and `s` for scissors). After both players have entered their respective choices, they both see the results of the game and are asked whether they want to play again.

Once a game of rock-paper-scissors has completed successfully, the program uses a `select` system call to get the responses for whether or not they want to play again. If they do not respond within the time limit of 3 seconds, the `select` system call times out and another round of rock-paper-scissors starts. The players are prompted to input a move again.

If the players do not want to play again, both clients may quit the game and return to the group chat, where they are able to see what other clients have said in the group chat while they have been playing the game.

#### Private Messaging

[INSERT PRIVATE MESSAGING EXPLANATION HERE]

#### Disconnecting

Once a client disconnects, they are removed from the hashtable and the socket associated with them is freed. We choose to implement this behavior, as opposed to leaving the client in the hashtable in case they return to the chat room later, because it is more space efficient. Should a client return to the chat room, they must go through the same process that they did when they first joined, where their information is stored in the hashtable again.

## Demonstration

Click the gif below to view the entire demonstration on YouTube.

[![alt text](https://j.gifs.com/kZLrZE.gif)](https://www.youtube.com/watch?v=UZxQNI3Gttg)

## Preventing Memory Leaks
Our program frees all memory allocations so that the results of valgrind yield:
```
==26589== LEAK SUMMARY:
==26589==    definitely lost: 0 bytes in 0 blocks
==26589==    indirectly lost: 0 bytes in 0 blocks
==26589==      possibly lost: 0 bytes in 0 blocks
==26589==    still reachable: 3,437 bytes in 13 blocks
```
We currently have a `free_value` function that frees the `Value` data structure. This function takes in a `Value` and frees all the pointers within the `Value` before freeing the pointer to that `Value`. Currently, the GLib hashtable of clients is meant to persist until the end of the program. Given more time, we would improve memory management by implementing more functions for a more organized and efficient way of freeing memory.

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

We were able to achieve all of our original learning goals, in addition to learning some additional skills along the way, as we became more familiar with using GLib hashtables, string operations in C, and more advanced usages of pointers. One of the most valuable experiences during this project was debugging and understanding the errors that we found.

We were not able to achieve all of our stretch goals for this project; we were unable to implement a way for players to play _Hunt the Wumpus_ and we were also unable to implement our program using threads and forks, even though this might have made the program less fallible. For example, when one user is specifying their username, any new users are unable to join the server because they are blocked while the main process waits for the first user to finish. In this way, our program is like a single threaded process, while it would have functioned much more smoothly as a multithreaded process.

However, we did achieve several stretch goals, including allowing users to change their names in the chat room and allowing users to privately message each other. We are confident that we could have implemented more stretch goals, but in the end, we were unable to due to time constraints.

## Resources

We would like to thank _Head First C_ by David and Dawn Griffiths and _ThinkOS_ by our instructor, Allen Downey, for being helpful resources with regards to how servers can be implemented, as well as being generally comprehensive guides to programming in C. We would like to thank Github user silv3rm00n for the code they provided in [this article](https://www.binarytides.com/multiple-socket-connections-fdset-select-linux/), which served as the foundation of our project. We would like to thank Stack Overflow for answering almost every possible debugging question we had. Lastly and mostly, we would like to thank the Software Systems teaching team (Allen and Serena) for supporting us in our endeavors and guiding us in the right direction always.
