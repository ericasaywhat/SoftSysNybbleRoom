# NybbleRoom Project Update: Online Chat Room

#### Leon Lam, Erica Lee, and Emily Yeh

#### Spring 2018

## Goals

Our primary goal for this project is to learn how to use C in the context of network programming, clients, and servers. We will figure out what web applications with multiple concurrent users need, such as multiple threads.

In order to achieve these learning goals, we plan to create a chat room with an interface where users can play multiplayer games with each other online. Our minimum viable product is a chat room that allows multiple users to talk with each other, as well as play a simple multiplayer game by sending commands to the server.

A stretch goal that we might work towards is allowing players to play more complex games, like Hunt the Wumpus or Battleship.

## Current Progress

We implemented a server that is able to host clients via `telnet`. For example, if there are multiple people connected to the same network, one person can run our program using the following command:

```
make main && ./main
```

and other people can join their server by using the following command:

```
telnet [INSERT SERVER IP ADDRESS HERE] 3000
```

and the new clients are able to talk to type and talk to each other via their terminals. Clients are also able to enter commands using `!` to specify commands, and the server may enter the `!wumpus` command to play the Wumpus game.

Other features we have implemented include allowing each user to specify the name they would like to use when they are in the chat room and keeping track of the current clients using a hashmap. This hashmap keeps track of each client's IP address, their hash key, and their chat room username, which they can specify when they first enter the chat room server.

## Next Steps

We have completed the chat room part of our project, although there are numerous features and improvements we would like to implement.

* First, we want to allow the user to change their name in the chat room using a `!` command. Currently, we are able to retrieve commands using the `!` flag. In order to complete this task, we need to find a way to edit the hashmap in a way that allows a client to change their name whenever they want.

* Second, we want to allow the clients to play the Wumpus game together by alternating turns - the Wumpus game is fully implemented, but currently, only the server is able to play the game, so we must implement a way for the server to help the clients play. Once implemented, each client will be able to enter a command, like `UP ARROW` or `UP MOVE`, and in this way, the player in the Wumpus game will be able to shoot and move.

* Third, we would like to reorganize the code, since much of it is stuffed into the `main` function and it is difficult to navigate. We are fairly sure we can break down the `main` function into several helper functions, which should improve readability and debuggability.
