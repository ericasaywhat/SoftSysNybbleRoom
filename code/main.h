#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <signal.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define WIDTH 5
#define HEIGHT 5

#define PIT 1
#define BAT 2
#define WUM 3
#define ARROW 4
#define PLAYER 5

#define PITDEATH 0
#define WUMDEATH 1
#define PLAYERWIN 2

int kills;
int deaths;
bool game_over;
bool want_to_play;

typedef struct {
	int x;
	int y;
} Object;

typedef struct {
	int width;
	int height;
	int numArrows;
	Object* player;
	Object* pit;
	Object* wum;
	Object* bat;
	int** coords;
} Map;

Map* map;

Map* make_map();
Object* make_object();
void placeObject(Object* object, int identity);
int** coords();
void playerMovement(char direction);
bool playerCanMove(char direction);
bool moveSuccessful(char direction);
void shoot(char direction);
void placeArrow();
void checkConsequences();
void wumpusMovement();
void batAbduction();
void endGame(int condition);
void whereisPlayer();
void printMap();
void printMaskedMap();
void free_objects();
void free_map();
void INThandler(int sig);
void getKeyPress();
void playGame();
void game_start();