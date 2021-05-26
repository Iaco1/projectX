#include "Menu.hpp"

//size is the no. elements in TileType
enum class TileType{ EMPTY, TERRAIN, HERO, ENEMY, BONUS, MALUS, SIZE };
const char *TileChar[]{ " ", "#", "H", "E", "B", "M"};

class Level{
    protected:
    TileType Board[WINDOW_HEIGHT][WINDOW_WIDTH];
    public:
    Level();                    //fills the board of TileType::EMPTY enums
    Level(int fillPercentage);  //fills fillPercentage % of the board randomly with TileTypes enums
    char *to_string();          //returns a char* representing how the map looks like
};