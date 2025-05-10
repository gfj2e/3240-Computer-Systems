#ifndef TILE_H
#define TILE_H
/* Creates Tile class that will be contained within a board object
   Each tile will be it's own object that contains a value
*/
class Tile {
    private:
        int m_value;          // The value of the tile
        bool m_isRevealed;    // bool to check if the mine has been revealed
        bool m_isFlagged;
    public:
        Tile();
        void SetValue(int);                             // Setter
        void AddValue();                                // Another setter
        int GetValue() const;                           // Getter
        void SetStatusTrue();                           // function to reveal a tile, sets bool to true
        void FlagTile();
        bool GetStatus() const { return m_isRevealed; } // Return status of the boolean m_isRevealed
        bool IsFlagged() const { return m_isFlagged; }  // Return whether tile is flagged or not
};

#endif