#include <iostream>
#include <ctime>
using namespace std;

class Game {  // Abstract class 
public:
    virtual void play() = 0; // Pure virtual function 
    virtual ~Game() {} // Virtual destructor 
};

class Cell {   // Class representing a single cell
    int value; // -1 for mine, 0 for empty, positive number for number of neighboring mines
    int status; // 0 for hidden , 1 for revealed , 2 for flagged
public:
    Cell() {
        value = 0;
        status = 0;
    }
    void setValue(int val) {
        value = val;
    }
    int getValue() const {
        return value;
    }
    void setStatus(int s) {
        status = s;
    }
    int getStatus() const {
        return status;
    }
};
class Coordinate {
public:
    int row;
    int col;
    Coordinate(){
        row = 0;
        col = 0;
    }
    Coordinate(int r, int c) {
        row = r;
        col = c;
    }
};
class Minesweeper : public Game {
    int rows;
    int cols;
    int totalMines;
    Cell** board;
    Coordinate* mines;
    bool gameOver;
    time_t startTime;
    int time_passed;

public:
    Minesweeper(const int r, const int c, const int minesCount) {
        rows = r;
        cols = c;
        totalMines = minesCount;
        gameOver = false;
        time_passed = 0;
        // Initialize board
        board = new Cell * [rows];
        for (int i = 0; i < rows; i++) {
            board[i] = new Cell[cols];
        }
        mines = new Coordinate[minesCount];
        placeMines();    // Place mines randomly
        calculateNumbers();  // Calculate numbers around mines
        startTime = time(NULL);  // Start timer
    }

    ~Minesweeper() {
        for (int i = 0; i < rows; i++) {
            delete[] board[i];
        }
        delete[] board;
        delete[] mines;
    }

    // Overridden function from Game class
    void play() override {
        cout << "Total Mines: " << totalMines << endl;
        while (!gameOver) {
            display();
            updateTimer();
            int r, c;
            char action;
            cout << "Enter row number" << endl;
            cin >> r;
            cout << "Enter column number" << endl;
            cin >> c;
            cout << "Enter L for exposing the cell and R for flagging it " << endl;
            cin >> action;

            if (action == 'L') {
                if (isMine(r, c)) {
                    revealCell(r, c);
                    gamelost();
                    break;
                }
                else {
                    revealCell(r, c);
                    if (isGameWon()) gamewon();
                }
            }
            else if (action == 'R') {
                flagCell(r, c);
                break;
            }
        }
    }

    void placeMines() {
        srand(time(NULL));
        int minesPlaced = 0;
        while (minesPlaced < totalMines) {
            int r = rand() % rows;
            int c = rand() % cols;
            if (board[r][c].getValue() != -1) {
                board[r][c].setValue(-1);
                mines[minesPlaced] = Coordinate(r, c);
                minesPlaced++;
            }
        }
    }

    void calculateNumbers() {
        for (int i = 0; i < totalMines; i++) {
            int r = mines[i].row;
            int c = mines[i].col;
            for (int j = -1; j <= 1; j++) {
                for (int k = -1; k <= 1; k++) {
                    if (j == 0 && k == 0) 
                    continue;
                    int newr = r + j;
                    int newc = c + k;
                    if (isValid(newr, newc) && board[newr][newc].getValue() != -1) {
                        board[newr][newc].setValue(board[newr][newc].getValue() + 1);
                    }
                }
            }
        }
    }

    bool isValid(int r, int c) {
        return r >= 0 && r < rows && c >= 0 && c < cols;
    }

    void display() {
        cout << "  ";
        for (int c = 0; c < cols; c++) {
            cout << c << " ";
        }
        cout << "\n";
        for (int r = 0; r < rows; r++) {
            cout << r << " ";
            for (int c = 0; c < cols; c++) {
                if (board[r][c].getStatus() == 1) {
                    if (board[r][c].getValue() == -1) cout << "* ";
                    else if (board[r][c].getValue() == 0) cout << ". ";
                    else cout << board[r][c].getValue() << " ";
                }
                else if (board[r][c].getStatus() == 2) {
                    cout << "F ";
                }
                else {
                    cout << "# ";
                }
            }
            cout << "\n";
        }
        cout << "\n";
        cout << "Time: " << time_passed << "s\n";
    }

    void updateTimer() {
        time_t currentTime = time(NULL);
        time_passed = currentTime - startTime;
    }

    void revealCell(int r, int c) {
        if (!isValid(r, c) || board[r][c].getStatus() == 1) // if out of bound or 
            return;                                         // already revealed then return

        board[r][c].setStatus(1);
        if (board[r][c].getValue() == 0) {
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    revealCell(r + i, c + j);
                }
            }
        }
    }

    void flagCell(int r, int c) {
        if (!isValid(r, c) || board[r][c].getStatus() == 1) // if out of bound or 
            return;                                         // already revealed then return

        if (board[r][c].getStatus() == 0) {
            board[r][c].setStatus(2);
        }
        else {
            board[r][c].setStatus(0);
        }
    }

    bool isGameWon() {
        for (int i = 0; i < totalMines; i++) {
            int r = mines[i].row;
            int c = mines[i].col;
            if (board[r][c].getStatus() != 2)
                return false;
        }
        return true;
    }

    bool isMine(int r, int c) {
        return isValid(r, c) && board[r][c].getValue() == -1;
    }

    void gamelost() {
        gameOver = true;
        display();
        cout << "Game Over! You have hit a mine.\n";
    }

    void gamewon() {
        gameOver = true;
        updateTimer();
        display();
        cout << "Congratulations! You won!\n";
        // Check if it's a high score and update if necessary
        // Save high scores to file
    }
};
int main() {
    int DifficultyLevel, Mines, rows, cols;
    cout << "Welcome to the Minesweeper Game!!" << endl;
    cout << "Enter the difficulty level ( 1 for easy , 2 for intermediate , 3 for advance ) " << endl;
    cin >> DifficultyLevel;
    if (DifficultyLevel == 1) {
        rows = 10;
        cols = 10;
        Mines = 10;
    }
    else if (DifficultyLevel == 2) {
        rows = 20;
        cols = 20;
        Mines = 40;
    }

    else if (DifficultyLevel == 3) {
        rows = 30;
        cols = 30;
        Mines = 99;
    }
    Minesweeper player(rows, cols, Mines);
    player.play();

    return 0;
}


















