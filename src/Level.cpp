#include <iostream>
#include <cmath>
#include "Level.hpp"
#include "MiscFunctions.hpp"

Level::Level() {}
Level::Level(int w, int h, int levelIndex) {
    horBound = w;
    vertBound = h;

    //previous level door placement
    if (levelIndex > 0) prevLevelDoor = Object(1, h - 1, TileType::PL_DOOR);
    else prevLevelDoor = Object();

    //floor generation
    for (int i = 0; i <= w; i++) terrain.pushHead(new Node<Object>(Object(i, h, TileType::TERRAIN)));

    //platform generation
    generatePlatforms(vertBound - 4, horBound / 2, 0, horBound - 1, 1);

    //next level door placement
    generateNLDoor();

    //entities generation
    spawnEnemies(levelIndex);
    spawnBonuses(levelIndex);
    spawnMaluses(levelIndex);
    spawnXp();
}

LinkedList <Object>* Level::getTerrain() { return &terrain; }
LinkedList <Enemy>* Level::getEnemies() { return &enemies; }
LinkedList <Bonus>* Level::getBonuses() { return &bonuses; }
LinkedList <Malus>* Level::getMaluses() { return &maluses; }
LinkedList <Entity>* Level::getBullets() { return &bullets; }
LinkedList <Object>* Level::getXps() { return &xps; }

int Level::countObjectsAt(int x, int y) {
    return countObjectsAtIn(x, y, terrain)
        + countObjectsAtIn(x, y, enemies)
        + countObjectsAtIn(x, y, bonuses)
        + countObjectsAtIn(x, y, maluses)
        + countObjectsAtIn(x, y, bullets)
        + countObjectsAtIn(x, y, xps)
        + (prevLevelDoor.getX() == x && prevLevelDoor.getY() == y)
        + (nextLevelDoor.getX() == x && nextLevelDoor.getY() == y);
}

LinkedList<TileType> Level::getListOfTileTypesAt(int x, int y) {
    LinkedList<TileType> list;
    for (int i = 0; i < countObjectsAtIn(x, y, terrain); i++) list.pushHead(new Node<TileType>(TileType::TERRAIN));
    for (int i = 0; i < countObjectsAtIn(x, y, enemies); i++) list.pushHead(new Node<TileType>(TileType::ENEMY));
    for (int i = 0; i < countObjectsAtIn(x, y, bonuses); i++) list.pushHead(new Node<TileType>(TileType::BONUS));
    for (int i = 0; i < countObjectsAtIn(x, y, maluses); i++) list.pushHead(new Node<TileType>(TileType::MALUS));
    for (int i = 0; i < countObjectsAtIn(x, y, bullets); i++) list.pushHead(new Node<TileType>(TileType::BULLET));
    for (int i = 0; i < countObjectsAtIn(x, y, xps); i++) list.pushHead(new Node<TileType>(TileType::XP));

    if (prevLevelDoor.getX() == x && prevLevelDoor.getY() == y) list.pushHead(new Node<TileType>(TileType::PL_DOOR));
    if (nextLevelDoor.getX() == x && nextLevelDoor.getY() == y) list.pushHead(new Node<TileType>(TileType::NL_DOOR));

    return list;
}

Object Level::getPrevLevelDoor() { return prevLevelDoor; }
Object Level::getNextLevelDoor() { return nextLevelDoor; }
int Level::getHorBound() { return horBound; }
int Level::getVertBound() { return vertBound; }

bool Level::checkOverlap(int x1, int y1, int x2, int y2, TileType tile /*= TileType::EMPTY*/) {
    if (x1 < 0 || y1 < 0 || x2 > horBound || y2 > vertBound || x1 > x2 || y1 > y2) return true;
    if (tile == TileType::EMPTY || tile == TileType::TERRAIN) {     //iterates through the objects' lists
        Node<Object>* iter = terrain.getHead();                     //and checks for tiles between the specified coordinates
        while (iter != NULL) {
            if (iter->data.getX() >= x1 && iter->data.getX() <= x2 && iter->data.getY() >= y1 && iter->data.getY() <= y2)
                return true;
            iter = iter->next;
        }
    }
    if (tile == TileType::EMPTY || tile == TileType::HERO) {
        if (x1 <= 2 && x2 >= 2 && y1 <= vertBound - 1 && y2 >= vertBound - 1)   //checks for overlap in the empty spot where
            return true;                                                        //the hero is supposed to spawn (i.e. 2,h-1)
    }
    if (tile == TileType::EMPTY || tile == TileType::ENEMY) {
        Node<Enemy>* iter = enemies.getHead();
        while (iter != NULL) {
            if (iter->data.getX() >= x1 && iter->data.getX() <= x2 && iter->data.getY() >= y1 && iter->data.getY() <= y2)
                return true;
            iter = iter->next;
        }
    }
    if (tile == TileType::EMPTY || tile == TileType::BONUS) {
        Node<Bonus>* iter = bonuses.getHead();
        while (iter != NULL) {
            if (iter->data.getX() >= x1 && iter->data.getX() <= x2 && iter->data.getY() >= y1 && iter->data.getY() <= y2)
                return true;
            iter = iter->next;
        }
    }
    if (tile == TileType::EMPTY || tile == TileType::MALUS) {
        Node<Malus>* iter = maluses.getHead();
        while (iter != NULL) {
            if (iter->data.getX() >= x1 && iter->data.getX() <= x2 && iter->data.getY() >= y1 && iter->data.getY() <= y2)
                return true;
            iter = iter->next;
        }
    }
    if (tile == TileType::EMPTY || tile == TileType::BULLET) {
        Node<Entity>* iter = bullets.getHead();
        while (iter != NULL) {
            if (iter->data.getX() >= x1 && iter->data.getX() <= x2 && iter->data.getY() >= y1 && iter->data.getY() <= y2)
                return true;
            iter = iter->next;
        }
    }
    if (tile == TileType::EMPTY || tile == TileType::PL_DOOR) {
        if (prevLevelDoor.getX() + 1 >= x1 && prevLevelDoor.getX() <= x2 && prevLevelDoor.getY() >= y1 && prevLevelDoor.getY() <= y2)
            return true;
    }
    if (tile == TileType::EMPTY || tile == TileType::NL_DOOR) {
        if (nextLevelDoor.getX() >= x1 && nextLevelDoor.getX() - 1 <= x2 && nextLevelDoor.getY() >= y1 && nextLevelDoor.getY() <= y2)
            return true;
    }
    if (tile == TileType::EMPTY || tile == TileType::XP) {
        Node<Object>* iter = xps.getHead();
        while (iter != NULL) {
            if (iter->data.getX() >= x1 && iter->data.getX() <= x2 && iter->data.getY() >= y1 && iter->data.getY() <= y2)
                return true;
            iter = iter->next;
        }
    }
    return false;
}

void Level::placePlatform(int height, int leftBound, int rightBound) {
    for (int i = leftBound; i <= rightBound; i++) {
        terrain.pushHead(new Node<Object>(Object(i, height, TileType::TERRAIN)));
    }
}

int Level::findClosestTerrain(int height, int xPosition, bool left) {
    int l = -1, r = horBound + 1;
    Node<Object>* iter = terrain.getHead();
    while (iter != NULL) {
        if (iter->data.getY() >= height - 2 && iter->data.getY() <= height + 2) {
            if (iter->data.getX() < xPosition && iter->data.getX() > l) l = iter->data.getX();
            if (iter->data.getX() > xPosition && iter->data.getX() < r) r = iter->data.getX();
        }
        iter = iter->next;
    }
    if (left) return l;
    else return r;
}

void Level::generatePlatforms(int height, int averageXPosition, int leftBound, int rightBound, int currentIteration) {
    if (leftBound > rightBound || leftBound < 0 || rightBound > horBound || height < 0 || height > vertBound) return;
    int platformLength = Misc::diceDistribution(2, min((horBound + 1) / 4, rightBound - leftBound + 1), 5, round(2 + currentIteration / 3));    //randomization of platform's length
    int minOffset = max(0, averageXPosition - leftBound - platformLength + 1);                          //calculates maximum and minimum possible offset of platform
    int maxOffset = min(averageXPosition - leftBound, rightBound - leftBound - platformLength + 1);
    int platformOffset = Misc::randInt(minOffset, maxOffset);                                           //randomization of platform's offset
    int xPos1 = leftBound + platformOffset;
    int xPos2 = xPos1 + platformLength - 1;
    placePlatform(height, xPos1, xPos2);

    double generateChance = 0.7 - 0.05 * currentIteration;  //chance to generate a child platform; decreases with iterations

    int order[6] = { 1, 2, 3, 4, 5, 6 };    //random order in which child platforms are generated
    Misc::shuffle(order, 6);                //new platforms can be generated up-left, up-right, down-left, down-right, left and right of the original platform

    for (int i = 0; i < 6; i++) {

        switch (i) {
        case 1:
            //new up-left platform
            if (Misc::randBool(generateChance)) {
                int hDiff = Misc::randInt(3, 4);
                if (!checkOverlap(xPos1 - 2, height - hDiff - 2, xPos1 + 2, height - hDiff + 2) && height - hDiff >= 3) {
                    generatePlatforms(height - hDiff, xPos1, findClosestTerrain(height - hDiff, xPos1, true) + 2, min(findClosestTerrain(height - hDiff, xPos1, false) - 2, xPos2 - 1), currentIteration + 1);
                }
            }
            break;

        case 2:
            //new up-right platform
            if (Misc::randBool(generateChance)) {
                int hDiff = Misc::randInt(3, 4);
                if (!checkOverlap(xPos2 - 2, height - hDiff - 2, xPos2 + 2, height - hDiff + 2) && height - hDiff >= 3) {
                    generatePlatforms(height - hDiff, xPos2, max(findClosestTerrain(height - hDiff, xPos2, true) + 2, xPos1 + 1), findClosestTerrain(height - hDiff, xPos2, false) - 2, currentIteration + 1);
                }
            }
            break;

        case 3:
            //new left platform
            if (Misc::randBool(generateChance)) {
                int hDiff = Misc::randInt(-1, 1);
                int dist = Misc::randInt(2, 3);
                if (!checkOverlap(xPos1 - dist - 4, height + hDiff - 2, xPos1 - dist, height + hDiff + 2) && height + hDiff >= 3) {
                    generatePlatforms(height + hDiff, xPos1 - dist, findClosestTerrain(height + hDiff, xPos1 - dist, true) + 2, xPos1 - dist, currentIteration + 1);
                }
            }
            break;

        case 4:
            //new right platform
            if (Misc::randBool(generateChance)) {
                int hDiff = Misc::randInt(-1, 1);
                int dist = Misc::randInt(2, 3);
                if (!checkOverlap(xPos2 + dist, height + hDiff - 2, xPos2 + dist + 4, height + hDiff + 2) && height + hDiff >= 3) {
                    generatePlatforms(height + hDiff, xPos2 + dist, xPos2 + dist, findClosestTerrain(height + hDiff, xPos2 + dist, false) - 2, currentIteration + 1);
                }
            }
            break;

        case 5:
            //new down-left platform
            if (Misc::randBool(generateChance)) {
                int hDiff = Misc::randInt(3, 4);
                if (!checkOverlap(xPos1 - 2, height + hDiff - 2, xPos1 + 2, height + hDiff + 2)) {
                    generatePlatforms(height + hDiff, xPos1, findClosestTerrain(height + hDiff, xPos1, true) + 2, findClosestTerrain(height + hDiff, xPos1, false) - 2, currentIteration + 1);
                }
            }
            break;

        case 6:
            //new down-right platform
            if (Misc::randBool(generateChance)) {
                int hDiff = Misc::randInt(3, 4);
                if (!checkOverlap(xPos2 - 2, height + hDiff - 2, xPos2 + 2, height + hDiff + 2)) {
                    generatePlatforms(height + hDiff, xPos2, findClosestTerrain(height + hDiff, xPos2, true) + 2, findClosestTerrain(height + hDiff, xPos2 + 2, false) - 2, currentIteration + 1);
                }
            }
            break;
        }

    }
}

void Level::generateNLDoor() {
    int destX = horBound, destY = vertBound + 1;
    for (int i = 1; i <= vertBound; i++) {              //searches for the highest terrain in the right half of the map and places the door there
        if (checkOverlap(horBound / 2 + 1, i, horBound, i, TileType::TERRAIN)) {
            destY = i - 1;
            destX = findClosestTerrain(i - 2, horBound + 1, true);
            break;
        }
    }
    nextLevelDoor = Object(destX, destY, TileType::NL_DOOR);
}

void Level::findFreeSpace(int& x, int& y, int offset) {
    x = -1;
    y = -1;
    int terrSize = terrain.getSize();               //searches for a random terrain tile
    int index = Misc::randInt(0, terrSize - 1);     //if the spot above said terrain is already occupied, it looks at the next terrain in the list
    Node<Object>* iter = terrain.getHead();
    for (int i = 0; i < index; i++) iter = iter->next;
    for (int i = 0; i < terrSize; i++) {
        int xIter = iter->data.getX(), yIter = iter->data.getY();
        if (!checkOverlap(xIter - 1, yIter - 1 - offset, xIter + 1, yIter - 1)) {
            x = xIter;
            y = yIter - 1 - offset;
            break;
        }
        if (iter->next == NULL) {
            iter = terrain.getHead();
        }
        else {
            iter = iter->next;
        }
    }
}

void Level::spawnEnemies(int currentLevel) {
    int enemiesNum = Misc::diceDistribution(3, Misc::bound(currentLevel + 5, 6, 20), Misc::bound(currentLevel + 5, 6, 20) / 2, 3);  //randomly generated number of enemies
    for (int i = 0; i < enemiesNum; i++) {                                                                                          //number increases with level
        int x, y;
        findFreeSpace(x, y);
        if (x == -1 || y == -1) break;
        enemies.pushHead(new Node<Enemy>(Enemy(x, y, Misc::randInt(100, 150), 30, (EnemyType)Misc::randInt(0, 1))));    //enemy strength is randomized
    }
}

void Level::spawnBonuses(int currentLevel) {
    int bonusNum = Misc::diceDistribution(Misc::bound(10 - currentLevel, 3, 7), 10, Misc::bound(10 - currentLevel, 3, 7) / 2, 3);
    for (int i = 0; i < bonusNum; i++) {
        int x, y;
        findFreeSpace(x, y, Misc::randInt(0, 2));
        if (x == -1 || y == -1) break;
        BonusType typeOfBonus = (BonusType)Misc::bound(Misc::diceDistribution(0, 4, 0, 2), 0, 3);
        switch (typeOfBonus) {
        case BonusType::HP:
            bonuses.pushHead(new Node<Bonus>(Bonus(x, y, 100, BonusType::HP, 100)));
            break;
        case BonusType::AMMO:
            bonuses.pushHead(new Node<Bonus>(Bonus(x, y, 100, BonusType::AMMO, 10)));
            break;
        case BonusType::MAXAMMO:
            bonuses.pushHead(new Node<Bonus>(Bonus(x, y, 100, BonusType::MAXAMMO, 100)));
            break;
        case BonusType::INSTAKILL:
            bonuses.pushHead(new Node<Bonus>(Bonus(x, y, 100, BonusType::INSTAKILL, 1)));
            break;    
        default:
            break;
        }
    }
}

void Level::spawnMaluses(int currentLevel) {
    int malusNum = Misc::diceDistribution(2, Misc::bound(currentLevel + 3, 3, 7), Misc::bound(currentLevel + 3, 3, 7) / 2, 3);
    for (int i = 0; i < malusNum; i++) {
        int x, y;
        findFreeSpace(x, y);
        if (x == -1 || y == -1) break;
        MalusType typeOfMalus = (MalusType)Misc::randInt(0, 2);
        switch (typeOfMalus) {
        case MalusType::THORN:
            maluses.pushHead(new Node<Malus>(Malus(x, y, 100, 5, MalusType::THORN, 10)));
            break;
        case MalusType::LANDMINE:
            maluses.pushHead(new Node<Malus>(Malus(x, y, 100, 50, MalusType::LANDMINE, 1)));
            break;
        case MalusType::BARBED_WIRE:
            maluses.pushHead(new Node<Malus>(Malus(x, y, 100, 1, MalusType::BARBED_WIRE, 20)));
            if (!checkOverlap(x, y - 1, x, y - 1)) {
                maluses.pushHead(new Node<Malus>(Malus(x, y - 1, 100, 1, MalusType::BARBED_WIRE, 20)));
                if (!checkOverlap(x, y - 2, x, y - 2)) {
                    maluses.pushHead(new Node<Malus>(Malus(x, y - 2, 100, 1, MalusType::BARBED_WIRE, 20)));
                }
            }
            break;
        default:
            break;
        }
    }
}

void Level::spawnXp() {
    for (int i = 0; i < 3; i++) {
        int x, y;
        findFreeSpace(x, y);
        if (x == -1 || y == -1) break;
        xps.pushHead(new Node<Object>(Object(x, y, TileType::XP)));
    }
}