#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class SudokuBoard {
private:
    char grid[9][9];
    std::string fileName;

public:
    SudokuBoard() {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                grid[i][j] = '_';
            }
        }
    }

    bool loadFromFile(const std::string& name) {
        fileName = name;
        std::ifstream file(fileName);
        if (!file.is_open()) {
            return false;
        }

        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                if (!(file >> grid[i][j])) {
                    return false; // Insufficient data or incorrect formatting
                }
                if (grid[i][j] != '_' && (grid[i][j] < '1' || grid[i][j] > '9')) {
                    return false; // Invalid character
                }
            }
        }
        
        // Verify there is no extra trailing data
        char extra;
        if (file >> extra) {
            return false;
        }

        return true;
    }

    std::string getFileName() const {
        return fileName;
    }

    char getCell(int r, int c) const {
        return grid[r][c];
    }

    void setCell(int r, int c, char val) {
        grid[r][c] = val;
    }

    bool isValid(int row, int col, char num) const {
        // Check row and column constraints
        for (int i = 0; i < 9; ++i) {
            if (grid[row][i] == num) return false;
            if (grid[i][col] == num) return false;
        }

        // Check 3x3 subgrid constraints
        int startRow = row - row % 3;
        int startCol = col - col % 3;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (grid[startRow + i][startCol + j] == num) return false;
            }
        }

        return true;
    }

    void printToStream(std::ostream& out) const {
        for (int i = 0; i < 9; ++i) {
            for (int j = 0; j < 9; ++j) {
                out << grid[i][j] << (j == 8 ? "" : " ");
            }
            out << "\n";
        }
    }
};

class SudokuSolver {
private:
    SudokuBoard board;
    std::vector<SudokuBoard> solutions;

    void solveRecursive(int row, int col) {
        if (row == 9) {
            solutions.push_back(board);
            return;
        }

        int nextRow = (col == 8) ? row + 1 : row;
        int nextCol = (col == 8) ? 0 : col + 1;

        if (board.getCell(row, col) != '_') {
            solveRecursive(nextRow, nextCol);
        } else {
            for (char num = '1'; num <= '9'; ++num) {
                if (board.isValid(row, col, num)) {
                    board.setCell(row, col, num);
                    solveRecursive(nextRow, nextCol);
                    board.setCell(row, col, '_'); // Backtrack
                }
            }
        }
    }

public:
    explicit SudokuSolver(const SudokuBoard& b) : board(b) {}

    void solve() {
        solutions.clear();
        solveRecursive(0, 0);
    }

    const std::vector<SudokuBoard>& getSolutions() const {
        return solutions;
    }
};

class SudokuApp {
public:
    void run() {
        std::string inputFilename;
        std::cout << "Enter the Sudoku puzzle input file name: ";
        if (!(std::cin >> inputFilename)) {
            return;
        }

        SudokuBoard initialBoard;
        if (!initialBoard.loadFromFile(inputFilename)) {
            std::cerr << "Error: File does not exist or has incorrect formatting.\n";
            return;
        }

        SudokuSolver solver(initialBoard);
        solver.solve();

        const auto& solutions = solver.getSolutions();
        std::string outputFilename = inputFilename + "_solution.txt";
        std::ofstream outFile(outputFilename);

        if (!outFile.is_open()) {
            std::cerr << "Error: Could not create output file.\n";
            return;
        }

        // Write puzzle file name at the top
        outFile << "Puzzle File: " << initialBoard.getFileName() << "\n\n";

        // Write initial puzzle
        outFile << "Initial Puzzle:\n";
        initialBoard.printToStream(outFile);
        outFile << "\n";

        if (solutions.empty()) {
            outFile << "No solution found\n";
            std::cout << "No solution found. Output written to " << outputFilename << "\n";
        } else {
            int solNum = 1;
            for (const auto& sol : solutions) {
                outFile << "Solution " << solNum << ":\n";
                sol.printToStream(outFile);
                outFile << "\n";
                solNum++;
            }
            outFile << "Solutions found: " << solutions.size() << "\n";
            std::cout << "Solutions found: " << solutions.size() << ". Results written to " << outputFilename << "\n";
        }
    }
};

int main() {
    SudokuApp app;
    app.run();
    return 0;
}