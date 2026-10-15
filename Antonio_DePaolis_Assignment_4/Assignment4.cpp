/*
Program Name: Sudoku Solver
Description: Solves sudoku puzzles inputted via a text file and outputs the solutions to a new text file using C++
Inputs: Text file containing sudoku puzzle 9x9 grid with '_' for empty cells and digits 1-9 for filled cells
Outputs: Rewrites the input file containing the original puzzle to include original puzzle, all solutions found, and the number of solutions found.
         If there is no solution outputs "No solution found" to the output file.
Collaborators: Gemini for base code and structure of the program
Other soruces: GeeksforGeeks: (Enum class) https://www.geeksforgeeks.org/cpp/enum-classes-in-c-and-their-advantage-over-enum-datatype/
               CoPilot: for previous idea of iterating through empty cells and backtracking to find solutions
Author: Antonio DePaolis
Creation Date: 10/14/2026
Revision Date: 10/15/2026
Revisions: Removed code that created a new output file and instead overwrote the input file. Modified comments that were affected by this change
            Added new comments to explain the new code. Made changed to other comments to make them more clear and concise. Renamed it to Assignment4.cpp
            As somehow I got the assignment numbers mixed. Modified the prolog to reflect changes
            Changed how the program handles invalid characters in the input file. Now it will return the correct error message and exit the program 
*/

#include <iostream> // Include iostream for standard input and output operations
#include <fstream>  // Include fstream for reading from and writing to files
#include <vector>   // Include vector for dynamic array storage
#include <string>   // Include string for using std::string objects

enum class LoadResult { // Enum class to represent the result of loading a Sudoku board from a file
        Success,    // Indicates successful loading of the Sudoku board
        FileNotFound,   // Indicates that the specified file was not found
        InvalidFormat,   // Indicates that the file format is invalid like incorrect characters or not enough data
        InvalidCharacter,   // Indicates that the file contains invalid characters (not '_' or '1'-'9')
        ExtraData   // Indicates that there is extra data beyond the expected 9x9 grid
    };

class SudokuBoard { // Class representing a Sudoku board
private:    // Private member variables for the Sudoku board and empty cell coordinates
    char grid[9][9];    
    std::vector<std::pair<int, int>> emptyCells; 

public: // Constructor to initialize the Sudoku board with empty cells represented by '_'

    // Default constructor initializing the grid to empty cells by iterating through each cell and setting it to '_'
    SudokuBoard() { // Creates an empty 9x9 Sudoku board with all cells set to '_'
        for (int i = 0; i < 9; ++i) { 
            for (int j = 0; j < 9; ++j) {
                grid[i][j] = '_';
            }
        }
    }

    LoadResult loadFromFile(const std::string& name) {  // Loads the Sudoku board from a file and returns the result
        std::ifstream file(name);   // Open the input file stream with the given file name
        if (!file.is_open()) {  // Check if the file failed to open and return false indicating file not found
            return LoadResult::FileNotFound; 
        }

        emptyCells.clear(); // Clear any previously stored empty cell coordinates

        // Reads the grid from the file entry by entry, checking for valid characters and storing empty cell coordinates
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                std::string token; // Temporary string to hold the read token from the file

                if (!(file >> token)) {    // If reading fails, return false indicating incorrect formatting or insufficient data
                    return LoadResult::InvalidFormat; // Insufficient data or incorrect formatting
                }
                if (token.length() != 1) {  // Check for invalid token length
                    return LoadResult::InvalidCharacter; // Invalid character
                }

                grid[i][j] = token[0]; // Stores the character in the grid

                if (grid[i][j] != '_' && (grid[i][j] < '1' || grid[i][j] > '9')) {  // Check for invalid characters (not '_' or '1'-'9')
                    return LoadResult::InvalidCharacter; // Invalid character
                }

                if (grid[i][j] == '_') {
                    emptyCells.emplace_back(i, j); // Store coordinates of empty cells
                }
            }
        }
        
        char extra; // Variable to check for any extra characters beyond the expected 9x9 grid
        if (file >> extra) {    // If there are extra characters after reading the grid, return false indicating incorrect formatting
            return LoadResult::ExtraData;
        }

        return LoadResult::Success;    // Return true indicating successful loading of the Sudoku board from the file
    }

    const std::vector<std::pair<int, int>>& getEmptyCells() const { // Returns a constant reference to the vector of empty cell coordinates
        return emptyCells;
    }

    char getCell(int r, int c) const {  // Returns the character at the specified row and column in the grid
        return grid[r][c];
    }

    
    void setCell(int r, int c, char val) {  // Sets the character at the specified row and column in the grid
        grid[r][c] = val;
    }

    // Checks if placing a number at the specified row and column is valid according to Sudoku rules
    bool isValid(int row, int col, char num) const {
        for (int i = 0; i < 9; ++i) {   // Check row and column constraints
            if (grid[row][i] == num) return false;
            if (grid[i][col] == num) return false;
        }

        // Checks the 3x3 subgrid constraints
        int startRow = row - row % 3;   // Calculate the starting row index of the 3x3 subgrid
        int startCol = col - col % 3;   // Calculate the starting column index of the 3x3 subgrid
        for (int i = 0; i < 3; ++i) {   // Iterate through the rows of the 3x3 subgrid
            for (int j = 0; j < 3; ++j) {
                if (grid[startRow + i][startCol + j] == num) return false;
            }
        }
        
        return true;    // If all checks pass, the placement is valid
    }

    // Prints the inputted Sudoku board to the provided output stream
    void printToStream(std::ostream& out) const {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                out << grid[i][j] << (j == 8 ? "" : " ");
            }
            out << "\n";
        }
    }
};

class SudokuSolver { // Class responsible for solving the Sudoku puzzle using backtracking
private:    // Private member variables for the Sudoku board and a vector to store board solutions
    SudokuBoard board;
    std::vector<SudokuBoard> solutions;

    // Recursive function to solve the Sudoku puzzle using backtracking
    void solveRecursive(size_t cellIndex) {
        if (cellIndex == board.getEmptyCells().size()) { // Base case: all empty cells are filled
            solutions.push_back(board); // Store the current valid solution
            return;
        }

        auto [row, col] = board.getEmptyCells()[cellIndex]; // Get the coordinates of the current empty cell

        for (char num = '1'; num <= '9'; ++num) { // Tries placing numbers 1-9 in the current empty cell
            if (board.isValid(row, col, num)) { // Checks if placing the number is valid
                board.setCell(row, col, num); // Places the number in the cell
                solveRecursive(cellIndex + 1); // Recurse to fill the next empty cell
                board.setCell(row, col, '_'); // Backtrack: resets the cell to empty after exploring
            }
        }
    }

public: // Public constructor to initialize the SudokuSolver with a given SudokuBoard
    explicit SudokuSolver(const SudokuBoard& b) : board(b) {}   // Constructor initializes the SudokuSolver with a given SudokuBoard

    void solve() {  // Public method to initiate the solving process and clear previous solutions
        solutions.clear();
        solveRecursive(0);
    }

    const std::vector<SudokuBoard>& getSolutions() const {  // Returns a constant reference to the vector of found solutions
        return solutions;
    }
};

class SudokuApp {   // Class to manage the Sudoku application, including user interaction and file handling
public:
    void run() {    // Public method to run the Sudoku application, handling user input and output
        std::string inputFilename;  // Variable to hold the input filename
        std::cout << "Enter the Sudoku puzzle input file name: ";   // Prompts the user for the input file name
        if (!(std::cin >> inputFilename)) { // Reads the input filename from standard input and checks for eEOF or input failure
            std::cerr << "\nError: Input stream closed (EOF).\n"; 
            return;
        }

        SudokuBoard initialBoard;   // Creates an instance of SudokuBoard to hold the initial puzzle state

        switch (initialBoard.loadFromFile(inputFilename)) { // Attempts to load the Sudoku puzzle from the specified file and checks for errors
            case LoadResult::Success: 
                break; // Successfully loaded the puzzle, proceed to solving
            case LoadResult::FileNotFound: 
                std::cerr << "Error: File not found.\n"; 
                return; // File not found, exit the application
            case LoadResult::InvalidFormat: 
                std::cerr << "Error: File has incorrect formatting.\n"; 
                return; // Invalid format, exit the application
            case LoadResult::InvalidCharacter: 
                std::cerr << "Error: File contains invalid characters.\n"; 
                return; // Invalid character(s), exit the application
            case LoadResult::ExtraData: 
                std::cerr << "Error: File contains extra data.\n"; 
                return; // Extra data, exit the application
        }

        SudokuSolver solver(initialBoard);  // Creates an instance of SudokuSolver with the loaded initial board

        solver.solve(); // Calls solve method to find all possible solutions

        const auto& solutions = solver.getSolutions();  // Retrieves the vector of found solutions from the solver

        std::ofstream outFile(inputFilename);  // Opens an input file stream to overwrite the solutions

        if (!outFile.is_open()) {   // Checks if the output file stream failed to open and outputs an error message if so
            std::cerr << "Error: Could not create output file.\n";
            return;
        }
        
        outFile << "Puzzle File: " << inputFilename << "\n\n"; // print puzzle file name at the top
        
        outFile << "Initial Puzzle:\n"; // print initial puzzle
        initialBoard.printToStream(outFile);    // Print the initial puzzle to the output file
        outFile << "\n";    // Add a newline for better formatting

        if (solutions.empty()) {    // If no solutions were found, prints "No solution found" to the output file and notifies the user
            outFile << "No solution found\n";
            std::cout << "No solution found. Output written to " << inputFilename << "\n";
        } else {    // If solutions were found, print each solution to the output file and notify the user of the number of solutions found
            int solNum = 1; // Initialize solution counter
            for (const auto& sol : solutions) { // Iterate through each found solution and print it to the output file
                outFile << "Solution " << solNum << ":\n";  // Print solution number header to the output file
                sol.printToStream(outFile); // Print the current solution to the output file
                outFile << "\n";    // Add a newline for better formatting between solutions
                solNum++;   // Increment solution counter for the next solution
            }
            outFile << "Solutions found: " << solutions.size() << "\n"; // Print the total number of solutions found to the output file
            // Notifies the user of the number of solutions found and the output file name
            std::cout << "Solutions found: " << solutions.size() << ". Results written to " << inputFilename << "\n"; 
        }
    }
};

int main() {    // Main entry point of the Sudoku application
    SudokuApp app;  // Create an instance of the SudokuApp class to manage the application
    app.run();  // Run the Sudoku application, which handles user input, puzzle solving, and output generation
    return 0;   // Return 0 to indicate successful execution of the program
}