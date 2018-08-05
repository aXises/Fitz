#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Constants
#define MAX_INPUT 70

// Type definations.
typedef struct {
    char **grid;
    int row;
    int col;
} Board;

// Function prototypes.
void play_game(char ***tiles, int tileAmount, int row, int col);
void draw_board(Board board);
void request_input(char *response);
int validate_input(char **args);
char **split(char* string, char *character);
char **generate_board(int row, int col);
char ***read_tile(char *fileName, int *tileAmount, int *error);
void swap(char *elemA, char *elemB);
void transpose(char **tile);
void reverse(char **tile);
void rotate_tile(char **tile, int step);
void display_tileset(char **tile);
void display_tile(char **tile);
int pair_in_array(int x, int y, int array[25][2], int size);
int place_tile(Board board, char **tile, int x, int y, int angle, int test);
int game_is_over(Board board, char **tile);

int main(int argc, char **argv) {
    if (argc == 1 || argc > 6) {
        fprintf(stderr, "Usage: fitz tilefile [p1type p2type " \
            "[height width | filename]]\n");
        return 1;
    }
    int tileAmount;
    int errorCode = 0;
    char ***tiles = read_tile(argv[1], &tileAmount, &errorCode);
    if (errorCode == 0 && argc == 2) {
        for (int i = 0; i < tileAmount; i++) {
            display_tileset(tiles[i]);
            if (i != tileAmount - 1) {
                printf("\n");
            } 
        }
    } else if (errorCode > 0) {
        if (errorCode == 2) {
            fprintf(stderr, "Can’t access tile file\n");
        }
        if (errorCode == 3) {
            fprintf(stderr, "Invalid tile file contents\n");
        }
        return errorCode;
    }
    if (argc == 6) {
        if (!(strcmp(argv[2], "h") == 0 || strcmp(argv[2], "p") == 0)
                || !(strcmp(argv[3], "h") == 0 ||
                strcmp(argv[3], "p") == 0)) {
            fprintf(stderr, "Invalid player type\n");
            return 4;
        }
        if (!isdigit(*argv[4]) || !isdigit(*argv[5])) {
            fprintf(stderr, "Invalid dimensions\n");
            return 5;
        }
        if (atoi(argv[4]) < 1 || atoi(argv[4]) > 999 ||
                    atoi(argv[5]) < 1 || atoi(argv[5]) > 999 ) {
            fprintf(stderr, "Invalid dimensions\n");
            return 5;             
        }
        play_game(tiles, tileAmount, atoi(argv[5]), atoi(argv[4]));
    }
    for (int i = 0; i < tileAmount; i++) {
        for (int j = 0; j < 5; j++) {
            free(tiles[i][j]);
        }
        free(tiles[i]);
    }
    free(tiles);
    return 0;
}

void play_game(char ***tiles, int tileAmount, int row, int col) {
    Board board;
    board.grid = generate_board(row, col);
    board.row = row;
    board.col = col;
    int tileCounter = 0;
    while (1) {
        char response[MAX_INPUT];
        char **args;
        if (tileCounter == tileAmount) {
            tileCounter = 0;
        }
        draw_board(board);
        if(game_is_over(board, tiles[tileCounter])) {
            printf("game over\n");
            break;
        }
        printf("\n");
        display_tile(tiles[tileCounter]);
        printf("Player *] ");
        while (1) {
            fgets(response, sizeof(response), stdin);
            if (strlen(response) > MAX_INPUT) {
                continue;
            }
            args = split(response, " ");
            if (validate_input(args)) {
                if (place_tile(board, tiles[tileCounter], atoi(args[1]),
                        atoi(args[0]), atoi(args[2]), 0)) {
                    tileCounter++;
                    break;
                }
            } else {
                for (int i = 0; i < MAX_INPUT; i++) {
                    free(args[i]);
                }
                free(args);
            }
        }
        for (int i = 0; i < MAX_INPUT; i++) {
            free(args[i]);
        }
        free(args);
    }
    for (int i = 0; i < col; i++) {
        free(board.grid[i]);
    }
    free(board.grid);
}

void draw_board(Board board) {
    for (int i = 0; i < board.col; i++) {
        for (int j = 0; j < board.row; j++) {
            printf("%c", board.grid[i][j]);
        }
        printf("\n");
    }
}

void request_input(char *response) {
    fgets(response, sizeof(response), stdin);
}

int validate_input(char **args) {
    int argLength = 0;
    while (args[argLength][strlen(args[argLength]) - 1] != '\n') {
        argLength++;
    }
    // Check for 3 arguments
    if ((argLength + 1) != 3) {
        return 0;
    }
    // Ensure inputs are digits
    for (int i = 0; i <= (argLength); i++) {
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

char ***read_tile(char *fileName, int *tileAmount, int *error) {
    FILE *file = fopen(fileName, "r");
    char character;
    int row = 0, col = 0, size = 0;
    char ***tiles = malloc(sizeof(char **));
    tiles[size] = malloc(sizeof(char *) * 5);
    for (int i = 0; i < 5; i++) {
        tiles[size][i] = malloc(sizeof(char) * 5);
    }   
    if (file) {
        while ((character = getc(file)) != EOF) {
            if (character != '\n' && character != ','
                    && character != '!') {
                *error = 3;
                return NULL;
            }
            if (((row == 5 && col == 4) || (row == 0 && col == -1))
                    && character != '\n') {
                *error = 3;
                return NULL;
            }
            if (row > 5) {
                *error = 3;
                return NULL;
            }
            if (character == '\n') {
                col++;
                if (col == 5) {
                    size++;
                    tiles = realloc(tiles,
                            sizeof(char **) * (size + 1));
                    tiles[size] = malloc(sizeof(char *) * 5);
                    for (int i = 0; i < 5; i++) {
                        tiles[size][i] = malloc(sizeof(char) * 5);
                    }  
                    col = -1;
                }
                row = 0;
            } else {
                tiles[size][col][row] = character;
                row++;
            }

        }
        fclose(file);
    } else {
        *error = 2;
        return NULL;
    }
    *tileAmount = size + 1;
    return tiles;
}

void swap(char *elemA, char *elemB) {
    char temp;
    temp = *elemB;
    *elemB = *elemA;
    *elemA = temp;
}

void transpose(char **tile) {
    for (int i = 0; i < 5; i++) {
        for (int j = i; j < 5; j++) {
            swap(&tile[i][j], &tile[j][i]);
        }
    }
}

void reverse(char **tile) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            swap(&tile[i][j], &tile[i][4 - j]);
        }
    }
}

void rotate_tile(char **tile, int step) {
    for (int i = 0; i < step; i++) {
        transpose(tile);
        reverse(tile);
    }
}

void display_tileset(char **tile) {
    for (int j = 0; j < 5; j++) {
        for (int i = 0; i < 4; i++) {
            for (int k = 0; k < 5; k++) {
                printf("%c", tile[j][k]);
            }
            printf(" ");
            rotate_tile(tile, 1);
        }
        printf("\n");
    }
}

void display_tile(char **tile) {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            printf("%c", tile[i][j]);
        }
        printf("\n");
    }
}

int pair_in_array(int x, int y, int array[25][2], int size) {
    for (int i = 0; i < size; i++) {
        if (array[i][0] == y && array[i][1] == x) {
            return 1;
        }
    }
    return 0;
}

int place_tile(Board board, char **tile, int x, int y, int angle, int test) {
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
    // for (int i = 0; i < 5; i++) {
    //     for (int j = 0; j < 5; j++) {
    //         printf("(%i %i %c)", i, j, tile[i][j]);
    //     }
    //     printf("\n");
    // }
    int yMax = y + 2;
    int yMin = y - 2;
    int xMax = x + 2;
    int xMin = x - 2;
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
    int tilePoints[25][2];
    int counter = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            tilePoints[counter][0] = i;
            tilePoints[counter][1] = j;
            counter++;
        }
    }
    int tilePointsOnBoard[25][2];
    counter = 0;
    for (int i = yMin; i <= yMax; i++) {
        for (int j = xMin; j <= xMax; j++) {
            tilePointsOnBoard[counter][0] = i - (y - 2);
            tilePointsOnBoard[counter][1] = j - (x - 2);
            counter++;
            if (board.grid[i][j] != '.' &&
                    tile[i - (y - 2)][j - (x - 2)] != ',') {
                rotate_tile(tile, 4 - rotation);
                return 0;
            }
        }
    }
    int tilePointsNotOnBoard[25][2];
    int arraySize = 0;
    for (int i = 0; i < 25; i++) {
        if (!pair_in_array(tilePoints[i][1], tilePoints[i][0],
                tilePointsOnBoard, counter)) {
            tilePointsNotOnBoard[arraySize][0] = tilePoints[i][0];
            tilePointsNotOnBoard[arraySize][1] = tilePoints[i][1];
            arraySize++;
        }
    }
    for (int i = 0; i < arraySize; i++) {
        int col = tilePointsNotOnBoard[i][0];
        int row = tilePointsNotOnBoard[i][1];
        if (tile[col][row] == '!') {
            rotate_tile(tile, 4 - rotation);
            return 0;
        }
    }
    if (!test) {
        for (int i = yMin; i <= yMax; i++) {
            for (int j = xMin; j <= xMax; j++) {
                if (tile[i - (y - 2)][j - (x - 2)] == '!') {
                    board.grid[i][j] = '#';
                }
            }
        }
    }
    rotate_tile(tile, 4 - rotation);
    return 1;
}

int game_is_over(Board board, char **tile) {
    for (int angle = 0; angle < 4; angle++) {
        for (int i = -2; i < board.col + 1; i++) {
            for (int j = -2; j < board.row + 1; j++) {
                int m = place_tile(board, tile, i, j, angle * 90, 1);
                if (m) {
                    return 0;
                }
            }
        }
    }
    return 1;
}
