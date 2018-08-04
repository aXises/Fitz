#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Constants
#define MAX_INPUT 70

// Function prototypes.
void new_game();
void play_game(int row, int col);
void draw_board(char **board, int row, int col);
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

int main(int argc, char **argv) {
    if (argc == 1 || argc > 6) {
        fprintf(stderr, "Usage: fitz tilefile [p1type p2type " \
            "[height width | filename]]\n");
    }
    if (argc == 2) {
        int tileAmount, errorCode = 0;
        char ***tiles = read_tile(argv[1], &tileAmount, &errorCode);
        if (errorCode == 0) {
            for (int i = 0; i < tileAmount; i++) {
                display_tileset(tiles[i]);
                if (i != tileAmount - 1) {
                    printf("\n");
                } 
            }
        }
        return errorCode;
    }
    // new_game();
    return 0;
}

void new_game() {
    play_game(5, 4);
}

void play_game(int row, int col) {
    int gameEnded = 0;
    char **board = generate_board(row, col);
    // draw_board(board, row, col);
    int tileAmount, errorCode = 0;
    char ***tiles = read_tile("testtiles", &tileAmount, &errorCode);
    if (tiles == NULL) {
        printf("tilefile error %i\n", errorCode);
        exit(errorCode);
        return;
    }
    while (!gameEnded) {
        char response[MAX_INPUT];
        char **args;
        while (1) {
            fgets(response, sizeof(response), stdin);
            if (strlen(response) > MAX_INPUT) {
                continue;
            }
            args = split(response, " ");
            if (validate_input(args)) {
                break;
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
        printf("loop complete\n");
        gameEnded = 1;  
    }
    for (int i = 0; i < col; i++) {
        free(board[i]);
    }
    free(board);
    for (int i = 0; i < tileAmount; i++) {
        for (int j = 0; j < 5; j++) {
            free(tiles[i][j]);
        }
        free(tiles[i]);
    }
    free(tiles);
}

void draw_board(char **board, int row, int col) {
    for (int i = 0; i < col; i++) {
        for (int j = 0; j < row; j++) {
            printf("%c", board[i][j]);
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
    
}
