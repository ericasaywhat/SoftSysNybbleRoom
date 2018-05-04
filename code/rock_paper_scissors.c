// #include "utils.h"

#define WINMSG "You win!\n"
#define LOSEMSG "You lose!\n"
#define TIEMSG "It's a tie!\n"

bool want_to_play;

void p1_wins(int p1Socket, int p2Socket){
	send(p1Socket, WINMSG, strlen(WINMSG), 0);
	send(p2Socket, LOSEMSG, strlen(LOSEMSG), 0);
}

void p2_wins(int p1Socket, int p2Socket){
	send(p1Socket, LOSEMSG, strlen(LOSEMSG), 0);
	send(p2Socket, WINMSG, strlen(WINMSG), 0);
}

void tie(int p1Socket, int p2Socket){
	send(p1Socket, TIEMSG, strlen(TIEMSG), 0);
	send(p2Socket, TIEMSG, strlen(TIEMSG), 0);
}

char *get_p1_move(int p1Socket){
	char *move;
    while(move == NULL) {  read(p1Socket, &move, sizeof(char));  }
	// char* choice = strcat("You've chosen", *move);
    // send(p1Socket, choice, strlen(choice), 0);
    return move;
}

char *get_p2_move(int p2Socket){
	char *move;
	while(move == NULL) {  read(p2Socket, &move, sizeof(char));  }
	// char* choice = strcat("You've chosen", move);
    // send(p2Socket, choice, strlen(choice), 0);

    return move;
}


void rps_get_key_press(int socket, char *move) {
    char *s = malloc(sizeof(char) * 9 );;
    bool valid = false;
    int i;
    char valid_moves[] = "rps";

    while(strlen(s) == 0){
    	read(socket, s, sizeof(char)*9);
    }

    for (i = 0; i < strlen(valid_moves); i++) {
        if (s[0] == valid_moves[i]) {
        	valid = true;
            *move = s[0];
        }
    }
}

// void play_again(int socket, int *want_to_play) {
// 	char response[2];

// 	// if (response[0] == 'n') {
//  //            puts("Exiting...");
//  //            want_to_play = false;
//  //    } else {
//  //        puts("Great, let's play another round!\n");
//  //    }

// }

void rps_play_game(int p1Socket, int p2Socket) {
	char p1move;
	char p2move;

	rps_get_key_press(p1Socket, &p1move);
	rps_get_key_press(p2Socket, &p2move);


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
	want_to_play = true;

 	while (want_to_play) {
        rps_play_game(p1Socket, p2Socket);
        // play_again(&want_to_play);

    }
        //free stuff


}





