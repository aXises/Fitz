#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Constants
#define MAX_INPUT 70

// Function prototypes.
void new_game();
void play_game();
void draw_board();
void request_input(char *response);
int validate_input(char **args);
char **split(char* string, char *character);

int main(int argc, char **argv) {
    new_game();
    return 0;
}

void new_game() {
    play_game();
}

void play_game() {
    int gameEnded = 0;
    draw_board();
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

}

void draw_board() {
    int height = 5;
    int width = 5;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf(".");
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