#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <ctype.h>

void p1wins(int p1socket, int p2socket){
	char* p1message = "You win!";
	char* p2message = "You lose!";
	send(p1socket, p1message, strlen(p1message), 0);
	send(p2socket, p2message, strlen(p2message), 0);
}

void p2wins(int p1socket, int p2socket){
	char* p2message = "You win!";
	char* p1message = "You lose!";
	send(p1socket, p1message, strlen(p1message), 0);
	send(p2socket, p2message, strlen(p2message), 0);
}

void tie(int p1socket, int p2socket){
	char* message = "It's a tie!";
	send(p1socket, message, strlen(message), 0);
	send(p2socket, message, strlen(message), 0);
}

char getP1Move(int p1socket){
	char move;
    read(p1socket, &move, sizeof(char));
    return move;
}

char getP2Move(int p2socket){
	char move;
    read(p2socket, &move, sizeof(char));
    return move;
}

char* playrps(int p1socket, int p2socket){
	char* query_name = "PLAYING ROCK PAPER SCISSORSSSSSS!";
	char* wait4p1 = "Waiting for Player 1";
	char* wait4p2 = "Waiting for Player 2";
	//check for errors
	send(p1socket, wait4p2, strlen(wait4p2), 0);
	send(p2socket, wait4p1, strlen(wait4p1), 0);
	char p1move = getP1Move(p1socket);
	char p2move = getP2Move(p2socket);

	if(p1move == 'r'){
		if(p2move == 'r') tie(p1socket, p2socket);
		if(p2move == 'p') p2wins(p1socket, p2socket);
		if(p2move == 's') p1wins(p1socket, p2socket);
	} else if (p1move == 'p'){
		if(p2move == 'p') tie(p1socket, p2socket);
		if(p2move == 's') p2wins(p1socket, p2socket);
		if(p2move == 'r') p1wins(p1socket, p2socket);
	} else if (p1move == 's'){
		if(p2move == 's') tie(p1socket, p2socket);
		if(p2move == 'r') p2wins(p1socket, p2socket);
		if(p2move == 'p') p1wins(p1socket, p2socket);
	}
	// send(p1socket, query_name , strlen(query_name) , 0 );
}





