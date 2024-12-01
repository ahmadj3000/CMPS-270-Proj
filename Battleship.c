#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#define GRID_SIZE 10
#define INPUT_SIZE 100
#define NAME_SIZE 50

#define NUM_SHIPS 4 // Carrier, Battleship, Destroyer, Submarine

// To store the coordinates of the ships
typedef struct
{
    int shipSize;
    int coords[5][2];
    char name[20];
    int sunk;
} Ship;

// Create arrays to store ship coordinates for both players
Ship player1Ships[NUM_SHIPS];
Ship player2Ships[NUM_SHIPS];

int smokeDurationGrid[GRID_SIZE][GRID_SIZE] = {0}; // To track smoke durations

// Function to clear the screen (platform dependent)
void clearScreen()
{
#ifdef _WIN32
    system("cls"); // For Windows
#else
    system("clear"); // For Linux/Unix/Mac
#endif
}

// Function to clear input buffer
void clearInputBuffer()
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
        ;
}