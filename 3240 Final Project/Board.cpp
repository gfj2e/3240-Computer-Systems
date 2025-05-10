#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <array>
#include "Board.h"
#include "Tile.h"

Board::Board() {                                            // A default constructor for a Board object
    m_width = 8;            
    m_height = 8;
    m_size = m_width * m_height;
    m_mine_count = 10;
    m_mine_revealed = 0;
    m_gameStatus = true;
    m_gameWon = false;
    m_board = new Tile[m_size];
    for(int row {0}; row < m_size; row++) {
        m_board[row].SetValue(EMPTY_TILE);
    }
}

Board::Board(int height, int width, int mine_count) {       // Constructor that takes in width, height, and mine count from user
    m_height = height;                                      // Calculates size of the board and allocates memory for it using pointer
    m_width = width;
    m_size = width * height;
    m_mine_count = mine_count;
    m_mine_revealed = 0;
    m_gameStatus = true;                                    // game status checks whether game is still running
    m_gameWon = false;                                      // When the game stops running, the code will check this variable to see if the player won the game or not
    m_board = new Tile[m_size];
    for (int row {0}; row < m_size; row++) {
        m_board[row].SetValue(EMPTY_TILE);                  // Seeds every tile with value 0 by default
    }
}

void Board::PlaceMines(int first_row, int first_col) {
    // This function places down the mines using c random functions
    // mine keeps track of total mines placed until it reaches the mine count
    srand(time(NULL));
    for (int i {0}; i < m_mine_count && i < m_size; i++) {  // and here checks to allows the player to place more mines than the board size can hold, ex: 5 5 30
        int mine_row = rand() % m_height;                   // Selects random index values based on size of the grid, places a mine into that index if there isn't one
        int mine_col = rand() % m_width;                    // and places a mine into that index if there isn't one

        if ((mine_row == first_row && mine_col == first_col) 
        || m_board[mine_row * m_width + mine_col].GetValue() == MINE) {
            i--;
            continue;
        }

        m_board[mine_row * m_width + mine_col].SetValue(MINE);
    }
}

void Board::UpdateCounts() {
    for (int row {0}; row < m_height; row++) {
        for (int col {0}; col < m_width; col++) {
            // Checks to see if there are mines adjacent to the current index tile and increments tile value by 1 each time, bounds checking
            if (m_board[m_width * row + col].GetValue() != MINE) {
                if (row != 0 && m_board[m_width * (row - 1) + col].GetValue() == MINE) {
                    m_board[m_width * row + col].AddValue();
                }
                if (row != (m_height - 1) && m_board[m_width * (row + 1) + col].GetValue() == MINE) {
                    m_board[m_width * row + col].AddValue();
                }
                if (col != (m_width - 1) && m_board[m_width * row + (col + 1)].GetValue() == MINE) {
                    m_board[m_width * row + col].AddValue();
                }
                if (col != 0 && m_board[m_width * row + (col - 1)].GetValue() == MINE) {
                    m_board[m_width * row + col].AddValue();
                }
                if (row != 0 && col != 0 && m_board[m_width * (row - 1) + (col - 1)].GetValue() == MINE) {
                    m_board[m_width * row + col].AddValue();
                }
                if (row != (m_height - 1) && col != m_width - 1 && m_board[m_width * (row + 1) + (col + 1)].GetValue() == MINE) {
                    m_board[m_width * row + col].AddValue();
                }
                if (row != 0 && col != (m_width - 1) && m_board[m_width * (row - 1) + (col + 1)].GetValue() == MINE) {
                    m_board[m_width * row + col].AddValue();
                }
                if (row != (m_height - 1) && col != 0 && m_board[m_width * (row + 1) + (col - 1)].GetValue() == MINE) {
                    m_board[m_width * row + col].AddValue();
                }
            }
        }
    }
}

void Board::PlayBoard(int row, int col) {
    int index = m_width * row + col;

    // If it's a mine, reveal it and end the game
    if (m_board[index].GetValue() == MINE) {
        m_board[index].SetStatusTrue();
        m_gameStatus = false;
        return;
    }

    // If it's a zero, use recursive reveal
    if (m_board[index].GetValue() == EMPTY_TILE) {
        FindEmptyTiles(row, col);  // Handles revealing and count
    }

    // If it's a number tile and not yet revealed
    else if (!m_board[index].GetStatus()) {
        m_board[index].SetStatusTrue();
        m_mine_revealed++;
    }

    // Check for win condition
    if (m_mine_revealed == (m_width * m_height - m_mine_count)) {
        m_gameStatus = false;
        m_gameWon = true;
    }
}


void Board::FindEmptyTiles(int row, int col) {
    if (row < 0 || col < 0 || row >= m_height || col >= m_width)
        return;

    Tile &tile = m_board[m_width * row + col];

    if (tile.GetStatus() || tile.GetValue() == MINE)
        return;

    tile.SetStatusTrue();
    m_mine_revealed++;

    if (tile.GetValue() != EMPTY_TILE)
        return;

    // Recursively reveal all 8 neighbors
    FindEmptyTiles(row - 1, col);     // Up
    FindEmptyTiles(row + 1, col);     // Down
    FindEmptyTiles(row, col - 1);     // Left
    FindEmptyTiles(row, col + 1);     // Right
    FindEmptyTiles(row - 1, col - 1); // Top-left
    FindEmptyTiles(row - 1, col + 1); // Top-right
    FindEmptyTiles(row + 1, col - 1); // Bottom-left
    FindEmptyTiles(row + 1, col + 1); // Bottom-right
}

void Board::ToggleFlag(int row, int col) {
    if (row >= 0 && row < m_height && col >= 0 && col < m_width) {
        m_board[m_width * row + col].FlagTile();
    }
}

void Board::RevealedBoard(ostringstream &oss, const string &colorCode) const {
    // This function prints the board in grid format after mines are placed
    oss << colorCode << setw(5) << "  ";
    for (int col_num {0}; col_num < m_width; col_num++) {
        if (col_num < 10) {
            oss << col_num << "  ";
        
        }
        oss << setw(2);
        if (col_num > 9) {
            oss << col_num << "  ";
        }
    }
    oss << endl;
    for (int row {0}; row < m_height; row++) {
        oss << "   ";
        for (int lines {0}; lines < m_width; lines++) {
            oss << "|---";
        }
        oss << "|";
        oss << endl;
        if (row > 9) {
            oss << row << " "; 
        } else {
            oss << row << "  ";
        }
        if (!m_gameStatus && !m_gameWon) {                            // If the player loses the game
            for (int col {0}; col < m_width; col++) {
                if (m_board[m_width * row + col].GetValue() == MINE) {  // If there is a mine, print M in place of it
                    oss << "| " << 'M' << " ";
                }
                else {
                    oss << "| " << m_board[m_width * row + col].GetValue() << " ";
                }
            }
            oss << "|";
            oss << endl;
        }
        if (m_gameStatus && !m_gameWon) {                                                   // If game is running but not won yet
            for (int col {0}; col < m_width; col++) {
                if (!m_board[m_width * row + col].GetStatus() && !m_board[m_width * row + col].IsFlagged()) {  
                    oss << "| " << '#' << " ";                                        // Print out hidden tile if tile not revealed
                } else if (!m_board[m_width * row + col].GetStatus() && m_board[m_width * row + col].IsFlagged()) {
                    oss << "| " << 'F' << " "; 
                } else {
                    oss << "| " << m_board[m_width * row + col].GetValue() << " ";    // If revealed, print the value
                }
            }
            oss << "|";
            oss << endl;
        }
        if (!m_gameStatus && m_gameWon) {                                                  // If the game is won
            for (int col {0}; col < m_width; col++) {
                if (!m_board[m_width * row + col].GetStatus()) {  
                    oss << "| " << '#' << " ";                                      // Print out the mines still hidden
                }
                else {
                    oss << "| " << m_board[m_width * row + col].GetValue() << " ";  // Print tiles revealed
                }
            }
            oss << "|";
            oss << endl;
        }
    }

    oss << setw(3) << "   ";
    for(int lines {0}; lines < m_width; lines++) {
            oss << "|---";
        }
    oss << "|" << endl;
}

Board::~Board() {
    delete[] m_board; // Deallocates memory from the heap
    m_board = nullptr;
}