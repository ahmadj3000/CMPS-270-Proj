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
// Function to validate column input
int isValidColumn(char colChar)
{
    return (toupper(colChar) >= 'A' && toupper(colChar) <= 'J');
}

// Function to check if the ship can be placed without overlapping or going out of bounds
int isValidPlacement(char grid[GRID_SIZE][GRID_SIZE], int row, int col, int shipSize, char orientation)
{
    if (orientation == 'H')
    {
        if (col + shipSize > GRID_SIZE)
            return 0; // Out of bounds

        for (int i = 0; i < shipSize; i++)
        {
            if (grid[row][col + i] != '~') // Check if space is already occupied
                return 0;
        }
    }
    else if (orientation == 'V')
    {
        if (row + shipSize > GRID_SIZE)
            return 0; // Out of bounds

        for (int i = 0; i < shipSize; i++)
        {
            if (grid[row + i][col] != '~') // Check if space is already occupied
                return 0;
        }
    }
    return 1; // Valid placement
}

// Function to place a ship on the grid
void placeShip(char grid[GRID_SIZE][GRID_SIZE], Ship *ship, int shipSize, const char *shipName)
{
    int row, col;
    char colChar, orientation;
    char input[INPUT_SIZE];

    ship->shipSize = shipSize;    // Store ship size
    strcpy(ship->name, shipName); // Store ship name
    ship->sunk = 1;               // Initialize the sunk flag to 1 (not sunk)

    printf("Place your %s (Size: %d): \n", shipName, shipSize);

    do
    {
        // Get input for column and row (e.g., B3)
        while (1)
        {
            printf("Enter column and row (e.g., B3): ");
            fgets(input, sizeof(input), stdin);

            // Remove newline character from the input buffer
            if (input[strlen(input) - 1] == '\n')
                input[strlen(input) - 1] = '\0';

            // Parse the column and row (e.g., B3)
            if (sscanf(input, " %c%d", &colChar, &row) == 2 && isValidColumn(colChar) && row >= 1 && row <= 10)
            {
                col = toupper(colChar) - 'A'; // Convert 'A'-'J' to 0-9
                row -= 1;                     // Adjust row to zero-index
                break;                        // Valid input, exit loop
            }
            else
            {
                printf("Invalid input. Please enter in the format B3.\n");
            }
        }

        // Get input for orientation (H or V)
        while (1)
        {
            printf("Enter orientation (H for horizontal, V for vertical): ");
            fgets(input, sizeof(input), stdin);

            // Remove newline character from the input buffer
            if (input[strlen(input) - 1] == '\n')
                input[strlen(input) - 1] = '\0';

            // Validate orientation (H or V)
            if (strlen(input) == 1 && (toupper(input[0]) == 'H' || toupper(input[0]) == 'V'))
            {
                orientation = toupper(input[0]); // Convert to uppercase
                break;                           // Valid orientation, exit loop
            }
            else
            {
                printf("Invalid orientation. Please enter H or V.\n");
            }
        }

        // Check for valid placement
        if (!isValidPlacement(grid, row, col, shipSize, orientation))
        {
            printf("Invalid placement. Either out of bounds or overlapping. Try again.\n");
        }

    } while (!isValidPlacement(grid, row, col, shipSize, orientation));

    // Place the ship on the grid and store the coordinates
    if (orientation == 'H')
    {
        for (int i = 0; i < shipSize; i++)
        {
            grid[row][col + i] = 'S';     // Mark ship on grid horizontally
            ship->coords[i][0] = row;     // Store row coordinate
            ship->coords[i][1] = col + i; // Store column coordinate
        }
    }
    else if (orientation == 'V')
    {
        for (int i = 0; i < shipSize; i++)
        {
            grid[row + i][col] = 'S';     // Mark ship on grid vertically
            ship->coords[i][0] = row + i; // Store row coordinate
            ship->coords[i][1] = col;     // Store column coordinate
        }
    }

    printf("\n"); // Add extra newline for readability
}
// Function to display the grid with consideration of the tracking difficulty
void displayGrid(char grid[GRID_SIZE][GRID_SIZE], int trackingDifficulty)
{
    printf("  A B C D E F G H I J\n"); // Column headers
    for (int i = 0; i < GRID_SIZE; i++)
    {
        printf("%d ", i + 1); // Row numbers
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (trackingDifficulty == 2) // Hard mode: only show hits
            {
                if (grid[i][j] == '*') // Show hits
                    printf("* ");
                else // Hide everything else as water
                    printf("~ ");
            }
            else // Easy mode: show hits and misses
            {
                if (grid[i][j] == '*') // Show hits
                    printf("* ");
                else if (grid[i][j] == 'o') // Show misses
                    printf("o ");
                else // Show water as-is
                    printf("~ ");
            }
        }
        printf("\n");
    }
}

// Function to randomly select the first player
int chooseFirstPlayer()
{
    return rand() % 2; // Randomly returns 0 or 1
}

// Function to ask the player for the tracking difficulty
int askForTrackingDifficulty()
{
    char input[INPUT_SIZE];
  int difficulty;

    while (1)
    {
        printf("Choose tracking difficulty: 1 for Easy, 2 for Hard: ");
        fgets(input, sizeof(input), stdin);

        // Check that only a single valid number (1 or 2) was entered and nothing else
        if (sscanf(input, "%d", &difficulty) == 1 && (difficulty == 1 || difficulty == 2) && strlen(input) == 2)
        {
            return difficulty; // Valid difficulty, return it
        }
        else
        {
            printf("Invalid input. Please enter 1 for Easy or 2 for Hard.\n");
        }
    }
}