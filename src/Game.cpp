#include "Game.hpp"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

const int JUMP_HEIGHT = 4;
const int targetFrameRate = 60;
auto frameDelay =  std::chrono::seconds(1/targetFrameRate); //max duration for a frame


Game::Game() {
	gameOver = false;
	score = 0;
}

void Game::createMap(){
	//here we genereate levels and the hero
	int h, w;
	getmaxyx(levelWindow, h,w);
	h--;w--;

    map.pushHead(new Node<Level>(Level(w,h)));
	hero = Hero(1,h-1,100,50);
    currentLevel = 0;
}

void Game::draw(bool newLevel){
	if(newLevel) {
		clear();
		refresh();
	}
	int HUD_h = 1;
	WINDOW* hud = newwin(HUD_h, getmaxx(levelWindow), 0, 0);
	drawHUD(hud);
	delwin(hud);
	
	//hero drawing
	drawHero();
	wrefresh(levelWindow);
	//terrain, enemies, bonuses and maluses' drawing
	drawLevelElements(map[currentLevel].getTerrain());
	drawLevelElements(map[currentLevel].getEnemies());
	drawLevelElements(map[currentLevel].getBonuses());
	drawLevelElements(map[currentLevel].getMaluses());

	wrefresh(levelWindow);
	if(hero.getActionLog()[0].getDelay() > 0) std::this_thread::sleep_for(std::chrono::milliseconds(hero.getActionLog()[0].getDelay()));
}

void Game::drawHero(){
	int x=0, y=0;
	hero.getXY(x,y);

	//this should hide the previous' hero's position with a ' ' char
	if(hero.getActionLog()[0].getAnimation() != Animation::STILL){
		switch(hero.getActionLog()[0].getAnimation()){
			case Animation::LEFT:
			mvwaddch(levelWindow, y, x+1, ' ');
			break;

			case Animation::RIGHT:
			mvwaddch(levelWindow, y, x-1, ' ');
			break;

			case Animation::CLIMB_DOWN:
			//yet to decide a stairs character
			break;

			case Animation::FALLING:
			mvwaddch(levelWindow, y-1,x,' ');
			break;

			case Animation::CLIMB_UP:
			break;

			case Animation::JUMPING:
			mvwaddch(levelWindow, y+1, x, ' ');
			break;

			case Animation::STILL:
			default:
			break;
		}
	}
	
	mvwaddch(levelWindow, y, x, hero.getTileChar());

}

template <class T>
void Game::drawLevelElements(LinkedList<T> list){
	int x,y;
	for(int i=0; i<list.getSize(); i++){
		list[i].getXY(x,y);
		mvwaddch(levelWindow, y, x, list[i].getTileChar());
	}
}

void Game::drawHUD(WINDOW* hud){
	mvwprintw(hud, 0,0, "HP: %d/100 - SCORE: %d - LEVEL: %d", hero.getHp(), score, currentLevel);
	wrefresh(hud);
}

Action Game::input(){
	nodelay(levelWindow, TRUE);
	noecho();

	Animation proposedAnimation = Animation::STILL;
	Initiator proposedInitiator = Initiator::LOGIC;
	
	char userKey = '_';
	
	//key pressing detection mechanism
	if((userKey = wgetch(levelWindow)) != ERR){
		proposedAnimation = getCorrespondingAnimation(userKey);
		proposedInitiator = Initiator::USER;
	}

	//keep jumping till you reached JUMP_HEIGHT even if the user presses left or right keys to jump diagonally
	// (the user initiated a jump some time ago && the hero din't move JUMP_HEIGHT units up yet) && (the user didn't ask to make a new move yet)
	if(
		(
			hero.countMoves(Animation::JUMPING, Initiator::LOGIC) < JUMP_HEIGHT-1
			&& hero.countMoves(Animation::JUMPING, Initiator::USER) >= 1
		)
		&& proposedAnimation == Animation::STILL
	){
		if(hero.countMoves(Animation::JUMPING) < JUMP_HEIGHT) {
			proposedAnimation = Animation::JUMPING;
			proposedInitiator = Initiator::LOGIC;
		}else proposedAnimation = Animation::FALLING; // this line may be superfluous
	}

	//falling mechanic
	if(proposedAnimation == Animation::STILL && map[currentLevel].elementAt(hero.getX(), hero.getY()+1) == TileType::EMPTY){
		proposedAnimation = Animation::FALLING;
		proposedInitiator = Initiator::LOGIC;
	}
	
	//sets the coordinates in which the action will take place, the time delay for it and the initiator of the action
	
	return getCorrespondingAction(proposedAnimation, proposedInitiator);

}

Animation Game::getCorrespondingAnimation(char userKey){
	switch(userKey){
			case 'w':
			//maybe we can have the hero climb up a ladder
			return Animation::CLIMB_UP;
			break;

			case 'a':
			return Animation::LEFT;
			break;
			
			case 's':
			//climb down a ladder or no use
			return Animation::CLIMB_DOWN;
			break;
			
			case 'd':
			return Animation::RIGHT;
			break;
			
			case ' ':
			return Animation::JUMPING;
			break;

			case 'x':
			gameOver = true;
			return Animation::STILL;
			break;

			case 'p':
			//here code to stop all moving entities 
			//and to display some sort of PAUSE label somewhere
			return Animation::STILL;
			break;

			default:
			return Animation::STILL;

		}
}

Action Game::getCorrespondingAction(Animation animation, Initiator initiator){
	switch(animation){
		case Animation::LEFT:
		return Action(animation, hero.getX()-1, hero.getY(), AnimationDelay::LEFT, initiator);
		break;

		case Animation::RIGHT:
		return Action(animation, hero.getX()+1, hero.getY(), AnimationDelay::RIGHT, initiator);
		break;

		case Animation::JUMPING:
		return Action(animation, hero.getX(), hero.getY()-1, AnimationDelay::JUMPING, initiator);
		break;

		case Animation::FALLING:
		return Action(animation, hero.getX(), hero.getY()+1, AnimationDelay::FALLING, initiator);
		break;

		case Animation::CLIMB_UP:
		case Animation::CLIMB_DOWN:
		case Animation::STILL:
		case Animation::PAUSE:
		case Animation::QUIT:
		default:
		return Action(animation, hero.getX(), hero.getY(), AnimationDelay::STILL, initiator);
		
	}
}

void Game::logic(Action proposedAction){
	Action engagedAction;
	int h, w;
	getmaxyx(levelWindow, h, w);

	//establishes the legality of the action
	switch(proposedAction.getAnimation()){
		case Animation::CLIMB_UP:
		case Animation::CLIMB_DOWN:
		case Animation::PAUSE:
		case Animation::QUIT:
		break;

		case Animation::JUMPING:
		if(hero.getY()>0 && hero.getY() <= h){
			if(map[currentLevel].elementAt(hero.getX(), hero.getY()-1) == TileType::EMPTY){
				if(hero.countMoves(Animation::JUMPING) < JUMP_HEIGHT && hero.countMoves(Animation::FALLING) == 0){
					hero.setXY(hero.getX(), hero.getY()-1);
					engagedAction = proposedAction;
				}
			}
		}
		break;

		case Animation::LEFT:
		if(hero.getX()>0 && hero.getX() <= w) {
			if(map[currentLevel].elementAt(hero.getX()-1, hero.getY()) == TileType::EMPTY){
				hero.setXY(hero.getX()-1, hero.getY());
				engagedAction = proposedAction;
			}
		}
		break;

		case Animation::RIGHT:
		if(hero.getX()>=0 && hero.getX() < w) {
			if(map[currentLevel].elementAt(hero.getX()+1, hero.getY()) == TileType::EMPTY){
				hero.setXY(hero.getX()+1, hero.getY());
				engagedAction = proposedAction;
			}
		}
		break;

		case Animation::FALLING:
		if(hero.getY() >= 0 && hero.getY() < h){
			switch(map[currentLevel].elementAt(hero.getX(), hero.getY()+1))
			{
				case TileType::EMPTY:
				hero.setXY(hero.getX(), hero.getY()+1);
				engagedAction = proposedAction;
				break;
				/*
				case TileType::TERRAIN:
				engagedAction = Action(Animation::STILL, hero.getX(), hero.getY(), AnimationDelay::STILL, Initiator::LOGIC);
				break;
				*/
				case TileType::BONUS:
				case TileType::MALUS:
				case TileType::ENEMY:
				case TileType::HERO:
				case TileType::SIZE:
				default:
				break;
			}
		}
		break; 

		case Animation::STILL:
		default: // this if may be useless
		if(hero.getActionLog()[0].getAnimation() == Animation::FALLING && map[currentLevel].elementAt(hero.getX(), hero.getY()+1) == TileType::EMPTY){ 
			hero.setXY(hero.getX(), hero.getY()+1);
			engagedAction = Action(Animation::FALLING, hero.getX(), hero.getY(), AnimationDelay::FALLING, Initiator::LOGIC);
		}else engagedAction = proposedAction;
	}

	hero.registerMove(engagedAction);
	
}

void Game::mainLoop() {
	srand(time(NULL));

	initscr();

	int w, h;

	menu = Menu();
	menu.menuLoop();

	if(menu.getOption() == MenuOption::PLAY){
		getmaxyx(stdscr, h, w);
		int HUD_h = 1;
		h = h - HUD_h;
		levelWindow = newwin(h,w, HUD_h,0); //later it wil have a different begx and begy to accomodate the hud
		
		createMap();

		bool newLevel = true;
		while(!gameOver){
			using namespace std::chrono;

			time_point<system_clock> frameStart;
			duration<double> frameTime; // frameTime indicates how much time the current frame has taken
			frameStart = system_clock::now();
			
			//if(levelChanged) newLevel = true;
			draw(newLevel);
			logic(input());
			newLevel = false;

			frameTime = system_clock::now() - frameStart;
			
			if(frameDelay > frameTime){
				std::this_thread::sleep_for(frameDelay - frameTime);
			}

			
		}
	}

	endwin();
}

Menu Game::getMenu(){ return menu; }