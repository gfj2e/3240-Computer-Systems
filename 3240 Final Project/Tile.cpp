#include "Tile.h"

Tile::Tile() {
    m_isRevealed = false;
    m_isFlagged = false;
    m_value = 0;
}

int Tile::GetValue() const {  // Return value
    return m_value;
}

void Tile::SetValue(int new_val) { // Set to specific value
    m_value = new_val;
}

void Tile::AddValue() { // Add one to value if there is a mine
    m_value = m_value + 1;
}

void Tile::SetStatusTrue() { // Reveal the tile
    m_isRevealed = true;
}

void Tile::FlagTile() {
    if (!m_isRevealed) {
        m_isFlagged = !m_isFlagged;
    }
}