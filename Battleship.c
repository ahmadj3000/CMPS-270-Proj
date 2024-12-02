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
// Function to initialize the grid with water '~'
void initializeGrid(char grid[GRID_SIZE][GRID_SIZE])
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            grid[i][j] = '~'; // Water representation
        }
    }
}

// Function to check if a ship is sunk
int isShipSunk(char grid[GRID_SIZE][GRID_SIZE], Ship ship)
{
    for (int i = 0; i < ship.shipSize; i++)
    {
        int row = ship.coords[i][0];
        int col = ship.coords[i][1];
        if (grid[row][col] != '*') // If any part of the ship is not hit
        {
            return 0; // The ship is not yet sunk
        }
    }
    // Inside the part where you detect ship sinking
    return 1; // All parts of the ship are hit, so it's sunk
}

// Function to ask for firing coordinates, similar to ship placement
void getFiringCoordinates(int *row, int *col)
{
    char input[INPUT_SIZE];
    char colChar;

    // Get single line input for firing coordinates (e.g., B1)
    while (1)
    {
        printf("Enter coordinates to fire at (e.g., B1): ");
        fgets(input, sizeof(input), stdin);

        // Parse the input to get column and row
        if (sscanf(input, " %c%d", &colChar, row) == 2 && isValidColumn(colChar) && *row >= 1 && *row <= 10)
        {
            *col = toupper(colChar) - 'A'; // Convert 'A'-'J' to 0-9
            *row -= 1;                     // Adjust row to zero-index
            break;                         // Valid input, exit loop
        }
        else
        {
            printf("Invalid input. Please enter in the format B1 or A6.\n");
        }
    }

    printf("\n"); // Add extra newline for readability
}
// Function to count the remaining parts of a ship on the grid
int countShipParts(char grid[GRID_SIZE][GRID_SIZE], char shipMarker)
{
    int count = 0;
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == shipMarker)
            {
                count++;
            }
        }
    }
    return count;
}

// Function to fire at a coordinate
int fireAtCoordinate(char grid[GRID_SIZE][GRID_SIZE], int row, int col, Ship ships[NUM_SHIPS], int trackingDifficulty)
{
    if (grid[row][col] == 'S') // Ship hit
    {
        grid[row][col] = '*'; // Mark as hit

        // If in Easy mode, show hit message
        if (trackingDifficulty == 1)
        {
            printf("Hit!\n");
        }

        return 1; // Hit
    }
    else if (grid[row][col] == '~') // Miss
    {
        grid[row][col] = 'o'; // Mark as miss

        // If in Easy mode, show miss message
        if (trackingDifficulty == 1)
        {
            printf("Miss.\n");
        }

        return 0; // Miss
    }
    else
    {
        // Already fired at this location, no additional feedback needed
        if (trackingDifficulty == 1)
        {
            printf("You've already fired here!\n");
        }

        return 0; // No hit
    }
}

// Function to check if all ships are sunk
int allShipsSunk(char grid[GRID_SIZE][GRID_SIZE])
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[i][j] == 'S')
                return 0; // Still a ship remaining
        }
    }
    return 1; // All ships are sunk
}
// Function to switch between players
int switchPlayer(int currentPlayer)
{
    return currentPlayer == 0 ? 1 : 0;
}

// Perform artillery strike (hits a 2x2 area)
void artilleryStrike(char grid[GRID_SIZE][GRID_SIZE], int row, int col, int trackingDifficulty)
{
    printf("Firing artillery at area %c%d to %c%d\n", col + 'A', row + 1, col + 'A' + 1, row + 2);

    for (int i = row; i < row + 2 && i < GRID_SIZE; i++)
    {
        for (int j = col; j < col + 2 && j < GRID_SIZE; j++)
        {
            if (grid[i][j] == 'S')
            {
                printf("Hit at %c%d!\n", j + 'A', i + 1);
                grid[i][j] = '*';
            }
            else
            {
                if (trackingDifficulty == 1) // Easy Mode
                {
                    printf("Miss at %c%d.\n", j + 'A', i + 1);
                    grid[i][j] = 'o'; // Mark miss
                }
                else // Hard Mode
                {
                    printf("Miss at %c%d.\n", j + 'A', i + 1);
                    // Optionally, you can choose not to mark misses in Hard Mode
                }
            }
        }
    }
}

// Function to perform Torpedo attack
int torpedoAttack(char grid[GRID_SIZE][GRID_SIZE], char choice, int num, int trackingDifficulty)
{
    int hit = 0;

    if (choice == 'R')
    { // Row attack
        printf("Firing torpedo at row %d\n", num + 1);
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (grid[num][j] == 'S')
            {
                grid[num][j] = '*'; // Mark hit
                hit = 1;
                if (trackingDifficulty == 1) // Only print in easy mode
                {
                    printf("Hit at %c%d!\n", j + 'A', num + 1);
                }
            }
            else
            {
                if (trackingDifficulty == 1) // Only print and mark miss in easy mode
                {
                    printf("Miss at %c%d.\n", j + 'A', num + 1);
                    grid[num][j] = 'o'; // Mark miss
                }
            }
        }
    }
    else if (choice == 'C')
    { // Column attack
        printf("Firing torpedo at column %c\n", num + 'A');
        for (int i = 0; i < GRID_SIZE; i++)
        {
            if (grid[i][num] == 'S')
            {
                grid[i][num] = '*'; // Mark hit
                hit = 1;
                if (trackingDifficulty == 1) // Only print in easy mode
                {
                    printf("Hit at %c%d!\n", num + 'A', i + 1);
                }
            }
            else
            {
                if (trackingDifficulty == 1) // Only print and mark miss in easy mode
                {
                    printf("Miss at %c%d.\n", num + 'A', i + 1);
                    grid[i][num] = 'o'; // Mark miss
                }
            }
        }
    }

    return hit;
}
void radarSweep(char grid[GRID_SIZE][GRID_SIZE], int smokeGrid[GRID_SIZE][GRID_SIZE], int row, int col)
{
    printf("Performing radar sweep on area %c%d to %c%d\n", col + 'A', row + 1, col + 'A' + 1, row + 2);
    int foundShip = 0;

    for (int i = row; i < row + 2 && i < GRID_SIZE; i++)
    {
        for (int j = col; j < col + 2 && j < GRID_SIZE; j++)
        {
            if (smokeGrid[i][j] == 0 && grid[i][j] == 'S') // Skip cells that are under smoke
            {
                foundShip = 1;
            }
        }
    }

    if (foundShip)
    {
        printf("Enemy ships found in the area.\n");
    }
    else
    {
        printf("No enemy ships found in the area.\n");
    }
}

void smokeScreen(int smokeDurationGrid[GRID_SIZE][GRID_SIZE], int row, int col)
{
    printf("Deploying smoke screen on area %c%d to %c%d\n", col + 'A', row + 1, col + 'A' + 1, row + 2);
    for (int i = row; i < row + 2 && i < GRID_SIZE; i++)
 {
        for (int j = col; j < col + 2 && j < GRID_SIZE; j++)
        {
            smokeDurationGrid[i][j] = 1; // Set smoke duration for 1 round
        }
    }
}

void reduceSmokeDuration(int smokeDurationGrid[GRID_SIZE][GRID_SIZE])
{
    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            if (smokeDurationGrid[i][j] > 0) // If smoke is active
            {
                smokeDurationGrid[i][j]--; // Decrease duration
            }
        }
    }
}
// Move handler
void performMove(
    char grid[GRID_SIZE][GRID_SIZE],
    Ship ships[NUM_SHIPS],
    int *radarUses,
    int *smokeScreenUses,
    int mySmokeDurationGrid[GRID_SIZE][GRID_SIZE],
    int opponentSmokeDurationGrid[GRID_SIZE][GRID_SIZE],
    int trackingDifficulty,
    int *sunkTotal,
    int *artilleryLifetime,
    int *torpedoLifetime)
{
    char move[INPUT_SIZE];
    int row, col;
    int validMove = 0; // Flag to check if a valid move was chosen

    while (!validMove) // Loop until a valid move is chosen
    {
        printf("Choose your move (Fire, Radar, Smoke, Artillery, Torpedo): ");
        fgets(move, sizeof(move), stdin);
        move[strcspn(move, "\n")] = '\0';

        // Convert move to uppercase for case insensitivity
        for (int i = 0; move[i]; i++)
        {
            move[i] = toupper(move[i]);
        }

        if (strncmp(move, "FIRE", 4) == 0)
        {
            getFiringCoordinates(&row, &col);
            fireAtCoordinate(grid, row, col, ships, trackingDifficulty);
            validMove = 1; // Mark the move as valid
        }
        else if (strncmp(move, "RADAR", 5) == 0)
        {
            if (*radarUses > 0)
            {
                // Get the coordinates and perform the radar sweep
                getFiringCoordinates(&row, &col);
                radarSweep(grid, opponentSmokeDurationGrid, row, col); // Perform radar sweep
                (*radarUses)--;                                        // Decrease the available radar uses for the current player
                validMove = 1;                                         // Mark the move as valid
            }
            else
            {
                printf("No radar sweeps left!\n");
            }
        }
        else if (strncmp(move, "SMOKE", 5) == 0)
        {
            if (*smokeScreenUses > 0)
            {
                // Get the coordinates and deploy the smoke screen
                getFiringCoordinates(&row, &col);
                smokeScreen(mySmokeDurationGrid, row, col); // Set the smoke duration
                (*smokeScreenUses)--;                       // Decrease the available smoke screen uses for the current player
                validMove = 1;                              // Mark the move as valid
            }
            else
            {
                printf("No smoke screens left!\n");
            }
        }
        else if (strncmp(move, "ARTILLERY", 9) == 0 && *artilleryLifetime > 0)
        {
            getFiringCoordinates(&row, &col);
            artilleryStrike(grid, row, col, trackingDifficulty);
            printf("Artillery used successfully!\n");
            *artilleryLifetime = 0; // Deactivate after use
            validMove = 1;          // Mark the move as valid
        }
        else if (strncmp(move, "TORPEDO", 7) == 0 && *torpedoLifetime > 0 && *sunkTotal >= 3)
        {
            char choice;
            int isValidInput = 0; // Flag to check if input is valid

            // Loop until valid input for row (R) or column (C)
            while (!isValidInput)
            {
                printf("Choose row (R) or column (C): ");
                scanf(" %c", &choice);
                choice = toupper(choice); // Make case insensitive

                if (choice == 'R' || choice == 'C') // Check if input is 'R' or 'C'
                {
                    isValidInput = 1; // Valid input
                }
                else
                {
                    printf("Invalid choice. Please enter 'R' for row or 'C' for column.\n");
                }
            }

            if (choice == 'R')
            {
                isValidInput = 0; // Reset flag for row validation
                while (!isValidInput)
                {
                    printf("Enter row number (1-10): ");
                    if (scanf("%d", &row) == 1 && row >= 1 && row <= 10) // Check if input is a valid number
                    {
                        isValidInput = 1; // Valid input
                    }
                    else
                    {
                        printf("Invalid row. Please enter a number between 1 and 10.\n");
                        clearInputBuffer(); // Clear invalid input
                    }
                }
                torpedoAttack(grid, 'R', row - 1, trackingDifficulty); // Perform row attack
                printf("Torpedo used successfully on row %d!\n", row);
                *torpedoLifetime = 0; // Deactivate after use
                validMove = 1;        // Mark the move as valid
            }
            else if (choice == 'C')
            {
                isValidInput = 0; // Reset flag for column validation
                char colChar;
                while (!isValidInput)
                {
                    printf("Enter column letter (A-J): ");
                    scanf(" %c", &colChar);
                    colChar = toupper(colChar); // Convert to uppercase

                    if (colChar >= 'A' && colChar <= 'J') // Check if input is a valid column letter
                    {
                        isValidInput = 1;                                            // Valid input
                        torpedoAttack(grid, 'C', colChar - 'A', trackingDifficulty); // Perform column attack
                        printf("Torpedo used successfully on column %c!\n", colChar);
                        *torpedoLifetime = 0; // Deactivate after use
                        validMove = 1;        // Mark the move as valid
                    }
                    else
                    {
                        printf("Invalid column. Please enter a letter between A and J.\n");
                    }
                }
            }
            clearInputBuffer(); // Clear the input buffer after scanf
        }
        else
        {
            printf("Invalid move! Please enter a valid move.\n");
        }
    }
}
int isAdjacent(char grid[GRID_SIZE][GRID_SIZE], int row, int col, int shipSize, char orientation)
{
    int directions[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};

    if (orientation == 'H')
    {
        for (int i = 0; i < shipSize; i++)
        {
            int r = row;
            int c = col + i;
            for (int d = 0; d < 8; d++)
            {
                int adjR = r + directions[d][0];
                int adjC = c + directions[d][1];
                if (adjR >= 0 && adjR < GRID_SIZE && adjC >= 0 && adjC < GRID_SIZE && grid[adjR][adjC] == 'S')
                {
                    return 1; // Adjacent to another ship
                }
            }
        }
    }
    else if (orientation == 'V')
    {
        for (int i = 0; i < shipSize; i++)
        {
            int r = row + i;
            int c = col;
            for (int d = 0; d < 8; d++)
            {
                int adjR = r + directions[d][0];
                int adjC = c + directions[d][1];
                if (adjR >= 0 && adjR < GRID_SIZE && adjC >= 0 && adjC < GRID_SIZE && grid[adjR][adjC] == 'S')
                {
                    return 1; // Adjacent to another ship
                }
            }
        }
    }

    return 0; // No adjacent ships
}

// Helper function to place a single ship automatically
void autoPlaceSingleShip(char grid[GRID_SIZE][GRID_SIZE], Ship *ship, int shipSize, const char *shipName)
{
    int valid = 0, attempts = 0;
    while (!valid)
    {
        if (attempts > 100)
        { // Safeguard: if too many attempts, reset grid
            printf("Too many attempts to place ships. Restarting bot placement...\n");
            initializeGrid(grid); // Reset grid
            attempts = 0;         // Reset attempts
        }

        int row = rand() % GRID_SIZE;
        int col = rand() % GRID_SIZE;
        char orientation = (rand() % 2 == 0) ? 'H' : 'V'; // Randomize orientation

        if (isValidPlacement(grid, row, col, shipSize, orientation) &&
            !isAdjacent(grid, row, col, shipSize, orientation))
        {
            // Place the ship on the grid
            if (orientation == 'H')
            {
                for (int i = 0; i < shipSize; i++)
                {
                    grid[row][col + i] = 'S';     // Mark ship on grid horizontally
                    ship->coords[i][0] = row;     // Store row coordinate
                    ship->coords[i][1] = col + i; // Store column coordinate
                }
            }
            else // 'V'
            {
                for (int i = 0; i < shipSize; i++)
                {
                    grid[row + i][col] = 'S';     // Mark ship on grid vertically
                    ship->coords[i][0] = row + i; // Store row coordinate
                    ship->coords[i][1] = col;     // Store column coordinate
                }
            }

            // Initialize ship properties
            ship->shipSize = shipSize;
            strcpy(ship->name, shipName);
            ship->sunk = 1; // Initialize the sunk flag to 1 (not sunk)

            valid = 1; // Ship placed successfully
        }
        attempts++;
    }
}
