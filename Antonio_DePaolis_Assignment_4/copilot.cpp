#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

class SudokuSolver
{
private:
    vector<vector<char>> board;
    vector<vector<vector<char>>> solutions;
    vector<pair<int, int>> emptyCells;
    string inputFileName;

public:
    SudokuSolver()
    {
        board.resize(9, vector<char>(9));
    }

    bool loadPuzzle(const string& fileName)
    {
        inputFileName = fileName;

        ifstream fin(fileName);

        if (!fin)
        {
            cerr << "Error: Input file does not exist." << endl;
            return false;
        }

        string token;
        int count = 0;

        while (fin >> token)
        {
            if (count >= 81)
            {
                cerr << "Error: Too many entries in puzzle." << endl;
                return false;
            }

            char value = token[0];

            if (!((value >= '1' && value <= '9') || value == '_'))
            {
                cerr << "Error: Invalid character in puzzle." << endl;
                return false;
            }

            board[count / 9][count % 9] = value;
            count++;
        }

        if (count != 81)
        {
            cerr << "Error: Puzzle must contain exactly 81 entries." << endl;
            return false;
        }

        findEmptyCells();
        return true;
    }

    void findEmptyCells()
    {
        emptyCells.clear();

        for (int r = 0; r < 9; r++)
        {
            for (int c = 0; c < 9; c++)
            {
                if (board[r][c] == '_')
                {
                    emptyCells.push_back({r, c});
                }
            }
        }
    }

    bool validPlacement(int row, int col, char value)
    {
        for (int i = 0; i < 9; i++)
        {
            if (board[row][i] == value)
                return false;
        }

        for (int i = 0; i < 9; i++)
        {
            if (board[i][col] == value)
                return false;
        }

        int startRow = (row / 3) * 3;
        int startCol = (col / 3) * 3;

        for (int r = startRow; r < startRow + 3; r++)
        {
            for (int c = startCol; c < startCol + 3; c++)
            {
                if (board[r][c] == value)
                    return false;
            }
        }

        return true;
    }

    vector<char> getCandidates(int row, int col)
    {
        vector<char> candidates;

        for (char num = '1'; num <= '9'; num++)
        {
            if (validPlacement(row, col, num))
            {
                candidates.push_back(num);
            }
        }

        return candidates;
    }

    void solveDFS(int cellIndex)
    {
        if (cellIndex == emptyCells.size())
        {
            solutions.push_back(board);
            return;
        }

        int row = emptyCells[cellIndex].first;
        int col = emptyCells[cellIndex].second;

        vector<char> candidates = getCandidates(row, col);

        if (candidates.empty())
        {
            return;
        }

        for (char candidate : candidates)
        {
            board[row][col] = candidate;

            solveDFS(cellIndex + 1);

            board[row][col] = '_';
        }
    }

    void solve()
    {
        solveDFS(0);
    }

    void printBoard(ofstream& fout,
                    const vector<vector<char>>& currentBoard)
    {
        for (int r = 0; r < 9; r++)
        {
            for (int c = 0; c < 9; c++)
            {
                fout << currentBoard[r][c];

                if (c != 8)
                {
                    fout << " ";
                }
            }
            fout << endl;
        }
    }

    bool writeResults()
    {
        string outputFile = inputFileName + "solution.txt";

        ofstream fout(outputFile);

        if (!fout)
        {
            cerr << "Error: Cannot create output file." << endl;
            return false;
        }

        fout << "Puzzle File: " << inputFileName << endl;
        fout << endl;

        fout << "Original Puzzle:" << endl;
        printBoard(fout, board);
        fout << endl;

        if (solutions.empty())
        {
            fout << "No solution found" << endl;
        }
        else
        {
            for (size_t i = 0; i < solutions.size(); i++)
            {
                fout << "Solution " << i + 1 << ":" << endl;

                printBoard(fout, solutions[i]);

                fout << endl;
            }

            fout << "Solutions found: "
                 << solutions.size()
                 << endl;
        }

        fout.close();

        cout << "Output written to "
             << outputFile
             << endl;

        return true;
    }
};

class SudokuApplication
{
public:
    void run()
    {
        string fileName;

        cout << "Enter puzzle filename: ";
        cin >> fileName;

        SudokuSolver solver;

        if (!solver.loadPuzzle(fileName))
        {
            return;
        }

        solver.solve();
        solver.writeResults();
    }
};

int main()
{
    SudokuApplication app;
    app.run();

    return 0;
}