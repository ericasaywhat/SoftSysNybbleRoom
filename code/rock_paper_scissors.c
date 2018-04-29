#include <stdio.h>
#include <string.h>
#include <sys/socket.h>


char* playrps(int p1socket, int p2socket){
	char* query_name = "PLAYING ROCK PAPER SCISSORSSSSSS!";
	send(p1socket, query_name , strlen(query_name) , 0 );
}

char playerMove(char* move){
	switch(move) {
    case 'r':
    case 'p':
    case 's':
        break;
    }
}

char getPlayerMove(){

}


