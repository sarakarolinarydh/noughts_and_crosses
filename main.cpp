#include <vector>
#include <queue>
#include <iostream>
#include <string>
#include <regex>

using namespace std;

void print_chessboard();
bool chess_move_input();
bool chess_move_ok(int pos);
bool chess_move_advanced_rules_ok(int pos);
bool winner(char c);

string player[] = {"BLUE", "RED"};
int turn = 0;
int pos = 0;

regex pattern("^([ABCabc][1-3])$");

std::vector<char> board{'-', '-', '-', '-', '-', '-', '-', '-', '-'};
std::queue<int> chess_pos[2];

void print_chessboard()
{
    cout << "   :::::::::::::\n";
    cout << " A :: " << board[0] << "  " << board[1] << "  " << board[2] << " ::\n"; // 1 2 3
    cout << " B :: " << board[3] << "  " << board[4] << "  " << board[5] << " ::\n"; // 4 5 6
    cout << " C :: " << board[6] << "  " << board[7] << "  " << board[8] << " ::\n"; // 7 8 9
    cout << "   :::::::::::::\n";
    cout << "     1   2   3\n";
}

bool chess_move_input()
{
    string move_input;
    cout << "Enter your move: ";
    cin >> move_input;

    // Shortcut to end the program.
    if (move_input[0] == '9')
    {
        exit(0);
    }

    if (!regex_match(move_input, pattern))
    {
        return false;
    }

    switch (move_input[0])
    {
    case 'A':
    case 'a':
        pos = 0;
        break;
    case 'B':
    case 'b':
        pos = 3;
        break;
    case 'C':
    case 'c':
        pos = 6;
    }

    switch (move_input[1])
    {
    case '1':
        pos = pos + 0;
        break;
    case '2':
        pos = pos + 1;
        break;
    case '3':
        pos = pos + 2;
    }

    return true;
}

bool chess_move_ok(int pos)
{
    if (board[pos] == '-')
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool chess_move_update_queue_and_board(int pos)
{
    // If three chess pieces are already placed, remove the oldest placed piece.
    if(chess_pos[turn].size() == 3)
    {
        board[chess_pos[turn].front()] = '-';
        chess_pos[turn].pop();
    }

    chess_pos[turn].push(pos);
    return true;
}

bool winner(char c)
{
    // cout << "Horizontally: " << endl;
    for(int i = 0; i < 7; i = i + 3)
    {
        if (board[i] == c && board[i+1] == c && board[i+2] == c)
        {
            return true;
        }
    }
    // cout << "Vertically: " << endl;
    for(int i = 0; i < 3; i++)
    {

        if (board[i] == c && board[i+3] == c && board[i+6] == c)
        {
            return true;
        }
    }

    // cout << "Diagonally: \n";
    if (board[0] == c && board[4] == c && board[8] == c)
    {
        return true;
    }

    if (board[2] == c && board[4] == c && board[6] == c)
    {
        return true;
    }

    return false;   
}

int main()
{
    bool pos_input = 0;
    bool pos_ok = 0;

    cout << "\n\n*******************\nNOUGHTS AND CROSSES\n      WELCOME!\n*******************\n\n";
    print_chessboard();
    cout << "\n BLUE COULOR STARTS\n";
    cout << "\n*******************\n\n";

    for (;;)
    {
        // Print player and the chessboard.
        cout << "        " << player[turn] << "\n";
        print_chessboard();
        cout << "\n";

        // Wait for the player to input a valid chess move.
        while (!chess_move_input())
        {
        }

        while (!chess_move_ok(pos))
        {
            while (!chess_move_input())
            {
            }
        }

        // Only three chess pieces at the time is aloud, remove the oldest placed piece.
        chess_move_update_queue_and_board(pos);

        // Place the chess piece.
        board[pos] = tolower(player[turn][0]); // The first letter in the current players name (blue, red).
        cout << "*******************\n\n";

        // Check if there is a winner.
        if (winner(tolower(player[turn][0])))
        {
            break;
        }

        // Next player to take turn.
        turn = 1 - turn;
    }

    // Print game over and the winner.
    print_chessboard();
    cout << "\n\n*******************\n\n";
    cout << "\n     " << "GAME OVER\n\n";

    cout << "\n   " << "THE WINNER IS";
    cout << "\n       " << player[turn] << "\n\n";
    cout << "\n*******************\n\n";

    return 0;
}