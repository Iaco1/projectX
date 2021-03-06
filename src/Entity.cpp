#include "Entity.hpp"
#include <cstdio>

Entity::Entity() : Item(){}
Entity::Entity(int x, int y, TileType tileType, int hp, int dp, Direction direction) : Item(x,y,tileType, hp, dp){
    //initialization for the actionLog with unsignificant Actions 
    for(int i=0; i<SIGNIFICANT_MOVES; i++){
        actionLog[i] = Action(Animation::STILL, 0,0, Initiator::LOGIC, TileType::EMPTY, 0);
    }
    
    this->direction = direction;
}

bool Entity::isMovementAction(Action action){
    switch(action.getAnimation()){
        case Animation::CLIMB_DOWN:
        case Animation::CLIMB_UP:
        case Animation::FALLING:
        case Animation::JUMPING:
        case Animation::LEFT:
        case Animation::RIGHT:
        case Animation::STILL:
        return true;
        break;

        default:
        return false;
    }
}

//registers last SIGNIFICANT_MOVES actions 
//by inserting the last one in actionLog[0] and deleting the oldest one in actionLog[SIGNIFICANT_MOVES-1]
void Entity::registerMove(Action action){
    for(int i = SIGNIFICANT_MOVES-1; i>0; i--){
        actionLog[i] = actionLog[i-1]; 
    }
    actionLog[0] = action;

    
    if(action.getInitiator() == Initiator::USER){
        if(action.getAnimation() == Animation::RIGHT) direction = Direction::RIGHT;
        else if(action.getAnimation() == Animation::LEFT) direction = Direction::LEFT;
    }
}

//counts the no. of nodes that have matching data (animation and initiator)
int Entity::countMoves(Action action){
	int count = 0;
	for(int i=0; i<SIGNIFICANT_MOVES-1; i++){
        if(actionLog[i].getAnimation() == action.getAnimation() && actionLog[i].getInitiator() == action.getInitiator()) count++;
    }
	return count;
}

//counts the no. of nodes that have matching data (animation) in the last n positions of actionLog[]
int Entity::countMoves(Animation animation, int n){
	int count = 0;
    if(n>SIGNIFICANT_MOVES || n<=0) n = SIGNIFICANT_MOVES;

	for(int i=0; i < n-1; i++){
        if(actionLog[i].getAnimation() == animation) count++;
    }
	return count;
}

//counts the no. of nodes that have matching data (animation and initiator) in the last n positions of actionLog[]
int Entity::countMoves(Animation animation, Initiator initiator, int n){
	int count = 0;
    if(n>SIGNIFICANT_MOVES || n<=0) n = SIGNIFICANT_MOVES;
    
	for(int i=0; i < n-1; i++){
        if(actionLog[i].getAnimation() == animation && actionLog[i].getInitiator() == initiator) count++;
    }
	return count;
}

Action* Entity::getActionLog(){
    Action* p = actionLog;
    return p;
}

Direction Entity::getDirection(){ return direction; }

