# NybbleRoom Project Report: Online Chat Room

#### Leon Lam, Erica Lee, and Emily Yeh

#### Spring 2018

## Goals

Our primary goal for this project is to learn how to use C in the context of network programming, clients, and servers. We also hope to figure out how to run a program with multiple concurrent users.

In order to achieve these learning goals, we have created a chat room with an interface where users can play multiplayer games with each other online.


## Current Progress

We implemented a server that is able to host clients via `telnet`. For example, if there are multiple people connected to the same network, one person can run our program using the following command:

```
make main && ./main
```

and other people can join their server by using the following command:

```
telnet [INSERT SERVER IP ADDRESS HERE] 3000
```

and the new clients are able to talk to type and talk to each other via their terminals. Clients are also able to enter commands using `!` to specify commands, and the server may enter the `!wumpus` command to play Hunt the Wumpus.

We have also implemented other features, such as allowing each user to specify the name they would like to use when they are in the chat room and keeping track of the current clients using a hashmap. This hashmap keeps track of each client's IP address, their hash key, and their chat room username, which they can specify when they first enter the chat room server.
