#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Constants */
#define MAX_INPUT 70

/* Type definations. */

/* Board type storing values of the game board. */
typedef struct {
    char **grid; /* 2 x 2 Matrix of the elements on the board. */
    int row; /* Rows of the board. */
    int col; /* Columns of the board. */
} Board;

/* Function prototypes. */
void free_tiles(char ***tiles, int tileAmount);
void check_err(int errorCode);
void computer_play(Board board, char **tile, char *p1Type, char *p2Type,
        int *currentPlayer, int *angle, int *p1RecentR, int *p1RecentC,
        int *p2RecentR, int *p2RecentC);
void human_play(Board board, char **tile, int tileCounter, 
        int *currentPlayer, int *p1RecentR, int *p1RecentC);
void play_game(char ***tiles, int tileAmount, int row, int col, char *file,
        char *p1Type, char *p2Type);
void draw_board(Board board);
int validate_input(char **args);
char **split(char *string, char *character);
char **generate_board(int row, int col);
char ***extract_tiles(char *charArr, int size, int *error);
char ***read_tile(char *filename, int *tileAmount, int *error);
void swap(char *elemA, char *elemB);
void transpose(char **tile);
void reverse(char **tile);
void rotate_tile(char **tile, int step);
void display_tileset(char **tile);
void display_tile(char **tile);
int pair_in_array(int x, int y, int array[25][2], int size);
int verify_tile_point(Board board, char **tile, int x, int y, int xMin,
        int yMin, int xMax, int yMax);
int place_tile(Board board, char **tile, int x, int y, int angle, int player,
        int test);
int game_is_over(Board board, char **tile);
int save(char *filename, Board board, int nextTile, int nextPlayer);
Board get_first_line(int counter, char *line);
Board load(char *filename, int *tileIndex, int *playerTurn);
void type1_play(Board board, char **tile, int *rStart, int *cStart,
        int *angle, int player);
void type2_play(Board board, char **tile, int *rStart, int *cStart,
        int *angle, int player);

/* Program entry point. */
int main(int argc, char **argv) {
    if (argc == 1 || argc == 3 || argc == 4 || argc > 6) { /* Invalid */
        check_err(1);
    }
    int tileAmount, errorCode = 0;
    char ***tiles = read_tile(argv[1], &tileAmount, &errorCode);
    if (errorCode == 0 && argc == 2) {
        for (int i = 0; i < tileAmount; i++) {
            display_tileset(tiles[i]);
            if (i != tileAmount - 1) {
                printf("\n");
            } 
        }
    } 
    check_err(errorCode);
    if (argc == 5 || argc == 6) {
        if (!(strcmp(argv[2], "h") == 0 || strcmp(argv[2], "1") == 0 ||
                strcmp(argv[2], "2") == 0) || !(strcmp(argv[3], "h") == 0 ||
                strcmp(argv[3], "1") == 0 || strcmp(argv[3], "2") == 0)) {
            free_tiles(tiles, tileAmount);
            check_err(4);
        }
        if (argc == 6) {
            if (!isdigit(*argv[4]) || !isdigit(*argv[5])) {
                free_tiles(tiles, tileAmount);
                check_err(5);
            }
            if (atoi(argv[4]) < 1 || atoi(argv[4]) > 999 ||
                    atoi(argv[5]) < 1 || atoi(argv[5]) > 999) {
                free_tiles(tiles, tileAmount);
                check_err(5);            
            }
            play_game(tiles, tileAmount, atoi(argv[5]), atoi(argv[4]), NULL,
                    argv[2], argv[3]);
        } else {
            play_game(tiles, tileAmount, 0, 0, argv[4], argv[2], argv[3]);
        }
    }
    free_tiles(tiles, tileAmount);
    return 0;
}

/** 
* Handles error and exits the program with the corrosponding exit status.
* Does nothing if the errorCode is 0.
*/
void check_err(int errorCode) {
    switch(errorCode) {
        case 1:
            fprintf(stderr, "Usage: fitz tilefile [p1type p2type " \
                    "[height width | filename]]\n");
            break;
        case 2:
            fprintf(stderr, "Can't access tile file\n");
            break;
        case 3:
            fprintf(stderr, "Invalid tile file contents\n");
            break;
        case 4:
            fprintf(stderr, "Invalid player type\n");
            break;
        case 5:
            fprintf(stderr, "Invalid dimensions\n");
            break;
        case 6:
            fprintf(stderr, "Can't access save file\n");
            break;
        case 7:
            fprintf(stderr, "Invalid save file contents\n");
            break;
        case 10:
            fprintf(stderr, "End of input\n");
            break;
    }
    if (errorCode > 0) {
        exit(errorCode);
    }
}

/* Frees memory allocated to tiles. */
void free_tiles(char ***tiles, int tileAmount) {
    for (int i = 0; i < tileAmount; i++) {
        for (int j = 0; j < 5; j++) {
            free(tiles[i][j]);
        }
        free(tiles[i]);
    }
    free(tiles);
}

/** 
* Method for an computer player to place a tile.
* board: The game board.
* tile: The tile to be played.
* p1Type: Player 1 type.
* p2Type: Player 2 type.
* currentPlayer: The current player playing.
* angle: The angle to place the tile.
* p1RecentR: Recent player 1 row played.
* p1RecentC: Recent player 1 column played.
* p2RecentR: Recent player 2 row played.
* p2RecentC: Recent player 2 column played.
*/
void computer_play(Board board, char **tile, char *p1Type, char *p2Type,
        int *currentPlayer, int *angle, int *p1RecentR, int *p1RecentC,
        int *p2RecentR, int *p2RecentC) {
    if ((strcmp(p1Type, "1") == 0 || strcmp(p1Type, "2") == 0) &&
            *currentPlayer) {
        if (strcmp(p1Type, "1") == 0) {
            type1_play(board, tile, *&p1RecentR, *&p1RecentC,
                    *&angle, *currentPlayer);
        } else {
            type2_play(board, tile, *&p2RecentR, *&p2RecentC,
                    *&angle, *currentPlayer);
            *p1RecentR = *p2RecentR;
            *p1RecentC = *p2RecentC;
        }
    } else if ((strcmp(p2Type, "1") == 0 || strcmp(p2Type, "2") == 0) &&
                !*currentPlayer) {
        if (strcmp(p2Type, "1") == 0) {
            type1_play(board, tile, *&p1RecentR, *&p1RecentC,
                    *&angle, *currentPlayer);
        } else {
            type2_play(board, tile, *&p2RecentR, *&p2RecentC,
                    *&angle, *currentPlayer);
            *p1RecentR = *p2RecentR;
            *p1RecentC = *p2RecentC;
        }
    }
    if (*currentPlayer) {
        printf("Player * => %i %i rotated %i\n", *p1RecentR,
                *p1RecentC, *angle);
    } else {
        printf("Player # => %i %i rotated %i\n", *p1RecentR,
                *p1RecentC, *angle);
    }
    *currentPlayer = !*currentPlayer;
}

/** 
* Method for an human player to place a tile.
* board: The game board.
* tile: The tile to be played.
* tileConunter: The index of the tile being played.
* currentPlayer: The current player playing.
* p1RecentR: Recent row played.
* p1RecentC: Recent column played.
*/
void human_play(Board board, char **tile, int tileCounter, 
        int *currentPlayer, int *p1RecentR, int *p1RecentC) {
    char response[MAX_INPUT], **args;
    int humanPlayed = 0;
    while (1) {
        *currentPlayer ? printf("Player *] ") : printf("Player #] ");
        if(fgets(response, sizeof(response), stdin) == NULL) {
            check_err(10);
        }
        if (strlen(response) > MAX_INPUT) {
            continue;
        }
        args = split(response, " ");
        int status = validate_input(args);
        humanPlayed = 1;
        if (status == 1) {
            if (place_tile(board, tile, atoi(args[1]),
                    atoi(args[0]), atoi(args[2]), *currentPlayer, 0)) {
                *p1RecentR = atoi(args[0]);
                *p1RecentC = atoi(args[1]);
                *currentPlayer = !*currentPlayer;
                break;
            }
        } else if (status == 2) {
            char *filename = malloc(sizeof(char) *
                    (strlen(args[1])));
            for (int i = 0; i < strlen(args[1]) - 1; i++) {
                filename[i] = args[1][i];
            }
            filename[strlen(args[1]) - 1] = '\0';
            if(!save(filename, board, tileCounter, *currentPlayer)) {
                fprintf(stderr, "Unable to save game\n");
            }
            free(filename);
        }
        for (int i = 0; i < MAX_INPUT; i++) {
            free(args[i]);
        }
        free(args);
    }
    if (humanPlayed) {
        for (int i = 0; i < MAX_INPUT; i++) {
            free(args[i]);
        }
        free(args);   
    }
}


/** 
* Loads or generates a game board/
* row: The amount of rows on the game board.
* col: The amount of columns on the game board.
* file: The saved file to load.
* tileCounter: The tile counter to restore.
* currentPlayer: The current player to resture.
*/
Board load_board(int row, int col, char *file, int *tileCounter,
    int *currentPlayer) {
    Board board;
    if (file != NULL) { /* Load a saved game. */
        board = load(file, *&tileCounter, *&currentPlayer);
    } else {
        board.grid = generate_board(row, col);
        board.row = row;
        board.col = col;
    }
    return board;
}

/** 
* Initializes a game with a set of paramaters.
* tiles: The tileset to cycle through.
* tileAmount: The amount of tiles loaded.
* row: The amount of rows on the game board.
* col: The amount of columns on the game board.
* file: The saved file to load.
* p1Type: Player 1 type.
* p2Type: Player 2 type.
*/
void play_game(char ***tiles, int tileAmount, int row, int col, char *file,
        char *p1Type, char *p2Type) {
    int tileCounter = 0, currentPlayer = 1;
    Board board = load_board(row, col, file, &tileCounter, &currentPlayer);
    /* Initialize variables to track recent plays. */
    int p1RecentR = -3, p1RecentC = -3, p2RecentR = -3, p2RecentC = -3;
    /* Loop which keeps the game playing, game is over on exit. */
    while (1) {
        draw_board(board);
        if (tileCounter == tileAmount) {
            tileCounter = 0;
        }
        if (game_is_over(board, tiles[tileCounter])) {
            if (currentPlayer) {
                printf("Player # wins\n");
            } else {
                printf("Player * wins\n");
            }
            break;
        }
        int angle;
        if (((strcmp(p1Type, "1") == 0 || strcmp(p1Type, "2") == 0) &&
                currentPlayer) || ((strcmp(p2Type, "1") == 0 ||
                strcmp(p2Type, "2") == 0) && !currentPlayer)) {
            computer_play(board, tiles[tileCounter], p1Type, p2Type,
            &currentPlayer, &angle, &p1RecentR, &p1RecentC,
            &p2RecentR, &p2RecentC);
        } else {
            display_tile(tiles[tileCounter]);
            human_play(board, tiles[tileCounter], tileCounter, &currentPlayer,
                    &p1RecentR, &p1RecentC);
        }
        tileCounter++;
    }
    for (int i = 0; i < board.col; i++) {
        free(board.grid[i]);
    }
    free(board.grid);
}

/* Displays the board in to stdout. */
void draw_board(Board board) {
    for (int i = 0; i < board.col; i++) {
        for (int j = 0; j < board.row; j++) {
            printf("%c", board.grid[i][j]);
        }
        printf("\n");
    }
}

/**
* Validates an array of arguments for errors.
* Returns 1 if arguments are valid, 0 if there is a error or 2 if save has
* invalid arguments.
*/
int validate_input(char **args) {
    int argLength = 0;
    while (args[argLength][strlen(args[argLength]) - 1] != '\n') {
        argLength++;
    }
    // Check for 3 or 2 arguments
    if (argLength + 1 == 2 && strcmp(args[0], "save") == 0) {
        return 2;
    } else if ((argLength + 1) != 3) {
        return 0;
    }
    // Ensure inputs are digits
    for (int i = 0; i <= argLength; i++) {
        if (args[i][0] == '-') {
            if (!isdigit(args[i][1])) {
                return 0;
            }
        } else if (!isdigit(*args[i])) {
            return 0;
        }
    }
    // Ensure third argument is a valid angle
    if (!(atoi(args[2]) == 0 || atoi(args[2]) == 90 ||
            atoi(args[2]) == 180 || atoi(args[2]) == 270)) {
        return 0;   
    }
    return 1;
}

/**
* Splits a string in to an array of substrings.
* string: The string to split.
* character: The seperator to use.
* Returns the array of substrings split according to the seperator.
*/
char **split(char *string, char *character) {
    char *word = strtok(string, character);
    char **array = malloc(sizeof(char *) * MAX_INPUT);
    for (int i = 0; i < MAX_INPUT; i++) {
        array[i] = malloc(sizeof(char) * MAX_INPUT);
    }
    int counter = 0;
    while (word != NULL) {
        strcpy(array[counter], word);
        word = strtok(NULL, character);
        counter++;
    }
    return array;
}

/**
* Splits a string in to an array of substrings, but a size value can be
* obtained from this function.
* string: The string to split.
* character: The seperator to use.
* size: The size of the array of substrings.
* Returns the array of substrings split according to the seperator.
*/
char **split_with_size(char *string, char *character, int *size) {
    char *word = strtok(string, character);
    char **array = malloc(sizeof(char *) * MAX_INPUT);
    for (int i = 0; i < MAX_INPUT; i++) {
        array[i] = malloc(sizeof(char) * MAX_INPUT);
    }
    int counter = 0;
    while (word != NULL) {
        strcpy(array[counter], word);
        word = strtok(NULL, character);
        counter++;
    }
    *size = counter;
    return array;
}

/**
* Generates an n by m array representing the game board.
* row: The amount of rows in the board.
* col: The amount of columns in the board.
* Returns an n by m array representing the game board.
*/
char **generate_board(int row, int col) {
    char **grid = malloc(sizeof(char *) * col);
    for (int i = 0; i < col; i++) {
        grid[i] = malloc(sizeof(char) * row);
        for (int j = 0; j < row; j++) {
            grid[i][j] = '.';
        }
    }
    return grid;
}

/**
* Extracts tiles from a array of chars.
* charArr: The array to extract the tiles from.
* size: Size of the char array.
* error: Errors which may arise from the function.
* Returns a 3D array of tiles.
*/
char ***extract_tiles(char *charArr, int size, int *error) {
    char ***tiles = malloc(sizeof(char **) * (size / 25));
    int index = 0, col = -1, row = 0;
    tiles[index] = malloc(sizeof(char *) * 5);
    for (int i = 0; i < 5; i++) {
        tiles[index][i] = malloc(sizeof(char) * 5);
    }
    for (int i = 0; i < size; i++) {
        if (charArr[i] != '\n' && charArr[i] != ','
                && charArr[i] != '!') {
            *error = 3;
            return tiles;
        }
        if (i % 5 == 0 && i != 0) {
            col = -1;
            row++;
        }
        if (i % 25 == 0 && i != 0) {
            row = 0;
            index++;
            tiles[index] = malloc(sizeof(char *) * 5);
            for (int i = 0; i < 5; i++) {
                tiles[index][i] = malloc(sizeof(char) * 5);
            }
        }
        col++;
        tiles[index][row][col] = charArr[i];
    }
    return tiles;
}

/**
* Reads characters from a file to generate game tiles.
* fileName: The file to read from.
* tileAmount: The amount of tiles extracted.
* error: Errors which may arise from the function.
* Returns a 3D array of tiles.
*/
char ***read_tile(char *filename, int *tileAmount, int *error) {
    FILE *file = fopen(filename, "r");    
    char *content = malloc(0), ***tiles = NULL;
    int charCount = 0;
    if (file) {
        char character;
        while ((character = getc(file)) != EOF) {
            content = realloc(content, sizeof(char) * (charCount + 1));
            content[charCount] = character;
            charCount++;
        }
        int size = 0;
        char *contentSymbol = malloc(0);
        for (int i = 0; i < charCount; i++) {
            if (content[i] != '\n') {
                contentSymbol = realloc(contentSymbol,
                        sizeof(char) * (size + 1));
                contentSymbol[size] = content[i];
                size++;
            }
        }
        /* Amount of chars should be divisible by 25 */
        if (size % 25 != 0) {
            *error = 3;
            free(content);
            return tiles;
        }
        *tileAmount = size / 25;
        tiles = extract_tiles(contentSymbol, size, error);
        free(contentSymbol);
        free(content);
    } else {
        *error = 2;
        return tiles;
        fclose(file);
    }
    return tiles;
}

/**
* Swaps two char elements.
* elemA: The first element to swap.
* elemB: The second element to swap.
*/
void swap(char *elemA, char *elemB) {
    char temp;
    temp = *elemB;
    *elemB = *elemA;
    *elemA = temp;
}

/**
* Takes the transpose of a 5 x 5 matrix.
* matrix: The matrix to perform the operation on. 
*/
void transpose(char **matrix) {
    for (int i = 0; i < 5; i++) {
        for (int j = i; j < 5; j++) {
            swap(&matrix[i][j], &matrix[j][i]);
        }
    }
}

/**
* Reverses the column of a 5 x 5 matrix.
* matrix: The matrix to perform the operation on. 
*/
void reverse(char **matrix) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            swap(&matrix[i][j], &matrix[i][4 - j]);
        }
    }
}

/**
* Rotates a tile matrix by n steps
* tile: The tile matrix to rotate.
* step: The amount of rotation to perform.
*/
void rotate_tile(char **tile, int step) {
    for (int i = 0; i < step; i++) {
        transpose(tile);
        reverse(tile);
    }
}

/**
* Displays the set of tiles along with its rotation to stdout.
* tile: The tile to display.
*/
void display_tileset(char **tile) {
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 4; i++) {
            for (int k = 0; k < 5; k++) {
                printf("%c", tile[j][k]);
            }
            if (i != 3) {
                printf(" ");
            }
            rotate_tile(tile, 1);
        }
        printf("\n");
    }
}

/**
* Displays a single tile to stdout.
* tile: The tile to display.
*/
void display_tile(char **tile) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%c", tile[i][j]);
        }
        printf("\n");
    }
}

/**
* Checks if a integer pair exists in an array.
* x: x value of the pair.
* y: y value of the pair.
* array: The array to check.
* size: The size of the array.
* Returns true if the pair exists in the array.
*/
int pair_in_array(int x, int y, int array[25][2], int size) {
    for (int i = 0; i < size; i++) {
        if (array[i][0] == y && array[i][1] == x) {
            return 1;
        }
    }
    return 0;
}

/**
* Verifies points between a tile and a board to check whether if the
* configuration allows placement of the tile on to the game board.
* board: The board to place the tile on.
* tile: The tile to place.
* x: x coordinate of the board to place the tile.
* y: y coordinate of the board to place the tile.
* xMin: Smallest x value of the tile with respect to the board.
* yMin: Smallest y value of the tile with respect to the board.
* xMax: Largest x value of the tile with respect to the board.
* yMax: Largest y value of the tile with respect to the board.
* Returns 0 if tile placement is invalid, 1 otherwise.
*/
int verify_tile_point(Board board, char **tile, int x, int y, int xMin,
        int yMin, int xMax, int yMax) {
    int tilePoints[25][2], tilePointsOnBoard[25][2], counter = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            tilePoints[counter][0] = i;
            tilePoints[counter][1] = j;
            counter++;
        }
    }
    counter = 0;
    /** 
    * Checks if points on board already have tiles which this placement will
    * interfere with.
    */
    for (int i = yMin; i <= yMax; i++) {
        for (int j = xMin; j <= xMax; j++) {
            tilePointsOnBoard[counter][0] = i - (y - 2);
            tilePointsOnBoard[counter][1] = j - (x - 2);
            counter++;
            if (board.grid[i][j] != '.' &&
                    tile[i - (y - 2)][j - (x - 2)] != ',') {
                return 0;
            }
        }
    }
    /* Gets all the tile points not on the game board. */
    int tilePointsNotOnBoard[25][2], arraySize = 0;
    for (int i = 0; i < 25; i++) {
        if (!pair_in_array(tilePoints[i][1], tilePoints[i][0],
                tilePointsOnBoard, counter)) {
            tilePointsNotOnBoard[arraySize][0] = tilePoints[i][0];
            tilePointsNotOnBoard[arraySize][1] = tilePoints[i][1];
            arraySize++;
        }
    }
    /** 
    * Checks tile points which are not on the board that might contain am
    * tile piece.
    */
    for (int i = 0; i < arraySize; i++) {
        int col = tilePointsNotOnBoard[i][0];
        int row = tilePointsNotOnBoard[i][1];
        if (tile[col][row] == '!') {
            return 0;
        }
    }
    return 1;
}

/**
* Attempt to place a tile on a game board.
* board: The board to place the tile on.
* tile: The tile to place.
* x: x coordinate of the board to place the tile.
* y: y coordinate of the board to place the tile.
* angle: The angle to place the tile.
* player: The player which is attempting to place the tile.
* test: Whether if the tile should be placed on to the board or to test
* if the tile can be placed.
* Returns 1 if tile can be or has been successfully placed, 0 otherwise.
*/
int place_tile(Board board, char **tile, int x, int y, int angle, int player,
        int test) {
    int rotation = 4;
    switch(angle) {
        case 90:
            rotate_tile(tile, 1);
            rotation = 1;
            break;
        case 180:
            rotate_tile(tile, 2);
            rotation = 2;
            break;
        case 270:
            rotate_tile(tile, 3);
            rotation = 3;
            break;
    }
    int yMax = y + 2, yMin = y - 2, xMax = x + 2, xMin = x - 2;
    if (yMin < 0) {
        yMin = 0;
    }
    if (xMin < 0) {
        xMin = 0;
    }
    if (yMax > board.col - 1) {
        yMax = board.col - 1;
    }
    if (xMax > board.row - 1) {
        xMax = board.row - 1;
    }
    if (!verify_tile_point(board, tile, x, y, xMin, yMin, xMax, yMax)) {
        rotate_tile(tile, 4 - rotation);
        return 0;
    }
    if (!test) { /* Place the tile if the operation is not a test */
        for (int i = yMin; i <= yMax; i++) {
            for (int j = xMin; j <= xMax; j++) {
                if (tile[i - (y - 2)][j - (x - 2)] == '!') {
                    if (player) {
                        board.grid[i][j] = '*';
                    } else {
                        board.grid[i][j] = '#';
                    }
                }
            }
        }
    }
    rotate_tile(tile, 4 - rotation);
    return 1;
}

/**
* Checks if the game is over by checking if the current tile can be played
* anywhere on the board.
* board: The game board.
* tile: The current tile being played.
* Returns 0 if there are no points to place the tile, 1 otherwise.
*/
int game_is_over(Board board, char **tile) {
    for (int angle = 0; angle < 4; angle++) {
        for (int i = -2; i < board.col + 2; i++) {
            for (int j = -2; j < board.row + 2; j++) {
                if (place_tile(board, tile, i, j, angle * 90, 0, 1)) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

/**
* Saves the game by writing to a file.
* filename: The name of the file to write to.
* board: The game board.
* nextTile: The index of the next tile being played.
* nextPlayer: The next player to be playing.
* Returns 1 if saved successfully, 0 otherwise
*/
int save(char *filename, Board board, int nextTile, int nextPlayer) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        return 0;
    }
    fprintf(file, "%i %i %i %i\n", nextTile, nextPlayer, board.row, board.col);
    for (int i = 0; i < board.col; i++) {
        for (int j = 0; j < board.row; j++) {
            fprintf(file, "%c", board.grid[i][j]);
        }
        if (i != board.col - 1) {
            fprintf(file, "\n");
        }
    }
    fclose(file);
    return 1;
}

/**
* Generates a game board from a line.
* counter: The current value of the counter.
* line: The line to process.
* Returns a board struct to begin a game.
*/
Board get_first_line(int counter, char *line) {
    Board board;
    char **array;
    int size;
    line[counter] = '\0';
    array = split_with_size(line, " ", &size);
    free(line);
    if (size != 4) {
        check_err(7);
    }
    for (int i = 0; i < size; i++) {
        if (atoi(array[i]) == 0 && array[i][0] != '0') {
            check_err(7);
        }
    }
    board.row = atoi(array[2]);
    board.col = atoi(array[3]);
    board.grid = malloc(sizeof(char *) * (board.col + 1));
    for (int i = 0; i < board.col; i++) {
        board.grid[i] = malloc(sizeof(char) * (board.row + 1));
    }
    for (int i = 0; i < MAX_INPUT; i++) {
        free(array[i]);
    }
    free(array);
    return board;
}

/**
* Loads a game from a file.
* filename: The name of the file to load from.
* tileIndex: The index of the tile to be played.
* playerTurn: The next player to be playing.
* Returns 1 if saved successfully, 0 otherwise
*/
Board load(char *filename, int *tileIndex, int *playerTurn) {
    FILE *file = fopen(filename, "r");
    Board board;
    char character;
    int row = 0, col = 0, rowMax = 0, colMax = 0;
    if (file) {
        int counter = 0, isFirstLine = 1;
        char *line = malloc(0);
        while ((character = getc(file)) != EOF) {
            if (isFirstLine) { /* Read the first line. */
                line = realloc(line, sizeof(char) * (counter + 1));
                line[counter] = character;
                if (character == '\n') {
                    isFirstLine = 0;
                    board = get_first_line(counter, line);
                }
            } else { /* Check the args if the first line matches the board. */
                if (col == 0) {
                    rowMax++;
                }
                if (character == '\n') {
                    colMax++;
                }
                if (col == board.col || row == board.row + 1) {
                    check_err(7);
                }
                if (character != '.' && character != '#' && 
                        character != '*' && character != '\n') {
                    check_err(7);
                }
                board.grid[col][row] = character;
                row++;
                if (character == '\n') {
                    row = 0;
                    col++;
                }
            }
            counter++;
        }
        if ((rowMax - 1) != board.row || (colMax + 1) != board.col) {
            check_err(7);
        }
        fclose(file);
    } else {
        check_err(6);
    }
    return board;
}

/**
* Play a tile on the board with type 1. 
* board: The game board.
* tile: The tile being played.
* rStart: Recent legal play row.
* cStart: Recent legal play colulm.
* angle: The angle of the tile.
* player: The player playing this tile.
*/
void type1_play(Board board, char **tile, int *rStart, int *cStart,
        int *angle, int player) {
    *angle = 0;
    if (*rStart == -3) {
        *rStart = -2;
    }
    if (*cStart == -3) {
        *cStart = -2;
    }
    int r = *rStart;
    int c = *cStart;
    do {
        do {
            if (place_tile(board, tile, c, r, *angle, player, 1)) {
                place_tile(board, tile, c, r, *angle, player, 0);
                *rStart = r;
                *cStart = c;
                return;
            }
            c++;
            if (c > board.col + 2) {
                c = -2;
                r++;
            }
            if (r > board.row + 2) {
                r = -2;
            }
        } while (r != *rStart || c != *cStart); 
        *angle += 90;
    } while (*angle <= 270);
}

/**
* Play a tile on the board with type 1. 
* board: The game board.
* tile: The tile being played.
* rStart: Recent legal play row.
* cStart: Recent legal play colulm.
* angle: The angle of the tile.
* player: The player playing this tile.
*/
void type2_play(Board board, char **tile, int *rStart, int *cStart,
        int *angle, int player) {
    if (player) {
        if (*rStart == -3) {
            *rStart = -2;
        }
        if (*cStart == -3) {
            *cStart = -2;
        }
    } else {
        if (*rStart == -3) {
            *rStart = board.row + 2;
        }
        if (*cStart == -3) {
            *cStart = board.col + 2;
        }
    }
    int r = *rStart, c = *cStart;
    do {
        *angle = 0;
        do {
            if (place_tile(board, tile, c, r, *angle, player, 1)) {
                place_tile(board, tile, c, r, *angle, player, 0);
                *rStart = r;
                *cStart = c;
                return;
            }
            *angle += 90;
        } while (*angle <= 270);
        if (player) {
            c++;
            if (c > board.col + 2) {
                c = -2;
                r++;
            }
            if (r > board.row + 2) {
                r = -2;
            }
        } else {
            c--;
            if (c == -2) {
                c = board.col + 2;
                r--;
            }
            if (r == -2) {
                r = board.row + 2;
            }
        }
    } while (r != *rStart || c != *cStart);
}
