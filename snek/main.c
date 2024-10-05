#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <windows.h>

typedef struct {
    int x;
    int y;
} Position;

int gameSpeed;
Position fieldSize;

const Position directions[4] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};

void increaseCapacity(Position **, int *);

char chooseFieldType();
char **createNewField(Position *, int *, char);
void updateField(char **, Position *, int, Position);

void createNewPoint(Position *, char **);

void delay(clock_t);

void draw(char **);

void gotoxy(int, int);
void redraw(Position *, Position, int);

void move(Position *, int, int *);

int main() {
    restart:
    srand(time(NULL));

    char option = chooseFieldType();
    char **field = createNewField(&fieldSize, &gameSpeed, option);
    if (!field) {
        goto restart;
    }

    CONSOLE_CURSOR_INFO ci;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);
    ci.bVisible = 0;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &ci);

    int snakeCapacity = 8;
    Position *snake = malloc(snakeCapacity * sizeof(Position));
    for (int i = 0; i < fieldSize.x; ++i) {
        for (int j = 0; j < fieldSize.y; ++j) {
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
            redraw(snake, point, snakeSize);
            delay(gameSpeed);
        }

        move(snake, snakeSize, &movement);

        if ((field[snake[0].x][snake[0].y] == '#') || (field[snake[0].x][snake[0].y] == 'O')) {
            gameOver = true;
        }

        updateField(field, snake, snakeSize, point);
        //draw(field);
        redraw(snake, point, snakeSize);

        if ((snake[0].x == point.x) && (snake[0].y == point.y)) {
            pointExists = false;
            score = snakeSize++;
            if ((snakeSize + 1) == snakeCapacity) {
                increaseCapacity(&snake, &snakeCapacity);
            }
        }

        delay(gameSpeed);
    }
    gotoxy(fieldSize.y, 0);
    printf("Game over!\n"
           "Score: %d", score);

    for (int i = 0; i < fieldSize.x; ++i) {
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

void increaseCapacity(Position **snake, int *size) {
    *size += 8;
    *snake = realloc(*snake, *size * sizeof(Position));
    if (!(*snake)) {
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

char **createNewField(Position *size, int *speed, char option) {
        if (option == '1') {
            size->x = size->y = 22;
            char **field = malloc((size->x) * sizeof(char *));
            for (int i = 0; i < size->x; ++i) {
                field[i] = malloc(size->y * sizeof(char));
            }
            *speed = 100;
            // borders
            // upper border
            for (int j = 0; j < size->y; ++j) {
                field[0][j] = '#';
            }
            // right border
            for (int i = 1; i < size->x; ++i) {
                field[i][size->y - 1] = '#';
            }
            // bottom border
            for (int j = size->y - 2; j >= 0; --j) {
                field[size->x - 1][j] = '#';
            }
            // left border
            for (int i = size->x - 2; i >= 0; --i) {
                field[i][0] = '#';
            }

            for (int i = 1; i < size->x - 1; ++i) {
                for (int j = 1; j < size->y - 1; ++j) {
                    field[i][j] = ' ';
                }
            }

            field[(size->x - 1) / 2][(size->y - 1) / 2] = '0';
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

            scanf("Game Speed: %d\n", speed);
            fieldSize.x = 1;
            fieldSize.y = 1;

            char **field = malloc(fieldSize.x * sizeof(char *));
            if (!field) {
                printf("Memory allocation failed.\n");
                exit(-1);
            }
            field[0] = malloc(fieldSize.y * sizeof(char));
            if (!(*field)) {
                printf("First row mem allocation failed.");
                exit(1);
            }
            char character;
            int j = 0;
            while ((character = getchar()) != EOF) {
                field[fieldSize.x - 1][j] = character;
                if (getchar() == '\n') {
                    ++fieldSize.x;
                    field = realloc(field, fieldSize.x * sizeof(char *));
                    if (!field) {
                        printf("2D array memory reallocation failed.\n");
                        exit(-2);
                    }
                    if (j > fieldSize.y) {fieldSize.y = j + 1;}
                    j = 0;
                    field[fieldSize.x - 1] = malloc(fieldSize.y * sizeof(char));
                } else {
                    ++j;
                    if (j > fieldSize.y) {
                        field[fieldSize.x - 1] = realloc(field[fieldSize.x - 1], (j + 1) * sizeof(char));
                        if (!field[fieldSize.x - 1]) {
                            printf("Row memory reallocation failed.\n");
                            exit(-3);
                        }
                    }
                }
            }

            for (int i = 0; i < fieldSize.x; ++i) {
                field[i] = realloc(field[i], fieldSize.y * sizeof(char));
                if (!field[i]) {
                    printf("New max row length reallocation failed.\n");
                    exit(-4);
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
        point->x = (rand() % (fieldSize.x - 2)) + 1;
        point->y = (rand() % (fieldSize.y - 2)) + 1;
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
        if (snake[0].x >= fieldSize.x) {
            snake[0].x = 0;
        } else if (snake[0].x < 0) {
            snake[0].x = fieldSize.x - 1;
        }

        if (snake[0].y >= fieldSize.y) {
            snake[0].y = 0;
        } else if (snake[0].y < 0) {
            snake[0].y = fieldSize.y - 1;
        }
    }
}

void draw(char **field) {
    system("cls");
    for (int i = 0; i < fieldSize.x; ++i) {
        for (int j = 0; j < fieldSize.y; ++j) {
            printf("%c ", field[i][j]);
        }
        printf("\n");
    }
}

void gotoxy(int x, int y) {
    COORD cursorPosition;
    cursorPosition.Y = x;
    cursorPosition.X = y * 2;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
}

void redraw(Position *snake, Position point, int snakeSize) {

    gotoxy(snake[0].x, snake[0].y);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xA0);
    printf("0");

    gotoxy(snake[1].x, snake[1].y);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xAA);
    printf("O");

    gotoxy(snake[snakeSize].x, snake[snakeSize].y);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
    printf(" ");

    gotoxy(point.x, point.y);
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0xCC);
    printf("X");

    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x0F);
}

void delay(clock_t seconds) {
    int startTime = clock();
    while (clock() < startTime + seconds) {
        ;
    }
}