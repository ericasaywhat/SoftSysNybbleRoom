// #include "utils.h"

#define WINMSG "You win!\n"
#define LOSEMSG "You lose!\n"
#define TIEMSG "It's a tie!\n"

bool fin;

void p1_wins(int p1Socket, int p2Socket){
	send(p1Socket, WINMSG, strlen(WINMSG), 0);
	send(p2Socket, LOSEMSG, strlen(LOSEMSG), 0);
	fin = true;
}

void p2_wins(int p1Socket, int p2Socket){
	send(p1Socket, LOSEMSG, strlen(LOSEMSG), 0);
	send(p2Socket, WINMSG, strlen(WINMSG), 0);
	fin = true;
}

void tie(int p1Socket, int p2Socket){
	send(p1Socket, TIEMSG, strlen(TIEMSG), 0);
	send(p2Socket, TIEMSG, strlen(TIEMSG), 0);
	fin = true;
}

char *get_p1_move(int p1Socket){
	char *move;
    // while(move == NULL) {
    	read(p1Socket, &move, sizeof(char));
    // }
	// char* choice = strcat("You've chosen", *move);
    // send(p1Socket, choice, strlen(choice), 0);
    return move;
}

char *get_p2_move(int p2Socket){
	char *move;
	// while(move == NULL) {
		read(p2Socket, &move, sizeof(char));
	// }
	// char* choice = strcat("You've chosen", move);
    // send(p2Socket, choice, strlen(choice), 0);

    return move;
}


void rps_get_move(int socket, char *move) {
    char *s = malloc(sizeof(char) * 9 );
    bool valid = false;
    int i;
    char valid_moves[] = "rps";

    // while(strlen(s) == 0){
    	read(socket, s, sizeof(char)*9);
    // }

    for (i = 0; i < strlen(valid_moves); i++) {
        if (s[0] == valid_moves[i]) {
        	valid = true;
            *move = s[0];
        }
    }
}

void play_again(int p1Socket, int p2Socket, bool *want_to_play) {
	char *p1Response = malloc(sizeof(char)*4);
	char *p2Response = malloc(sizeof(char)*4);
	int valread;

	char* noPlay = "The other player does not want to play again\n";
	char* playAgain = "Do you want to play again?\n";

	send(p1Socket, playAgain, strlen(playAgain), 0);
	send(p2Socket, playAgain, strlen(playAgain), 0);
	puts("hewwo");
	valread = read( p1Socket, p1Response, sizeof(char)*4) || read(p2Socket, p2Response, sizeof(char)*4);
	if ( valread != 0) {
	    if (p1Response[0] == 'n') {
	    	send(p2Socket, noPlay, strlen(noPlay), 0);
	    	*want_to_play = false;
	    	puts("p1 says no");
	    	return;
    	}

	    if (p2Response[0] == 'n') {
	    	send(p1Socket, noPlay, strlen(noPlay), 0);
	    	*want_to_play = false;

	    	puts("p2 wants to no");

	    	return;
	    }

	}



	// while(strlen(p1Response) == 0 || strlen(p2Response) == 0){
    	// read(p1Socket, p1Response, sizeof(char)*4);
    	// read(p2Socket, p2Response, sizeof(char)*4);
    	// printf("p2 says %s\n", p2Response);
    	// puts("getting BOTH response");
    // }




}

void rps_play_game(int p1Socket, int p2Socket, bool* fin) {
	*fin = false;
	char p1move;
	char p2move;

	rps_get_move(p1Socket, &p1move);
	rps_get_move(p2Socket, &p2move);
	puts("help");

	if(p1move == 'r'){
		if(p2move == 'r') tie(p1Socket, p2Socket);
		else if(p2move == 'p') p2_wins(p1Socket, p2Socket);
		else if(p2move == 's') p1_wins(p1Socket, p2Socket);
	} else if (p1move == 'p'){
		if(p2move == 'p') tie(p1Socket, p2Socket);
		else if(p2move == 's') p2_wins(p1Socket, p2Socket);
		else if(p2move == 'r') p1_wins(p1Socket, p2Socket);
	} else if (p1move == 's'){
		if(p2move == 's') tie(p1Socket, p2Socket);
		else if(p2move == 'r') p2_wins(p1Socket, p2Socket);
		else if(p2move == 'p') p1_wins(p1Socket, p2Socket);
	}


}

void rps_game_start(char* p1Name, int p1Socket, char* p2Name, int p2Socket){
	bool want_to_play = true;

 	while (want_to_play) {
        rps_play_game(p1Socket, p2Socket, &fin);
        if (fin) {  play_again(p1Socket, p2Socket, &want_to_play);  }

    }
        //free stuff


}





