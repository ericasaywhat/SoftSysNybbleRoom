// #include "utils.h"


void p1_wins(int p1Socket, int p2Socket){
	char* p1Message = "You win!";
	char* p2Message = "You lose!";
	send(p1Socket, p1Message, strlen(p1Message), 0);
	send(p2Socket, p2Message, strlen(p2Message), 0);
}

void p2_wins(int p1Socket, int p2Socket){
	char* p2Message = "You win!";
	char* p1Message = "You lose!";
	send(p1Socket, p1Message, strlen(p1Message), 0);
	send(p2Socket, p2Message, strlen(p2Message), 0);
}

void tie(int p1Socket, int p2Socket){
	char* message = "It's a tie!";
	send(p1Socket, message, strlen(message), 0);
	send(p2Socket, message, strlen(message), 0);
}

char get_p1_move(int p1Socket){
	char move;
    read(p1Socket, &move, sizeof(char));
    return move;
}

char get_p2_move(int p2Socket){
	char move;
    read(p2Socket, &move, sizeof(char));
    return move;
}

void play_rps(char* p1Name, int p1Socket, char* p2Name, int p2Socket){
	int playing = 1;
	char* wait4p1 = "Waiting for Player 1\n";
	char* wait4p2 = "Waiting for Player 2\n";

	char* readerr = "Can't read your move. Please try again!\n";
	//check for errors
	send(p1Socket, wait4p2, strlen(wait4p2), 0);
	send(p2Socket, wait4p1, strlen(wait4p1), 0);

	char p1move = get_p1_move(p1Socket);
	char p2move = get_p2_move(p2Socket);
	// while(p1move != 'r' && p1move != 'p' && p1move != 's') {
	// 	send(p1Socket, readerr, strlen(readerr), 0);
	// 	char p1move = get_p1_move(p1Socket);
	// }

	// while(p2move != 'r' && p2move != 'p' && p2move != 's') {
	// 	send(p2Socket, readerr, strlen(readerr), 0);
	// 	char p2move = get_p2_move(p1Socket);
	// }
	if(p1move == 'r'){
		if(p2move == 'r') tie(p1Socket, p2Socket);
		if(p2move == 'p') p2_wins(p1Socket, p2Socket);
		if(p2move == 's') p1_wins(p1Socket, p2Socket);
	} else if (p1move == 'p'){
		if(p2move == 'p') tie(p1Socket, p2Socket);
		if(p2move == 's') p2_wins(p1Socket, p2Socket);
		if(p2move == 'r') p1_wins(p1Socket, p2Socket);
	} else if (p1move == 's'){
		if(p2move == 's') tie(p1Socket, p2Socket);
		if(p2move == 'r') p2_wins(p1Socket, p2Socket);
		if(p2move == 'p') p1_wins(p1Socket, p2Socket);
	}
}





