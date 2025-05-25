#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <windows.h>
#include <time.h>

#define WIDTH 50
#define HEIGHT 25
#define SCREEN_WIDTH 80

// ======== Structures ========
typedef struct {
    char username[20];
    char name[20];
    char email[30];
    char password[20];
} User;

typedef struct {
    int x, y;
    int fruitX, fruitY;
    int score;
    int gameover;
    int tailX[100], tailY[100];
    int nTail;
    int direction;
    int highScore;
    int speed;
} GameState;

// ======== Global Variables ========
User currentUser;
GameState game;

// ======== Function Declarations ========
void signup(User *user);
int login(User *user);
void viewProfile(User *user);
void setup(GameState *game);
void draw(GameState *game);
void input(GameState *game);
void logic(GameState *game);
void snakeGame(User *user, GameState *game);
void menu(User *user, GameState *game);
void getPassword(char *pwd);
void gameOverMenu(GameState *game, User *user);
void welcomeScreen();
void selectDifficulty(GameState *game);
void loadHighScore(GameState *game);
void saveHighScore(GameState *game);
void gotoxy(int x, int y);
void delay(int ms);
void hideCursor();
void setColor(int color);
void updateScore(int *score);

// ======== Helper Functions ========
void gotoxy(int x, int y) {
    COORD coord = {x, y};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void delay(int ms) {
    DWORD start_time = GetTickCount();
    while (GetTickCount() - start_time < ms) {}
}

void hideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void welcomeScreen() {
    system("cls");
    printf("#############################################\n");
    printf("#                                           #\n");
    printf("#           WELCOME TO SNAKE GAME           #\n");
    printf("#                                           #\n");
    printf("#############################################\n");
    printf("#                                           #\n");
    printf("#     Press any key to continue...          #\n");
    printf("#                                           #\n");
    printf("#############################################\n");
    _getch();
}

int main() {
    srand(time(NULL));
    welcomeScreen();
    hideCursor();
    loadHighScore(&game);
    signup(&currentUser);
    while (!login(&currentUser));
    menu(&currentUser, &game);
    return 0;
}

// ======== User Functions ========
void signup(User *user) {
    char confirmPassword[20];
    while (1) {
        system("cls");
        printf("=== Signup ===\n");
        printf("Enter Username: "); scanf("%s", user->username);
        printf("Enter Name: "); scanf("%s", user->name);
        printf("Enter Email: "); scanf("%s", user->email);
        printf("Enter Password: "); getPassword(user->password);
        printf("Confirm Password: "); getPassword(confirmPassword);

        if (strcmp(user->password, confirmPassword) == 0) {
            FILE *fp = fopen("user.txt", "w");
            if (fp) {
                fwrite(user, sizeof(User), 1, fp);
                fclose(fp);
                printf("Signup successful!\n");
            }
            system("pause");
            break;
        } else {
            printf("Passwords do not match. Try again.\n\n");
            system("pause");
        }
    }
}

int login(User *user) {
    char uname[30], pass[30];
    system("cls");
    FILE *fp = fopen("user.txt", "r");
    if (!fp) {
        printf("No user data found. Please sign up first.\n");
        system("pause");
        return 0;
    }
    fread(user, sizeof(User), 1, fp);
    fclose(fp);

    printf("=== Login ===\n");
    printf("Enter username: "); scanf("%s", uname);
    printf("Enter password: "); getPassword(pass);

    if (strcmp(uname, user->username) == 0 && strcmp(pass, user->password) == 0) {
        printf("Login successful!\n");
        system("pause");
        return 1;
    } else {
        printf("Invalid credentials!\n");
        system("pause");
        return 0;
    }
}

void viewProfile(User *user) {
    system("cls");
    printf("\n=== Profile ===\n");
    printf("Username: %s\n", user->username);
    printf("Name: %s\n", user->name);
    printf("Email: %s\n", user->email);
    system("pause");
}

// ======== Game Setup ========
void setup(GameState *game) {
    game->gameover = 0;
    game->x = WIDTH / 2;
    game->y = HEIGHT / 2;
    game->fruitX = rand() % WIDTH;
    game->fruitY = rand() % HEIGHT;
    game->score = 0;
    game->nTail = 0;
    game->direction = 0;  // No movement until arrow pressed
}

// ======== Draw Game ========
void draw(GameState *game) {
    int leftMargin = (SCREEN_WIDTH - WIDTH - 2) / 2;
    gotoxy(leftMargin, 0);

    // Top border
    setColor(10);
    for (int i = 0; i < WIDTH + 2; i++) printf("#");
    printf("\n");

    // Game field rows
    for (int i = 0; i < HEIGHT; i++) {
        gotoxy(leftMargin, i + 1);
        for (int j = 0; j < WIDTH; j++) {
            if (j == 0) setColor(10), printf("#");

            if (i == game->y && j == game->x) {
                setColor(14); printf("O"); // Snake head
            } else if (i == game->fruitY && j == game->fruitX) {
                setColor(12); printf("o"); // Fruit
            } else {
                int printed = 0;
                for (int k = 0; k < game->nTail; k++) {
                    if (game->tailX[k] == j && game->tailY[k] == i) {
                        setColor(11); printf("o"); // Tail
                        printed = 1;
                        break;
                    }
                }
                if (!printed) printf(" ");
            }

            if (j == WIDTH - 1) setColor(10), printf("#");
        }
        printf("\n");
    }

    // Bottom border
    gotoxy(leftMargin, HEIGHT + 1);
    for (int i = 0; i < WIDTH + 2; i++) printf("#");

    // Score display below game
    setColor(7);
    gotoxy(leftMargin + WIDTH / 2, HEIGHT + 3);
    printf("Score: %d   High Score: %d", game->score, game->highScore);
}

// ======== Input Handling ========
void input(GameState *game) {
    if (_kbhit()) {
        switch (_getch()) {
            case 72: if(game->direction != 2) game->direction = 1; break; // Up
            case 80: if(game->direction != 1) game->direction = 2; break; // Down
            case 75: if(game->direction != 4) game->direction = 3; break; // Left
            case 77: if(game->direction != 3) game->direction = 4; break; // Right
            case 'x': game->gameover = 1; break; // Exit game
        }
    }
}

// ======== Logic ========
void logic(GameState *game) {
    int prevX = game->tailX[0];
    int prevY = game->tailY[0];
    int prev2X, prev2Y;

    game->tailX[0] = game->x;
    game->tailY[0] = game->y;

    for (int i = 1; i < game->nTail; i++) {
        prev2X = game->tailX[i];
        prev2Y = game->tailY[i];
        game->tailX[i] = prevX;
        game->tailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }

    // Update head position based on direction
    switch (game->direction) {
        case 1: game->y--; break;
        case 2: game->y++; break;
        case 3: game->x--; break;
        case 4: game->x++; break;
    }

    // Check for wall collision
    if (game->x < 0 || game->x >= WIDTH || game->y < 0 || game->y >= HEIGHT)
        game->gameover = 1;

    // Check for collision with tail
    for (int i = 0; i < game->nTail; i++) {
        if (game->tailX[i] == game->x && game->tailY[i] == game->y)
            game->gameover = 1;
    }

    // Check if fruit eaten
    if (game->x == game->fruitX && game->y == game->fruitY) {
        updateScore(&game->score);  // Using pointer function
        game->fruitX = rand() % WIDTH;
        game->fruitY = rand() % HEIGHT;
        game->nTail++;
    }
}

// ======== Pointer function example ========
void updateScore(int *score) {
    *score += 10;
}

// ======== Game Over Menu ========
void gameOverMenu(GameState *game, User *user) {
    system("cls");
    if (game->score > game->highScore) {
        game->highScore = game->score;
        saveHighScore(game);
        printf("New High Score!\n");
    }
    printf("=== Game Over ===\n");
    printf("Your Score: %d\n", game->score);
    printf("High Score: %d\n\n", game->highScore);

    printf("1. Restart Game\n");
    printf("2. Return to Main Menu\n");
    printf("Choose an option: ");

    int choice;
    while (1) {
        scanf("%d", &choice);
        if (choice == 1) {
            snakeGame(user, game);
            break;
        } else if (choice == 2) {
            break;
        } else {
            printf("Invalid choice! Choose again: ");
        }
    }
}

// ======== Snake Game ========
void snakeGame(User *user, GameState *game) {
    system("cls");
    selectDifficulty(game);  // Show difficulty once, before game starts
    system("cls");           // Clear screen immediately after difficulty selection to hide it

    setup(game);

    while (!game->gameover) {
        draw(game);
        input(game);
        logic(game);
        delay(game->speed);
    }

    gameOverMenu(game, user);
}

// ======== Main Menu ========
void menu(User *user, GameState *game) {
    int choice;
    do {
        system("cls");
        printf("=== Main Menu ===\n");
        printf("1. View Profile\n");
        printf("2. Play Snake Game\n");
        printf("3. Logout\n");
        printf("Choose: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: viewProfile(user); break;
            case 2: snakeGame(user, game); break;
            case 3: printf("Logging out...\n"); delay(1000); break;
            default: printf("Invalid choice!\n"); system("pause");
        }
    } while (choice != 3);
}

// ======== Get Password (masked input) ========
void getPassword(char *pwd) {
    int i = 0;
    char ch;
    while (1) {
        ch = _getch();
        if (ch == 13) {  // Enter key
            pwd[i] = '\0';
            break;
        } else if (ch == 8 && i > 0) { // Backspace
            i--;
            printf("\b \b");
        } else {
            pwd[i++] = ch;
            printf("*");
        }
    }
    printf("\n");
}


// ======== Difficulty Selection ========
void selectDifficulty(GameState *game) {
    int choice;
    printf("Select Difficulty Level:\n");
    printf("1. Easy\n2. Medium\n3. Hard\nChoose (1-3): ");
    while (1) {
        scanf("%d", &choice);
        if (choice >= 1 && choice <= 3) break;
        printf("Invalid choice! Choose 1, 2, or 3: ");
    }

    switch (choice) {
        case 1: game->speed = 200; break;
        case 2: game->speed = 100; break;
        case 3: game->speed = 50;  break;
    }
}

// ======== High Score Handling ========
void loadHighScore(GameState *game) {
    FILE *fp = fopen("highscore.txt", "r");
    game->highScore = 0;
    if (fp) {
        fscanf(fp, "%d", &game->highScore);
        fclose(fp);
    }
}

void saveHighScore(GameState *game) {
    FILE *fp = fopen("highscore.txt", "w");
    if (fp) {
        fprintf(fp, "%d", game->highScore);
        fclose(fp);
    }
}



