#include <stdio.h>
#include <string.h>
#include <conio.h>  // for _getch()

// Global variables to store user details
char username[20], name[20], email[30], password[20], confirmPassword[20];
int loggedIn = 0;
// Function declarations
void signup();
void getPassword(char *pwd);
void welcomeScreen(); 
int login();

void welcomeScreen() {
   
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
// ================= Main ==============================//
int main() {
    welcomeScreen();
    signup();
     while (!login());
    return 0;
}

// =================== Signup Function ===================
void signup() {
    while (1) {
        printf("=== Signup ===\n");
        printf("Enter Username: ");
        scanf("%s", username);

        printf("Enter Name: ");
        scanf("%s", name);

        printf("Enter Email: ");
        scanf("%s", email);

        printf("Enter Password: ");
        getPassword(password);  // Masked password input

        printf("Confirm Password: ");
        getPassword(confirmPassword);  // Masked confirm password

        if (strcmp(password, confirmPassword) == 0) {
            printf("\nSignup successful!\n");

            FILE *fp = fopen("user.txt", "w");
            if (fp != NULL) {
                fprintf(fp, "%s %s %s %s\n", username, name, email, password);
                fclose(fp);
            }
            break;
        } else {
            printf("\nPasswords do not match. Please try again.\n\n");
        }
    }
}

// =================== Get Password Function ===================
void getPassword(char *pwd) {
    int i = 0;
    char ch;
    while (1) {
        ch = _getch();  // get character without echoing
        if (ch == 13) {  // Enter key
            pwd[i] = '\0';
            break;
        } else if (ch == 8) {  // Backspace
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else {
            if (i < 19) {  // prevent buffer overflow
                pwd[i++] = ch;
                printf("*");  // print asterisk
            }
        }
    }
    printf("\n");
}
// =================== Login =================== //
int login() {
    char uname[30], pass[30];
    char storedUsername[30], storedName[30], storedEmail[50], storedPassword[30];

    FILE *fp = fopen("user.txt", "r");
    if (fp == NULL) {
        printf("No user data found. Please sign up first.\n");
        return 0;
    }

    fscanf(fp, "%s %s %s %s", storedUsername, storedName, storedEmail, storedPassword);
    fclose(fp);

    printf("=== Login ===\n");
    printf("Enter username: ");
    scanf("%s", uname);
    printf("Enter password: ");
    getPassword(pass);

    if (strcmp(uname, storedUsername) == 0 && strcmp(pass, storedPassword) == 0) {
        printf("Login successful!\n");
        strcpy(username, storedUsername);
        strcpy(name, storedName);
        strcpy(email, storedEmail);
        strcpy(password, storedPassword);
        return 1;
    } else {
        printf("Invalid credentials!\n");
        return 0;
    }
}

