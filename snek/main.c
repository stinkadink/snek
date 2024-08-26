#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>

typedef struct {
    int x;
    int y;
} Position;

int fieldSize, gameSpeed;

const Position directions[4] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};

void increaseCapacity(Position *, int *);

char chooseFieldType();
char **createNewField(int *, int *, char);
void updateField(char **, Position *, int, Position);

void createNewPoint(Position *, char **);

void delay(clock_t);

void draw(char **);

void move(Position *, int, int *);

int main() {
    restart:
    srand(time(NULL));

    char option = chooseFieldType();
    char **field = createNewField(&fieldSize, &gameSpeed, option);
    if (!field) {
        goto restart;
    }

    int snakeCapacity = 8;
    Position *snake = malloc(snakeCapacity * sizeof(Position));
    for (int i = 0; i < fieldSize; ++i) {
        for (int j = 0; j < fieldSize; ++j) {
            if (field[i][j] == '0') {
                snake[0].x = i;
                snake[0].y = j;
            }
        }
    }
    snake[1] = snake[0];
    int snakeSize = 1;

    bool gameOver = false;
    bool pointExists = false;
    int movement = -1;
    int score = 0;
    Position point = {-1, -1};
    while (!gameOver) {
        if (!pointExists) {
            createNewPoint(&point, field);
            pointExists = true;
        }

        if (movement == -1) {
            draw(field);
            printf("Control the snake with WASD!\n");
            while (movement == -1) {
                move(snake, snakeSize, &movement);
            }
            if ((field[snake[0].x][snake[0].y] == '#') || (field[snake[0].x][snake[0].y] == 'O')) {
                gameOver = true;
                break;
            }
            updateField(field, snake, snakeSize, point);
            draw(field);
            delay(gameSpeed);
        }

        move(snake, snakeSize, &movement);

        if ((field[snake[0].x][snake[0].y] == '#') || (field[snake[0].x][snake[0].y] == 'O')) {
            gameOver = true;
        }

        updateField(field, snake, snakeSize, point);
        draw(field);

        if ((snake[0].x == point.x) && (snake[0].y == point.y)) {
            pointExists = false;
            score = snakeSize++;
            if ((snakeSize + 1) == snakeCapacity) {
                increaseCapacity(snake, &snakeCapacity);
            }
        }

        delay(gameSpeed);
    }

    printf("Game over!\n"
           "Score: %d", score);

    for (int i = 0; i < fieldSize; ++i) {
        free(field[i]);
    }
    free(field);
    free(snake);

    delay(500);

    printf("\nPress R to play again!");
    if (kbhit()) {
        do {
            getch();
        } while (kbhit());
    }
    char key;
    while (1) {
        if (kbhit()) {
            key = getch();
        } else continue;
        if (tolower(key) == 'r') {
            goto restart;
        } else break;
    }

    return 0;
}

void increaseCapacity(Position *snake, int *size) {
    *size += 8;
    snake = realloc(snake, *size * sizeof(Position));
    if (!snake) {
        printf("This fucking snake doesn't fit in the computer anymore"
               "\nPlease install more RAM!");
        exit(-1);
    }
}

char chooseFieldType() {
    system("cls");
    printf("Snek!"
           "\n1. Default map"
           "\n2. Custom map");
    while (!kbhit()) {
        ;
    }
    char option = getch();
    if (option == '1' || option == '2') {
        return option;
    } else return chooseFieldType();
}

char **createNewField(int *size, int *speed, char option) {
        if (option == '1') {
            *size = 22;
            char **field = malloc((*size) * sizeof(char *));
            for (int i = 0; i < *size; ++i) {
                field[i] = malloc(*size * sizeof(char));
            }
            *speed = 10;

            for (int j = 0; j < *size; ++j) {
                field[0][j] = '#';
            }
            for (int i = 1; i < *size; ++i) {
                field[i][*size - 1] = '#';
            }
            for (int j = *size - 2; j >= 0; --j) {
                field[*size - 1][j] = '#';
            }
            for (int i = *size - 2; i >= 0; --i) {
                field[i][0] = '#';
            }

            for (int i = 1; i < *size - 1; ++i) {
                for (int j = 1; j < *size - 1; ++j) {
                    field[i][j] = ' ';
                }
            }

            field[((*size) - 1) / 2][((*size) - 1) / 2] = '0';
            return field;
        }

        if (option == '2') {
            system("cls");
            char fileName[50];
            printf("Enter the name of the file: ");
            scanf("%s", fileName);

            char filePath[200] = "../fields/";
            strcat(filePath, fileName);

            if (!freopen(filePath, "r", stdin)) {
                printf("File not found.\n");
                getch();
                return NULL;
            }
            scanf("Field Size: %d\n", size);
            scanf("Game Speed: %d\n", speed);
            char **field = malloc(*size * sizeof(char *));
            for (int i = 0; i < *size; ++i) {
                field[i] = malloc(*size * sizeof(char));
            }

            for (int i = 0; i < *size; ++i) {
                for (int j = 0; j < *size; ++j) {
                    scanf("%c", &field[i][j]);
                    getchar();
                }
            }

            freopen("CON", "r", stdin);
            return field;
        }
}

void updateField(char **field, Position *snake, int size, Position point) {
    if (size > 1) {
        field[snake[1].x][snake[1].y] = 'O';
    }
    field[snake[size].x][snake[size].y] = ' ';
    field[snake[0].x][snake[0].y] = '0';
    field[point.x][point.y] = 'X';
}

void createNewPoint(Position *point, char **field) {

    // Attempts to create a new random point until it's not placed on the snake
    do {
        point->x = (rand() % (fieldSize - 2)) + 1;
        point->y = (rand() % (fieldSize - 2)) + 1;
    } while ((field[point->x][point->y] == '0') || (field[point->x][point->y] == '#'));
}

void move(Position *snake, int snakeSize, int *direction) {

    char key;

    // snake is at an angle of direction * 90 degrees; -1 = no movement
    Position next = {0, 0};
    if (kbhit()) {
        key = getch();
    } else key = ' ';
    switch (tolower(key)) {
        case 'w':
            if (*direction != 3) {
                next = directions[1];
                *direction = 1;
            } else {
                next = directions[*direction];
                *direction = 3;
            }
            break;
        case 'a':
            if (*direction != 0) {
                next = directions[2];
                *direction = 2;
            } else {
                next = directions[*direction];
                *direction = 0;
            }
            break;
        case 's':
            if (*direction != 1) {
                next = directions[3];
                *direction = 3;
            } else {
                next = directions[*direction];
                *direction = 1;
            }
            break;
        case 'd':
            if (*direction != 2) {
                next = directions[0];
                *direction = 0;
            } else {
                next = directions[*direction];
                *direction = 2;
            }
            break;
        case 'p':
            *direction = -1;
        default:
            if (*direction != -1) {
                next = directions[*direction];
            }
    }
    if (*direction != -1) {
        next.x += snake[0].x;
        next.y += snake[0].y;
        for (int i = snakeSize; i >= 0; --i) {
            snake[i] = snake[i - 1];
        }
        snake[0] = next;
    }
}

void draw(char **field) {
    system("cls");
    for (int i = 0; i < fieldSize; ++i) {
        for (int j = 0; j < fieldSize; ++j) {
            printf("%c ", field[i][j]);
        }
        printf("\n");
    }
}

void delay(clock_t seconds) {
    int startTime = clock();
    while (clock() < startTime + seconds) {
        ;
    }
}