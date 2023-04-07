#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;

int col = 0, row = 0;
int bombNum = 0;
char** devBoard;
char** board;
char** gameBoard;
ifstream inFile;
ofstream outFile;
ifstream in;

void expandBoard(int y, int x)
{
    gameBoard[y][x] = '0';
    if (y - 1 >= 0 && board[y - 1][x] == '0')
    {
        expandBoard(y - 1, x);
    }
    if (y + 1 < col && board[y + 1][x] == '0')
    {
        expandBoard(y + 1, x);
    }
    if (x - 1 >= 0 && board[y][x - 1] == '0')
    {
        expandBoard(y, x - 1);
    }
    if (x + 1 < row && board[y][x + 1] == '0')
    {
        expandBoard(y, x + 1);
    }
    return;
}

void ClickBoard(int y, int x)
{
    gameBoard[y][x] = board[y][x];
    if (gameBoard[y][x] == 'X')
    {
        outFile << "Stepped on mine! Game end." << endl;
    }
    else if (gameBoard[y][x] == '0')
    {
        expandBoard(y, x);
    }
}

char checkSurroundMines(char** board, int y, int x)
{
    int mines = 0;
    if (board[y][x] == 'X') return 'X';
    for (int i = y - 1; i <= y + 1; i++)
    {
        if (i < 0 || i >= col) continue;
        for (int j = x - 1; j <= x + 1; j++)
        {
            if (j < 0 || j >= row) continue;
            if (board[i][j] == 'X') mines++;
        }
    }
    return mines + '0';
}

void printBoard(char** board)
{
    for (int i = 0; i < col; i++)
    {
        for (int j = 0; j < row; j++)
        {
            outFile << board[i][j] << " ";
        }
        outFile << endl;
    }
}

void makeBoard(ifstream &in)
{
    in >> col >> row;
    board = new char* [col];
    gameBoard = new char* [col];
    devBoard = new char* [col];
    for (int i = 0; i < col; i++)
    {
        board[i] = new char[row];
        gameBoard[i] = new char[row];
        devBoard[i] = new char[row];
    }
    for (int i = 0; i < col; i++)
    {
        for (int j = 0; j < row; j++)
        {
            char s;
            in >> s;
            if (s == 'X') bombNum++;
            devBoard[i][j] = s;
        }
    }
    for (int i = 0; i < col; i++)
    {
        for (int j = 0; j < row; j++)
        {
            board[i][j] = checkSurroundMines(devBoard, i, j);
            gameBoard[i][j] = '#';
        }
    }
}

void ConditionOutput(string seq, int flag, string additional)
{
    if (flag == 1)
    {
        outFile << "< " << seq << " > " << ": Success" << endl;
    }
    else if (flag == 0)
    {
        outFile << "< " << seq << " > " << ": Failed " << additional << endl;
    }
    else
    {
        outFile << "< " << seq << " > : " << additional << endl;
    }
}

int main(int argc, char* argv[])
{
    if (argc == 1)
    {
        //cin >> in >> out;
        inFile.open("input.txt");
        outFile.open("output.txt");
    }
    else if (argc == 4)
    {
        inFile.open(argv[2]);
        outFile.open(argv[3]);
    }
    else
    {
        cout << argc << " Error" << endl;
    }

    string method;
    string cOutput;
    string status = "Standby";
    bool hasLoaded = false;

    while (inFile >> method)
    {
        cOutput = method;

        if (method != "Load" && method != "StartGame" && method != "Print" && method != "LeftClick" && method != "RightClick" && method != "Replay" && method != "Quit")
        {
            ConditionOutput(cOutput, 0, ": No such method.");
            continue;
        }

        if (method == "Load")
        {
            string type;
            inFile >> type;
            cOutput += " " + type;
            if (type == "BoardFile")
            {
                string filename;
                inFile >> filename;
                ifstream in(filename);
                if (in.is_open())
                {
                    hasLoaded = true;
                    ConditionOutput(cOutput + " " + filename, 1, "");
                    makeBoard(in);
                }
                else
                {
                    ConditionOutput(cOutput, 0, "");
                }
            }
        }
        if (method == "Print")
        {
            string type;
            inFile >> type;
            cOutput += " " + type;
            if (type == "GameBoard")
            {
                if (status != "Playing")
                {
                    ConditionOutput(cOutput, 0, ": Not in playing state!");
                    continue;
                }
                ConditionOutput(cOutput, 2, "");
                printBoard(gameBoard);
            }
            if (type == "GameState")
            {
                ConditionOutput(cOutput, 2, status);
            }
            if (type == "GameAnswer")
            {
                if (status != "Playing")
                {
                    ConditionOutput(cOutput, 0, ": Not in playing state!");
                    continue;
                }
                ConditionOutput(cOutput, 2, "");
                printBoard(board);
            }
            if (type == "BombCount")
            {
                if (status != "Playing")
                {
                    ConditionOutput(cOutput, 0, ": Not in playing state!");
                    continue;
                }
                ConditionOutput(cOutput, 2, to_string(bombNum));
            }
        }
        if (method == "StartGame")
        {
            if (hasLoaded == false)
            {
                ConditionOutput(cOutput, 0, ": No board to play!");
                continue;
            }
            ConditionOutput(cOutput, 1, "");
            status = "Playing";
        }
        if (method == "LeftClick")
        {
            int y, x;
            inFile >> y >> x;
            cOutput += " " + to_string(y) + " " + to_string(x);
            if (status != "Playing")
            {
                ConditionOutput(cOutput, 0, ": Not in playing state!");
                continue;
            }
            ConditionOutput(cOutput, 1, "");
            ClickBoard(y, x);
        }
    }
}