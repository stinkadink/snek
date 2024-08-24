#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>

typedef struct {
    int x;
    int y;
} Position;

const Position directions[4] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};

const int fieldSize = 16;
const int winLength = 200;
const float gameSpeed = 0.001f;

char **createNewField();
void updateField(char **, Position *, int, Position);

void createNewPoint(Position *, char **);

void delay(float);

void draw(char **);

void move(Position *, int, int *);

int main() {
    restart:
    srand(time(NULL));

    char **field = createNewField();

    Position *snake = (Position *)malloc((winLength + 1) * sizeof(Position));
    snake[0].x = (fieldSize - 1) / 2;
    snake[0].y = snake[0].x;
    snake[1] = snake[0];
    int snakeSize = 1;

    bool gameOver = false;
    bool win = false;
    bool pointExists = false;
    int movement = -1;
    int score = 0;
    Position point = {-1, -1};
    while (!gameOver) {
        if (!pointExists) {
            createNewPoint(&point, field);
            pointExists = true;
        }

        move(snake, snakeSize, &movement);

        if ((field[snake[0].x][snake[0].y] == '#') || (field[snake[0].x][snake[0].y] == 'O')) {
            gameOver = true;
        } else if (snakeSize >= winLength) {
            gameOver = true;
            win = true;
        }

        if (movement != -1) {
            updateField(field, snake, snakeSize, point);
            draw(field);
        }

        delay(gameSpeed);

        if (snake[0].x == point.x && snake[0].y == point.y) {
            pointExists = false;
            score = snakeSize++;
        }

    }

    if (win) {
        printf("You win!\n"
               "Score: %d", score);
    } else {
        printf("Game over!\n"
               "Score: %d", score);
    }

    for (int i = 0; i < fieldSize; ++i) {
        free(field[i]);
    }
    free(field);
    free(snake);

    delay(0.5f);

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

char **createNewField() {
    char **field = (char **)malloc(fieldSize * sizeof(char *));
    for (int i = 0; i < fieldSize; ++i) {
        field[i] = (char *)malloc(fieldSize * sizeof(char));
    }

    // Borders
    for (int j = 0; j < fieldSize; ++j) {
        field[0][j] = '#';
    }
    for (int i = 1; i < fieldSize; ++i) {
        field[i][fieldSize - 1] = '#';
    }
    for (int j = fieldSize - 1; j >= 0; --j) {
        field[fieldSize - 1][j] = '#';
    }
    for (int i = fieldSize - 1; i > 0; --i) {
        field[i][0] = '#';
    }

    // Within the borders
    for (int i = 1; i < fieldSize - 1; ++i) {
        for (int j = 1; j < fieldSize - 1; ++j) {
            field[i][j] = ' ';
        }
    }

    field[(fieldSize - 1) / 2][(fieldSize - 1) / 2] = '0';

    return field;
}

void updateField(char **field, Position *snake, int size, Position point) {
    field[snake[0].x][snake[0].y] = '0';
    if (size > 1) {
        field[snake[1].x][snake[1].y] = 'O';
    }
    field[snake[size].x][snake[size].y] = ' ';
    field[point.x][point.y] = 'X';
}

void createNewPoint(Position *point, char **field) {

    // Attempts to create a new random point until it's not placed on the snake
    do {
        point->x = (rand() % (fieldSize - 2)) + 1;
        point->y = (rand() % (fieldSize - 2)) + 1;
    } while (field[point->x][point->y] == '0');
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
        default:
            if (*direction != -1) {
                next = directions[*direction];
            }
    }

    next.x += snake[0].x;
    next.y += snake[0].y;
    for (int i = snakeSize; i >= 0; --i) {
        snake[i] = snake[i - 1];
    }
    snake[0] = next;
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

void delay(float seconds) {
    float delayTime = seconds * 1000;
    int startTime = clock();
    while (clock() < startTime + delayTime) {
        ;
    }
}