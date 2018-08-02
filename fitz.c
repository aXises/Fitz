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
char **read_tile(char *fileName);

int main(int argc, char **argv) {
    new_game();
    return 0;
}

void new_game() {
    play_game(5, 4);
}

void play_game(int row, int col) {
    int gameEnded = 0;
    char **board = generate_board(row, col);
    // draw_board(board, row, col);
    read_tile("testtiles");
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

char **read_tile(char *fileName) {
    FILE *file = fopen(fileName, "r");
    int character;
    int row = 0, col = 0;
    if (file) {
        while ((character = getc(file)) != EOF) {
            if (character != '\n' && character != ',' && character != '!') {
                return NULL;
            }
            if (row != 5 && row != 0 && character == '\n') {
                return NULL;
            }
            if (character == '\n') {
                if (row == 5) {
                    col = 0;
                }
                row = 0;
                col++;
            } else {
                row++;
            }
        }
        fclose(file);
    }
    return NULL;
}