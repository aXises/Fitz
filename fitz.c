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
void free_tiles(char ***tiles, int tileAmount);
void play_game(char ***tiles, int tileAmount, int row, int col, char *file,
        char *p1Type, char *p2Type);
void draw_board(Board board);
void request_input(char *response);
int validate_input(char **args);
char **split(char *string, char *character);
char **generate_board(int row, int col);
char ***read_tile(char *filename, int *tileAmount, int *error);
void swap(char *elemA, char *elemB);
void transpose(char **tile);
void reverse(char **tile);
void rotate_tile(char **tile, int step);
void display_tileset(char **tile);
void display_tile(char **tile);
int pair_in_array(int x, int y, int array[25][2], int size);
int place_tile(Board board, char **tile, int x, int y, int angle, int player,
        int test);
int game_is_over(Board board, char **tile);
int save(char *filename, Board board, int nextTile, int nextPlayer);
Board load(char *filename, int *tileIndex, int *playerTurn, int *errorCode);
void type1_play(Board board, char **tile, int *rStart, int *cStart,
        int *angle, int player);
void type2_play(Board board, char **tile, int *rStart, int *cStart,
        int *angle, int player);


int main(int argc, char **argv) {
    if (argc == 1 || argc == 3 || argc == 4 || argc > 6) {
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
    if (argc == 5 || argc == 6) {
        if (!(strcmp(argv[2], "h") == 0 || strcmp(argv[2], "1") == 0 ||
                strcmp(argv[2], "2") == 0) || !(strcmp(argv[3], "h") == 0 ||
                strcmp(argv[3], "1") == 0 || strcmp(argv[3], "2") == 0)) {
            fprintf(stderr, "Invalid player type\n");
            free_tiles(tiles, tileAmount);
            return 4;
        }
        if (argc == 6) {
            if (!isdigit(*argv[4]) || !isdigit(*argv[5])) {
                fprintf(stderr, "Invalid dimensions\n");
                free_tiles(tiles, tileAmount);
                return 5;
            }
            if (atoi(argv[4]) < 1 || atoi(argv[4]) > 999 ||
                    atoi(argv[5]) < 1 || atoi(argv[5]) > 999) {
                fprintf(stderr, "Invalid dimensions\n");
                free_tiles(tiles, tileAmount);
                return 5;             
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

void free_tiles(char ***tiles, int tileAmount) {
    for (int i = 0; i < tileAmount; i++) {
        for (int j = 0; j < 5; j++) {
            free(tiles[i][j]);
        }
        free(tiles[i]);
    }
    free(tiles);
}

void play_game(char ***tiles, int tileAmount, int row, int col, char *file,
        char *p1Type, char *p2Type) {
    Board board;
    int tileCounter = 0;
    int currentPlayer = 1;
    if (file != NULL) {
        int errorCode;
        board = load(file, &tileCounter, &currentPlayer, &errorCode);
        if (tileCounter >= tileAmount) {
            fprintf(stderr, "Invalid save file contents\n");
            exit(7);
        }
        if (errorCode == 1) {
            fprintf(stderr, "Invalid save file contents\n");
            exit(7);
        } else if (errorCode == 2) {
            fprintf(stderr, "Can’t access save file\n");
            exit(6);
        }
    } else {
        board.grid = generate_board(row, col);
        board.row = row;
        board.col = col;
        tileCounter = 0;
        currentPlayer = 1;
    }
    int p1RecentR = NULL;
    int p1RecentC = NULL;
    int p2RecentR = NULL;
    int p2RecentC = NULL;
    while (1) {
        draw_board(board);
        char response[MAX_INPUT];
        char **args;
        if (tileCounter == tileAmount) {
            tileCounter = 0;
        }
        if(game_is_over(board, tiles[tileCounter])) {
            if (currentPlayer) {
                printf("Player # wins\n");
            } else {
                printf("Player * wins\n");
            }
            break;
        }

        int noArgs = 0;
        if ((strcmp(p1Type, "1") == 0 || strcmp(p1Type, "2") == 0) &&
                currentPlayer) {
            int angle;
            if (strcmp(p1Type, "1") == 0) {
                type1_play(board, tiles[tileCounter], &p1RecentR, &p1RecentC,
                        &angle, currentPlayer);
            } else {
                type2_play(board, tiles[tileCounter], &p2RecentR, &p2RecentC,
                        &angle, currentPlayer);
                p1RecentR = p2RecentR;
                p1RecentC = p2RecentC;
            }
            if (currentPlayer) {
                printf("Player * => %i %i rotated %i\n", p1RecentR,
                        p1RecentC, angle);
            } else {
                printf("Player # => %i %i rotated %i\n", p1RecentR,
                        p1RecentC, angle);   
            }
            currentPlayer = !currentPlayer;
            noArgs = 1;
        } else if ((strcmp(p2Type, "1") == 0 || strcmp(p2Type, "2") == 0) &&
                !currentPlayer) {
            int angle;
            if (strcmp(p2Type, "1") == 0) {
                type1_play(board, tiles[tileCounter], &p1RecentR, &p1RecentC,
                        &angle, currentPlayer);
            } else {
                type2_play(board, tiles[tileCounter], &p2RecentR, &p2RecentC,
                        &angle, currentPlayer);
                p1RecentR = p2RecentR;
                p1RecentC = p2RecentC;
            }
            if (currentPlayer) {
                printf("Player * => %i %i rotated %i\n", p1RecentR,
                        p1RecentC, angle);
            } else {
                printf("Player # => %i %i rotated %i\n", p1RecentR,
                        p1RecentC, angle); 
            }
            currentPlayer = !currentPlayer;
            noArgs = 1;
        } else {
            display_tile(tiles[tileCounter]);
            while (1) {
                if (currentPlayer) {
                    printf("Player *] ");    
    
                } else {
                    printf("Player #] ");
                }
                fgets(response, sizeof(response), stdin);
                if (strlen(response) > MAX_INPUT) {
                    continue;
                }
                args = split(response, " ");
                noArgs = 0;
                int status = validate_input(args);
                if (status == 1) {
                    if (place_tile(board, tiles[tileCounter], atoi(args[1]),
                            atoi(args[0]), atoi(args[2]), currentPlayer, 0)) {
                        p1RecentR = atoi(args[0]);
                        p1RecentC = atoi(args[1]);
                        currentPlayer = !currentPlayer;
                        break;
                    }
                } else if (status == 2) {
                    char *filename = malloc(sizeof(char) *
                            (strlen(args[1])));
                    for (int i = 0; i < strlen(args[1]) - 1; i++) {
                        filename[i] = args[1][i];
                    }
                    filename[strlen(args[1]) - 1] = '\0';
                    if(!save(filename, board, tileCounter, currentPlayer)) {
                        fprintf(stderr, "Unable to save game\n");
                    }
                    free(filename);
                }
                for (int i = 0; i < MAX_INPUT; i++) {
                    free(args[i]);
                }
                free(args);   
            }
            if (!noArgs) {
                for (int i = 0; i < MAX_INPUT; i++) {
                    free(args[i]);
                }
                free(args);   
            }
        }
        tileCounter++;
    }
    for (int i = 0; i < board.col; i++) {
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

char ***read_tile(char *filename, int *tileAmount, int *error) {
    FILE *file = fopen(filename, "r");    
    char *content = malloc(0);
    char ***tiles = NULL;
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
                contentSymbol = realloc(contentSymbol, sizeof(char) * (size + 1));
                contentSymbol[size] = content[i];
                size++;
            }
        }
        if (size % 25 != 0) {
                *error = 3;
                free(content);
                return tiles;
        }
        tiles = malloc(sizeof(char **) * (size / 25));
        *tileAmount = size / 25;
        int index = 0, col = -1, row = 0;
        tiles[index] = malloc(sizeof(char *) * 5);
        for (int i = 0; i < 5; i++) {
            tiles[index][i] = malloc(sizeof(char) * 5);
        }
        for (int i = 0; i < size; i++) {
            if (content[i] != '\n' && content[i] != ','
                    && content[i] != '!') {
                *error = 3;
                free(content);
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
            tiles[index][row][col] = contentSymbol[i];
        }
        free(contentSymbol);
        free(content);
    } else {
        *error = 2;
        return tiles;
    }
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

int game_is_over(Board board, char **tile) {
    for (int angle = 0; angle < 4; angle++) {
        for (int i = -2; i < board.col + 1; i++) {
            for (int j = -2; j < board.row + 1; j++) {
                if (place_tile(board, tile, i, j, angle * 90, 0, 1)) {
                    return 0;
                }
            }
        }
    }
    return 1;
}

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

Board load(char *filename, int *tileIndex, int *playerTurn, int *errorCode) {
    FILE *file = fopen(filename, "r");
    Board board;
    *errorCode = 0;
    char character;
    char **array;
    int size, row = 0, col = 0, rowMax = 0, colMax = 0;
    if (file) {
        int counter = 0;
        char *line = malloc(0);
        int isFirstLine = 1;
        while ((character = getc(file)) != EOF) {
            if (isFirstLine) {
                line = realloc(line, sizeof(char) * (counter + 1));
                line[counter] = character;
                if (character == '\n') {
                    line[counter] = '\0';
                    isFirstLine = 0;
                    array = split_with_size(line, " ", &size);
                    free(line);
                    if (size != 4) {
                        *errorCode = 1;
                        break;
                    }
                    for (int i = 0; i < size; i++) {
                        if (atoi(array[i]) == 0 && array[i][0] != '0') {
                            *errorCode = 1;
                            break;
                        }
                    }
                    *tileIndex = atoi(array[0]);
                    *playerTurn = atoi(array[1]);
                    board.row = atoi(array[2]);
                    board.col = atoi(array[3]);
                    board.grid = malloc(sizeof(char *) * (board.col + 1));
                    for (int i = 0; i < board.col; i++) {
                        board.grid[i] = malloc(sizeof(char) * (board.row + 1));
                    }
                }
            } else {
                if (col == 0) {
                    rowMax++;
                }
                if (character == '\n') {
                    colMax++;
                }
                if (col == board.col) {
                    *errorCode = 1;
                    break;
                }
                if (row == board.row + 1) {
                    *errorCode = 1;
                    break;
                }
                if (character != '.' && character != '#' && 
                        character != '*' && character != '\n') {
                    *errorCode = 1;
                    break;
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
            *errorCode = 1;
        }
        fclose(file);
    } else {
        *errorCode = 2;
        return board;
    }
    for (int i = 0; i < MAX_INPUT; i++) {
        free(array[i]);
    }
    free(array);
    return board;
}

void type1_play(Board board, char **tile, int *rStart, int *cStart,
        int *angle, int player) {
    *angle = 0;
    if (rStart == NULL) {
        *rStart = -2;
    }
    if (cStart == NULL) {
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

void type2_play(Board board, char **tile, int *rStart, int *cStart,
        int *angle, int player) {
    if (player) {
        if (rStart == NULL) {
            *rStart = -2;
        }
        if (cStart == NULL) {
            *cStart = -2;
        }
    } else {
        if (rStart == NULL) {
            *rStart = board.row + 2;
        }
        if (cStart == NULL) {
            *cStart = board.col + 2;
        }
    }
    int r = *rStart;
    int c = *cStart;
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
