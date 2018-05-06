// #include "utils.h"

#define WINMSG "You win!\n"
#define LOSEMSG "You lose!\n"
#define TIEMSG "It's a tie!\n"

int fin;

char* gen_msg(char* p1Name, char* p2Name, char p1move, char p2move){
    char* temp1 = malloc(sizeof(char) * 100);
    char* temp2 = malloc(sizeof(char) * 100);

    strcpy(temp1, p1Name);
    strcpy(temp2, p2Name);

	char* append_p1 = strcat(strcat(temp1, " put "), &p1move);
	char* append_p2 = strcat(strcat(temp2, " put "), &p2move);
	char* moves = strcat(strcat(append_p1, " "), append_p2);

	free(temp1);
	free(temp2);
	// free(append_p1);
	// free(append_p2);

	return moves;
}


void p1_wins(int p1Socket, int p2Socket, char* msg){
	puts("p1wins");
	char* winmsg = malloc(sizeof(char)*MAX_BUFFER_SIZE);
	char* losemsg = malloc(sizeof(char)*MAX_BUFFER_SIZE);
	strcpy(winmsg, msg);
	strcat(strcat(winmsg, " "), WINMSG);

	strcpy(losemsg, msg);
	strcat(strcat(losemsg, " "), LOSEMSG);

	send(p1Socket, winmsg, strlen(winmsg), 0);
	send(p2Socket, losemsg, strlen(losemsg), 0);

	free(winmsg);
	free(losemsg);

	fin = 1;
}

void p2_wins(int p1Socket, int p2Socket, char* msg){
	puts("p2wins?");

	char* winmsg = strcat(strcat(msg, " "), WINMSG);
	char* losemsg = strcat(strcat(msg, " "), LOSEMSG);

	send(p1Socket, losemsg, strlen(losemsg), 0);
	send(p2Socket, winmsg, strlen(winmsg), 0);

	free(winmsg);
	free(losemsg);

	fin = 1;
}

void tie(int p1Socket, int p2Socket, char* msg){
	char* tiemsg = strcat(strcat(msg, " "), TIEMSG);

	send(p1Socket, tiemsg, strlen(tiemsg), 0);
	send(p2Socket, tiemsg, strlen(tiemsg), 0);
	fin = 1;
}


void rps_get_move(int socket, char *move) {
    char *s = malloc(sizeof(char) * 9 );
    int valid = 0;
    int i;
    char valid_moves[] = "rps";

    char* inputMove = "Input your move [r/p/s]\n";

	send(socket, inputMove, strlen(inputMove), 0);
	read(socket, s, sizeof(char)*9);
	// printf("nyaaaa %s\n", s);


    for (i = 0; i < strlen(valid_moves); i++) {
        if (s[0] == valid_moves[i]) {
        	valid = 1;
            *move = s[0];
        }
    }
    free(s);
}

void rps_get_response(int socket, char *resp, int* want_to_play) {
	fd_set set;
  	struct timeval timeout;
  	int rv;
	FD_ZERO(&set); /* clear the set */
	FD_SET(socket, &set); /* add our file descriptor to the set */

    char *s = malloc(sizeof(char) * 9 );
    int valid = 0;
    int i;
    char valid_moves[] = "yn";
    char *timesup = "Time is up. Quitting\n";

	timeout.tv_sec = 0;
	timeout.tv_usec = 3000000;

	rv = select(socket +1, &set, NULL, NULL, &timeout);
	if(rv == -1) {  perror("select");  } /* an error accured */
	else if(rv == 0) {
        *want_to_play = 0;
        send(socket, timesup, strlen(timesup), 0);
        return;

	} else {  read( socket, s, sizeof(char)*9 );  } /* there was data to read */


    for (i = 0; i < strlen(valid_moves); i++) {
        if (s[0] == valid_moves[i]) {
        	valid = 1;
            *resp = s[0];
        }
    }
    free(s);
}


void play_again(int p1Socket, int p2Socket, int *want_to_play) {
	char p1Response;
	char p2Response;
	int valread;

	char* noPlay = "The other player does not want to play again\n";
	char* playAgain = "Do you want to play again?\n";

	send(p1Socket, playAgain, strlen(playAgain), 0);
	send(p2Socket, playAgain, strlen(playAgain), 0);

	rps_get_response(p1Socket, &p1Response, &want_to_play);
	rps_get_response(p2Socket, &p2Response, &want_to_play);

    if (p1Response == 'n') {
    	send(p2Socket, noPlay, strlen(noPlay), 0);
    	*want_to_play = 0;
    	return;
	}

    if (p2Response == 'n') {
    	send(p1Socket, noPlay, strlen(noPlay), 0);
    	*want_to_play = 0;
    	return;
    }

	// while(strlen(p1Response) == 0 || strlen(p2Response) == 0){
    	// read(p1Socket, p1Response, sizeof(char)*4);
    	// read(p2Socket, p2Response, sizeof(char)*4);
    	// printf("p2 says %s\n", p2Response);
    	// puts("getting BOTH response");
    // }

}

void rps_play_game(char* p1Name, char* p2Name, int p1Socket, int p2Socket, int* fin) {
	*fin = 0;
	char p1move;
	char p2move;

	rps_get_move(p1Socket, &p1move);
	rps_get_move(p2Socket, &p2move);

	char* msg = gen_msg(p1Name, p2Name, p1move, p2move);

	if(p1move == 'r'){
		if(p2move == 'r') {
			tie(p1Socket, p2Socket, msg);
		} else if(p2move == 'p') {
			p2_wins(p1Socket, p2Socket, msg);
		} else if(p2move == 's') {
			p1_wins(p1Socket, p2Socket, msg);
		}
	} else if (p1move == 'p'){
		if(p2move == 'p') {
			tie(p1Socket, p2Socket, msg);
		} else if(p2move == 's') {
			p2_wins(p1Socket, p2Socket, msg);
		} else if(p2move == 'r') {
			p1_wins(p1Socket, p2Socket, msg);
		}
	} else if (p1move == 's'){
		if(p2move == 's') {
			tie(p1Socket, p2Socket, msg);
		} else if(p2move == 'r') {
			p2_wins(p1Socket, p2Socket, msg);
		} else if(p2move == 'p') {
			p1_wins(p1Socket, p2Socket, msg);
		}
	}


}

void rps_game_start(char* p1Name, int p1Socket, char* p2Name, int p2Socket){
	int want_to_play = 1;

 	while (want_to_play) {
        rps_play_game(p1Name, p2Name, p1Socket, p2Socket, &fin);
        if (fin) {
            play_again(p1Socket, p2Socket, &want_to_play);

        }

    }
        //free stuff


}





