#ifndef BOARD_H
#define BOARD_H
#include <array>
#include <string>
#include "Tile.h"
using namespace std;

constexpr int MINE {9};
constexpr int EMPTY_TILE {0};

class Board {
    // Creates a 1-D array using dynamic memory allocation
    // Initializes mine count
    private:
        int m_height;
        int m_width;
        int m_size;
        Tile *m_board {nullptr};
        int m_mine_count;
        int m_mine_revealed;
        bool m_gameStatus;
        bool m_gameWon;

    // Three functions to place the mines and to print the board with the mines laid
    public: 
        void PlaceMines(int first_row, int first_col);
        void UpdateCounts();
        void RevealedBoard(ostringstream &oss, const string &colorCode) const;
        void PlayBoard(int, int);                                  // A function to check the tile entered by the player
        bool CheckGameStatus() const { return m_gameStatus; }      // Returns game status in main
        bool CheckWin() const { return m_gameWon; }                // Check if the game has been won or not
        void FindEmptyTiles(int row, int col);
        void ToggleFlag(int row, int col);

        // Constructor
        Board();
        Board(int, int, int);                                       // width, height, and mine count
        // Destructor
        ~Board();                                                   // Deallocate memory for board
};

#endif