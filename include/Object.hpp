#pragma once

enum class TileType{ EMPTY, TERRAIN, HERO, ENEMY, BONUS, MALUS, BULLET, PL_DOOR, NL_DOOR, XP, SIZE};
/*
base class for all in-game objects
*/
class Object{
    protected:
    int x, y;
    TileType tileType;
    
    public:
    Object();
    Object(int x, int y, TileType tileType);
    Object(bool random); //creates a random object
    //void toString(char str[24]);
    void getXY(int &x, int &y);
    int getX();
    int getY();
    char getTileChar();
    void setXY(int x, int y);
    TileType getTileType();
};