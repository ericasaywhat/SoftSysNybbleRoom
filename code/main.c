/*
 * HUNT THE WUMPUS!
 * A Software Systems Spring 2018 project by Erica Lee ('19) and Emily Yeh ('19)
 * with some help from James (Zhecan) Wang ('18).
 *
 * This game is meant to be as similar as possible to the official Hunt the Wumpus
 * game invented by Gregory Yob, although we did not implement the squashed
 * dodecahedron map and instead opted for a 5x5 square map.
 *
 * We hope this game is as fun for others as it was for us to create.
 */

#include "main.h"

/*
 * Creates the game Map, including placing Objects on the Map.
 * Initializes locations of the player, the pit, the Wumpus, and the bats.
 *
 * return: pointer to newly created Map
 */
Map* make_map(){
    map = malloc(sizeof(Map));
    map->width = 5;
    map->height = 5;
    map->numArrows = 5;
    map->coords = coords();
    Object* player = make_object();
    Object* pit = make_object();
    Object* wum = make_object();
    Object* bat = make_object();

    map->player = player;
    map->pit = pit;
    map->wum = wum;
    map->bat = bat;

    placeObject(map->pit, PIT);
    placeObject(map->bat, BAT);
    placeObject(map->wum, WUM);
    placeObject(map->player, PLAYER);
    checkConsequences();
    whereisPlayer();

    return map;
}

/*
 * Allocates space to an Object and returns a pointer to the Object.
 *
 * return: pointer to newly created Object
 */
Object* make_object(){
    Object* obj = malloc(sizeof(Object));
    return obj;
}

/*
 * Places an object on the map.
 *
 * object: Object to be placed
 * identity: ID of the object to be placed
 */
void placeObject(Object* object, int identity) {
    time_t t;
    srand((unsigned) time(&t));

    int x = rand()%map->width;
    int y = rand()%map->height;
    while (map->coords[y][x] != 0) {
        x = rand()%map->width;
        y = rand()%map->height;
    }

    object->x = x;
    object->y = y;

    map->coords[y][x] = identity;
}

/*
 * Generates an empty 2D array for a Map to populate.
 *
 * return: pointer to pointer to 2D array
 */
int** coords() {
    int i, j;
    int** coords = malloc(HEIGHT * WIDTH * sizeof(int*));

    for(i=0; i<HEIGHT; i++) {
        int* values = calloc(WIDTH*HEIGHT, sizeof(int));
        coords[i] = values;
    }
    return coords;
}

/*
 * Allows the player to shoot or move in a given direction.
 *
 * direction: char direction to shoot or move in
 */
void playerMovement(char direction){
    switch(direction) {
    //player shooting
    case 'W':
    case 'A':
    case 'S':
    case 'D':
        shoot(direction);
        break;
    default:
        if (playerCanMove(direction) && moveSuccessful(direction)) {
            if (map->coords[map->player->y][map->player->x] == ARROW) {
                map->numArrows++;
                printf("You found an arrow! Arrows remaining: %i.\n", map->numArrows);
            }
            map->coords[map->player->y][map->player->x] = PLAYER;
            checkConsequences();
            if (!game_over) {
                whereisPlayer();
                printMaskedMap();
            }
        } else {
            printf("You could not move in direction: %c. Please try again.\n", direction);
            printMaskedMap();
        }
    }
}

/*
 * Determines whether it's possible for the player to move in the given direction.
 *
 * direction: char direction the player wants to move in
 */
bool playerCanMove(char direction) {
    if ((direction == 'a' && map->player->x == 0) ||
        (direction == 'd' && map->player->x == WIDTH-1) ||
        (direction == 's' && map->player->y == HEIGHT-1) |
        (direction == 'w' && map->player->y == 0) ||
        (direction != 'a' && direction != 'w' &&
            direction != 's' && direction != 'd')) {
        return false;
    } else {
        return true;
    }
}

/*
 * Moves the player.
 *
 * direction: char direction to move the player in
 */
bool moveSuccessful(char direction) {
    map->coords[map->player->y][map->player->x] = 0;
    switch (direction) {
        case 'w':
            map->player->y--;
            break;
        case 'a':
            map->player->x--;
            break;
        case 's':
            map->player->y++;
            break;
        case 'd':
            map->player->x++;
            break;
        default:
            return false;
    }
    return true;
}

/*
 * Shoots an arrow in the direction inputted by the user.
 *
 * direction: char direction to shoot in ('WASD')
 */
void shoot(char direction){
    if (map->numArrows == 0) {
        puts("You are out of arrows! Try walking around to find more.");
    } else {
        map->numArrows--;

        switch(direction) {
            case 'W':
            if (map->player->y < map->wum->y &&
                map->player->x == map->wum->x) {
                wumpusMovement();
            } else if (map->player->y > map->wum->y &&
                map->player->x == map->wum->x) {
                endGame(PLAYERWIN);
            }
            break;
            case 'S':
            if(map->player->y > map->wum->y &&
                map->player->x == map->wum->x){
                wumpusMovement();
            } else if(map->player->y < map->wum->y &&
                map->player->x == map->wum->x){
                endGame(PLAYERWIN);
            }
            break;
            case 'A':
            if(map->player->x < map->wum->x &&
                map->player->y == map->wum->y){
                wumpusMovement();
            } else if (map->player->x > map->wum->x &&
                map->player->y == map->wum->y) {
                endGame(PLAYERWIN);
            }
            break;
            case 'D':
            if(map->player->x > map->wum->x &&
                map->player->y == map->wum->y){
                wumpusMovement();
            } else if(map->player->x < map->wum->x &&
                map->player->y == map->wum->y){
                endGame(PLAYERWIN);
            }
            break;
        }

        if (!endGame) {
            printf("You successfully shot an arrow in direction %c. Arrows remaining: %i.\n",
            direction, map->numArrows);

            if (map->numArrows == 0) {
                placeArrow();
            }
        }
    }
}


/*
 * Checks consequences of the player's location.
 * If they are on the Wumpus or the pit, the game ends.
 * If they are near the Wumpus, a pit, or a bat, a text prompt is printed.
 */
void checkConsequences(){
    int playerx = map->player->x;
    int playery = map->player->y;
    int pitx = map->pit->x;
    int pity = map->pit->y;
    int wumx = map->wum->x;
    int wumy = map->wum->y;
    int batx = map->bat->x;
    int baty = map->bat->y;

    if ((playerx == pitx &&
        playery == pity) ||
        (playerx == pitx &&
        playery == pity)) {
        endGame(PITDEATH);
    } else if (playerx == batx &&
        playery == baty) {
        batAbduction();
    } else if (playerx == wumx &&
        playery == wumy) {
        endGame(WUMDEATH);
    } else {
        if ((playerx + 1 == wumx && playery == wumy) ||
            (playerx - 1 == wumx && playery == wumy) ||
            (playery + 1 == wumy && playerx == wumx) ||
            (playery - 1 == wumy && playerx == wumx)) {
            puts("You smell a wumpus! Proceed with caution...");
        }
        if ((playerx + 1 == pitx && playery == pity) ||
            (playerx - 1 == pitx && playery == pity) ||
            (playery + 1 == pity && playerx == pitx) ||
            (playery - 1 == pity && playerx == pitx)) {
            puts("You feel a breeze nearby. Watch your step...");
        }
        if ((playerx + 1 == batx && playery == baty) ||
            (playerx - 1 == batx && playery == baty) ||
            (playery + 1 == baty && playerx == batx) ||
            (playery - 1 == baty && playerx == batx)) {
            puts("You hear flapping nearby. Wonder what's making that sound?");
        }
    }
}

/*
 * Places an arrow at a random location in the map.
 */
void placeArrow() {
    time_t t;
    srand((unsigned) time(&t));

    int x = rand()%map->width;
    int y = rand()%map->height;
    while (map->coords[y][x] != 0) {
        x = rand()%map->width;
        y = rand()%map->height;
    }

    map->coords[y][x] = ARROW;
}

/*
 * Moves the Wumpus in a random direction.
 */
void wumpusMovement() {
    puts("You scared the Wumpus! You hear it running away to another part of the map...");
    map->coords[map->wum->y][map->wum->x] = 0;
    placeObject(map->wum, WUM);
}

/*
 * Abducts the player and moves them to a new part of the map.
 */
void batAbduction() {
    puts("Some giant friendly bats abduct you and carry you to another part of the map!");
    map->coords[map->player->y][map->player->x] = 0;
    placeObject(map->player, PLAYER);
}

/*
 * Ends the game and prints the appropriate statement.
 *
 * condition: 0 if player lost, 1 if player won
 */
void endGame(int condition){
    char c;

    switch (condition){
    case WUMDEATH:
        puts("Game Over... The Wumpus got you.");
        deaths++;
        break;
    case PITDEATH:
        puts("Game Over... You fell down a pit!");
        deaths++;
        break;
    case PLAYERWIN:
        puts("You killed the Wumpus! Congratulations!");
        kills++;
        break;
    }

    game_over = true;
}

/*
 * Prints the location of the player.
 * Player location goes left to right and top to bottom.
 */
void whereisPlayer() {
    printf("You are at %i,%i.\n", map->player->x, map->player->y);
}

/*
 * Prints the map. (Just used for debugging.)
 */
void printMap() {
    int i, j;

    for (i=0; i<5; i++){
        for (j=0; j<5; j++) {
            printf("%i ", map->coords[i][j]);
        }
        puts("\n");
    }
}

/*
 * Prints a masked version of the map for visual aid.
 */
void printMaskedMap() {
    int i, j;

    for (i = 0; i < WIDTH; i++) {
        for (j = 0; j < HEIGHT; j++) {
            if (map->coords[i][j] == PLAYER) {
                printf("O ");
            } else {
                printf("- ");
            }
        }
        puts("");
    }
}

/*
 * Frees the objects (player, bat, pit, Wumpus).
 */
void free_objects(){
    free(map->player);
    free(map->bat);
    free(map->pit);
    free(map->wum);
}

/*
 * Frees the map.
 */
void free_map(){
    int i;
    for (i = 0; i < HEIGHT; i++) {
        free(map->coords[i]);
    }
    free(map->coords);
    free(map);
}

/*
 * Handles a signal (generally, CTRL+C for quitting).
 *
 * sig: integer signal to be handled
 */
void INThandler(int sig) {
    char  c;

    signal(sig, SIG_IGN);
    printf("\nAre you sure you want to quit? [y/n] ");
    c = getchar();
    if (c == 'y' || c == 'Y'){
        free_objects(); // free objects before map
        free_map();
        exit(0);
    }
    else{
        signal(SIGINT, INThandler);
    }
    getchar(); // Get new line character
}

/*
 * Retrieves keys pressed by the user as commands for the game.
 */
void getKeyPress() {
    char s;
    int i;
    char valid_directions[] = "wasdWASD";

    s = getchar();

    for (i = 0; i < strlen(valid_directions); i++) {
        if (s == valid_directions[i]) {
            playerMovement(s);
        }
    }
}

/*
 * Creates a new map and runs the game.
 */
void playGame(){
    make_map();
    game_over = false;
    printMaskedMap();

    while (!game_over) {
        signal(SIGINT, INThandler);
        getKeyPress();
    }
}

void game_start() {
    char response[2];
    want_to_play = true;
    kills = 0;
    deaths = 0;
    while (want_to_play) {
        puts("***************************");
        puts("WELCOME TO HUNT THE WUMPUS!");
        puts("Use lowercase wasd keys to move around the map.");
        puts("Use uppercase WASD keys to shoot arrows. (You start out with 5.)");
        printf("You have killed the Wumpus %i times.\n",
            kills);
        printf("You have died %i times.\n",
            deaths);
        puts("Good luck!");
        puts("***************************\n");
        playGame();
        printf("Would you like to play again? [y/n] ");
        scanf("%s", response);
        if (response[0] == 'n') {
            puts("Exiting...");
            want_to_play = false;
        } else {
            puts("Great, let's play another round!\n");
        }
        free_objects(); // free objects before map
        free_map();
    }
}