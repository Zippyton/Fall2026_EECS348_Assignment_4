/*
Program Name: Sudoku Solver
Description: Solves sudoku puzzles inputted via a text file and outputs the solutions to a new text file using C++
Inputs: Text file containing sudoku puzzle 9x9 grid with '_' for empty cells and digits 1-9 for filled cells
Outputs: Outputs a text file containing original input puzzle and it's solutions to the inputted sudoku puzzle. 
         Also outputs the number of solutions found for each puzzle. If no solution outputs "No solution found" to the output file.
Collaborators: Gemini for base code and structure of the program
Other soruces: GeeksforGeeks: (Enum class) https://www.geeksforgeeks.org/cpp/enum-classes-in-c-and-their-advantage-over-enum-datatype/
               CoPilot: for previous idea of iterating through empty cells and backtracking to find solutions
Author: Antonio DePaolis
Creation Date: 10/14/2026
Revision Date: 10/15/2026
Revisions: Added More comments that were missing
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
private:    // Private member variables for the Sudoku board, empty cell coordinates, and file name
    char grid[9][9];    
    std::vector<std::pair<int, int>> emptyCells; 
    std::string fileName;  

public: // Constructor to initialize the Sudoku board with empty cells represented by '_'

    // Default constructor initializing the grid to empty cells
    SudokuBoard() { // Creates an empty 9x9 Sudoku board with all cells set to '_'
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                grid[i][j] = '_';
            }
        }
    }

    // Loads the Sudoku board from a file and returns the result
    LoadResult loadFromFile(const std::string& name) {
        fileName = name;    // Store the file name for reference
        std::ifstream file(fileName);   // Open the input file stream with the given file name
        if (!file.is_open()) {  // Check if the file failed to open and return false indicating file not found
            return LoadResult::FileNotFound; 
        }

        emptyCells.clear(); // Clear any previously stored empty cell coordinates

        // Reads the grid from the file entry by entry, checking for valid characters and storing empty cell coordinates
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (!(file >> grid[i][j])) {    // If reading fails, return false indicating incorrect formatting or insufficient data
                    return LoadResult::InvalidFormat; // Insufficient data or incorrect formatting
                }
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

    
    std::string getFileName() const {   // Returns the file name associated with the Sudoku board
        return fileName;
    }

    // Returns the vector of empty cell coordinates
    const std::vector<std::pair<int, int>>& getEmptyCells() const { // Returns a constant reference to the vector of empty cell coordinates
        return emptyCells;
    }

   
    char getCell(int r, int c) const {  // Returns the character at the specified row and column in the grid
        return grid[r][c];
    }

    // Sets the character at the specified row and column in the grid
    void setCell(int r, int c, char val) {
        grid[r][c] = val;
    }

    // Checks if placing a number at the specified row and column is valid according to Sudoku rules
    bool isValid(int row, int col, char num) const {
        // Check row and column constraints
        for (int i = 0; i < 9; ++i) {
            if (grid[row][i] == num) return false;
            if (grid[i][col] == num) return false;
        }

        // Checks the 3x3 subgrid constraints
        int startRow = row - row % 3;   // Calculate the starting row index of the 3x3 subgrid
        int startCol = col - col % 3;   // Calculate the starting column index of the 3x3 subgrid
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (grid[startRow + i][startCol + j] == num) return false;
            }
        }

        // If all checks pass, the placement is valid
        return true;
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
                return; // Invalid characters, exit the application
            case LoadResult::ExtraData: 
                std::cerr << "Error: File contains extra data.\n"; 
                return; // Extra data, exit the application
        }

        SudokuSolver solver(initialBoard);  // Creates an instance of SudokuSolver with the loaded initial board
        solver.solve();

        std::string outputFilename; // Variable to hold the output filename
        const auto& solutions = solver.getSolutions();  // Retrieves the vector of found solutions from the solver
        if (inputFilename.length() >= 4 && inputFilename.substr(inputFilename.length() - 4) == ".txt") {
            // It ends with .txt, so safely strip the extension then append _solution.txt
            outputFilename = inputFilename.substr(0, inputFilename.length() - 4)+ "_solution.txt";
        } 

        else {  // It does not end with .txt, keep the full name as is
            outputFilename = inputFilename + "_solution.txt";  
        }

        std::ofstream outFile(outputFilename);  // Opens an output file stream to write the solutions to the specified output file

        if (!outFile.is_open()) {   // Checks if the output file stream failed to open and outputs an error message if so
            std::cerr << "Error: Could not create output file.\n";
            return;
        }

        
        outFile << "Puzzle File: " << initialBoard.getFileName() << "\n\n"; // Write puzzle file name at the top

        
        outFile << "Initial Puzzle:\n"; // Write initial puzzle
        initialBoard.printToStream(outFile);    // Print the initial puzzle to the output file
        outFile << "\n";    // Add a newline for better formatting

        if (solutions.empty()) {    // If no solutions were found, writes "No solution found" to the output file and notifies the user
            outFile << "No solution found\n";
            std::cout << "No solution found. Output written to " << outputFilename << "\n";
        } else {    // If solutions were found, write each solution to the output file and notify the user of the number of solutions found
            int solNum = 1; // Initialize solution counter
            for (const auto& sol : solutions) { // Iterate through each found solution and write it to the output file
                outFile << "Solution " << solNum << ":\n";  // Write solution number header to the output file
                sol.printToStream(outFile); // Print the current solution to the output file
                outFile << "\n";    // Add a newline for better formatting between solutions
                solNum++;   // Increment solution counter for the next solution
            }
            outFile << "Solutions found: " << solutions.size() << "\n"; // Write the total number of solutions found to the output file
            // Notifies the user of the number of solutions found and the output file name
            std::cout << "Solutions found: " << solutions.size() << ". Results written to " << outputFilename << "\n"; 
        }
    }
};

int main() {    // Main entry point of the Sudoku application
    SudokuApp app;  // Create an instance of the SudokuApp class to manage the application
    app.run();  // Run the Sudoku application, which handles user input, puzzle solving, and output generation
    return 0;   // Return 0 to indicate successful execution of the program
}