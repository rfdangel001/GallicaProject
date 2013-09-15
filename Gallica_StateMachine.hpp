////////////////////////////////////////////////////////////////////////////////
//        FILE : Gallica_StateMachine.hpp
//      AUTHOR : Charles Hosson
//        DATE :   Creation : June 28 2013
//               Last Entry : August 10 2013
// DESCRIPTION : Defines the classes and the functions for the Gallica game
//               state machine.
////////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace std;


//{ Includes

#include <string>
#include <cmath>
#include <limits>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

#include "SdlUtility.hpp"
#include "LinearAlgebra.hpp"

#include "Gallica_Globals.hpp"
#include "Gallica_CharacterClasses.hpp"

//}




//{ Declarations

//{ Structures

struct LevelData
{
	double width;
	double height;
	   int maxAiCount;
};

//}


//{ Classes

// Forward declarations
class GameState; class Intro; class MainMenu; class ActiveLevel;


class GameState
{
public:
	// Static attributes
	static int currentStateCode;
	
	// Constructor and destructor
	virtual ~GameState ( ) {};
	
	// Methods
	virtual void loadFiles ( ) = 0;
	virtual void executeLoop ( ) = 0;
	virtual void handleEvents ( bool& ) = 0;
	virtual void renderScreen ( ) = 0;

protected:
	     SDL_Event inputEvent_;
	Sdl::MouseData mouseState_;
	      uint8_t* keyboardState_;
	    Sdl::Timer framerateTimer_;
};
int GameState::currentStateCode = STATE_NULL;


class Intro : public GameState
{
public:
	Intro ( ) {};
	~Intro ( );
	
	void loadFiles ( );
	void executeLoop ( );
	void handleEvents ( bool& );
	void renderScreen ( );

protected:
	SDL_Surface* titleImage_;
};


class MainMenu : public GameState
{
public:
	enum ButtonIndex {START,
	                  QUIT,
	                  LEVEL_SELECT,
	                  LEVEL_1,
	                  LEVEL_2};
	
	MainMenu ( ) {};
	~MainMenu ( );
	
	void loadFiles ( );
	void executeLoop ( );
	void handleEvents ( bool& );
	void renderScreen ( );

protected:
	Sdl::Button* buttons_;
	SDL_Surface* background_;
};


class ActiveLevel : public GameState
{
public:
	ActiveLevel ( ) {};
	~ActiveLevel ( );
	
	void loadFiles ( );
	void executeLoop ( );
	void handleEvents ( bool& );
	void renderScreen ( );
	
	void parseSettingsFile ( ifstream& );
	void parseLevelFile ( ifstream&, string, void (ActiveLevel::*)(ifstream&) );
	void parseData ( ifstream& );
	void parsePlayer ( ifstream& );
	void parseObstacles ( ifstream& );
	void readObstacleSection ( ifstream&, int );
	
	void applyUi ( );
	void applyBackground ( );

protected:
	LevelData data_;
	
	   Player player_;
	Obstacle* allObstacles_;
	
	   TTF_Font* uiFont_;
	SDL_Surface* floorImage_;
};

//}


//{ Functions

void setNextState ( int );

GameState* getNewGameState ( );

//}

//}




//{ Intro

Intro::~Intro ( )
{
	SDL_FreeSurface(titleImage_);
}


void Intro::loadFiles ( )
{
	TTF_Font* titleFont = TTF_OpenFont(INTRO_FONT_FILENAME.c_str(), 40);
	
	SDL_Surface* background = Sdl::loadImage(INTRO_BACKGROUND_FILENAME.c_str());
	
	SDL_Surface* text = TTF_RenderText_Solid(titleFont, "Hello, world!", 
	                                         {10, 10, 200});
	
	titleImage_ = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_WIDTH,
	                                   SCREEN_HEIGHT, SCREEN_BPP,
	                                   0, 0, 0, 0x000000FF);
	
	Sdl::applySurface(background, titleImage_, 0, 0);
	
	Sdl::applySurface(text, titleImage_, SCREEN_WIDTH / 2 - text->w / 2,
	                  SCREEN_HEIGHT / 2 - text->h / 2);
	
	SDL_FreeSurface(background);
	SDL_FreeSurface(text);
	TTF_CloseFont(titleFont);
}


void Intro::executeLoop ( )
{
	int frameCounter = 0;
	
	bool introDone = false;
	while ( not introDone ) {
		framerateTimer_.start();
		
		this->handleEvents(introDone);
		
		if ( frameCounter >= 1.5 * SCREEN_FRAMERATE )
			introDone = true;
		
		this->renderScreen();
		
		frameCounter ++;
		
		Sdl::wait(1000 / SCREEN_FRAMERATE - framerateTimer_.getTicks());
	}
	framerateTimer_.stop();
	
	setNextState(STATE_MAIN_MENU);
}


void Intro::handleEvents ( bool& endLoop )
{
	SDL_PollEvent(&inputEvent_);
	
	if ( inputEvent_.type == SDL_QUIT ) {
		setNextState(STATE_EXIT);
		endLoop = true;
	}
	else if ( inputEvent_.key.keysym.sym == SDLK_ESCAPE )
		endLoop = true;
}


void Intro::renderScreen ( )
{
	SDL_FillRect(screen, NULL, 0);
	
	Sdl::applySurface(titleImage_, screen, 0, 0);
	
	SDL_Flip(screen);
}

//}


//{ MainMenu

MainMenu::~MainMenu ( )
{
	SDL_FreeSurface(background_);
	
	delete []buttons_;
}


void MainMenu::loadFiles ( )
{
	background_ = Sdl::loadImage("MenuBackground.png");
	
	buttons_ = new Sdl::Button[1];
	buttons_[0] = Sdl::Button("StartButton.png", 500, 400, 200, 50);
}


void MainMenu::executeLoop ( )
{
	bool menuDone = false;
	while ( not menuDone ) {
		framerateTimer_.start();
		
		this->handleEvents(menuDone);
		
		this->renderScreen();
		
		Sdl::wait(1000 / SCREEN_FRAMERATE - framerateTimer_.getTicks());
	}
	framerateTimer_.stop();
}


void MainMenu::handleEvents ( bool& endLoop )
{
	SDL_PollEvent(&inputEvent_);
	mouseState_ = Sdl::getMouseState ( );
	
	buttons_[0].handleInput(mouseState_);
	
	if ( inputEvent_.type == SDL_QUIT ) {
		setNextState(STATE_EXIT);
		endLoop = true;
	}
	else if ( buttons_[0].isClicked() ) {
		setNextState(STATE_LEVEL_1);
		endLoop = true;
	}
}


void MainMenu::renderScreen ( )
{
	SDL_FillRect(screen, NULL, 0);
	
	Sdl::applySurface(background_, screen, 0, 0);
	
	buttons_[0].apply(screen);
	
	SDL_Flip(screen);
}

//}


//{ ActiveLevel

ActiveLevel::~ActiveLevel ( )
{
	TTF_CloseFont(uiFont_);
	SDL_FreeSurface(floorImage_);
}


void ActiveLevel::loadFiles ( )
{
	ifstream uiSettingsFile("Ui.cfg");
	parseSettingsFile(uiSettingsFile);
	uiSettingsFile.close();
	string levelFilename = "Level" + to_string(GameState::currentStateCode);
	
	ifstream levelFile(levelFilename);
	parseLevelFile(levelFile, "LEVEL", &ActiveLevel::parseData);
	parseLevelFile(levelFile, "PLAYER", &ActiveLevel::parsePlayer);
	parseLevelFile(levelFile, "OBSTACLES", &ActiveLevel::parseObstacles);
	levelFile.close();
	
	for ( int i = 0; i < Obstacle::count; i ++ )
		allObstacles_[i].apply(floorImage_);
}


void ActiveLevel::executeLoop ( )
{
	bool gamePaused = false;
	
	bool levelDone = false;
	while ( not levelDone ) {
		framerateTimer_.start();
		
		this->handleEvents(levelDone);
		
		player_.updatePosition();
		
		player_.handleBorderCollisions(data_.width, data_.height);
		
		player_.handleObstacleCollisions(allObstacles_);
		
		this->renderScreen();
		
		Sdl::wait(1000 / SCREEN_FRAMERATE - framerateTimer_.getTicks());
	}
	framerateTimer_.stop();
}


void ActiveLevel::handleEvents ( bool& endLoop )
{
	SDL_PollEvent(&inputEvent_);
	mouseState_ = Sdl::getMouseState();
	keyboardState_ = SDL_GetKeyState(NULL);
	
	if ( inputEvent_.type == SDL_QUIT ) {
		setNextState(STATE_EXIT);
		endLoop = true;
	}
	
	player_.handleInput(keyboardState_);
}


void ActiveLevel::renderScreen ( )
{
	SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 60, 60, 60));
	
	this->applyBackground();
	
	player_.apply(Player::CLIP_DEFAULT);
	
	this->applyUi();
	
	SDL_Flip(screen);
}


void ActiveLevel::parseSettingsFile ( ifstream& settingsFile )
{
	string uiFontFilename;
	int uiFontSize;
	
	while ( not ws(settingsFile).eof() ) {
		settingsFile.ignore(numeric_limits<streamsize>::max(), '[');
		string currentSection;
		getline(settingsFile, currentSection);
		for ( int i = 0; i < currentSection.length(); i ++ )
			currentSection[i] = toupper(currentSection[i]);
		
		if ( currentSection == "PANEL]" )
			while ( settingsFile.peek() != '[' and 
			not ws(settingsFile).eof() ) {
				string currentProperty;
				settingsFile >> currentProperty;
				for ( int i = 0; i < currentProperty.length(); i ++ )
					currentProperty[i] = toupper(currentProperty[i]);
				
				settingsFile.ignore(3);
				
				if ( currentProperty == "FONTFILENAME" )
					getline(settingsFile, uiFontFilename);
				else if ( currentProperty == "FONTSIZE" )
					settingsFile >> uiFontSize;
			}
		
		uiFont_ = TTF_OpenFont(uiFontFilename.c_str(), uiFontSize);
	}
}


void ActiveLevel::parseLevelFile ( ifstream& levelFile, string section,
                                   void (ActiveLevel::*parsingMethod)(
                                   ifstream&) )
{
	for ( int i = 0; i < section.length(); i ++ )
		section[i] = toupper(section[i]);
	
	levelFile.seekg(0);
	
	bool endRead = false;
	while ( not endRead ) {
		levelFile.ignore(numeric_limits<streamsize>::max(), '[');
		
		string currentSection;
		getline(levelFile, currentSection);
		for ( int i = 0; i < currentSection.length(); i ++ )
			currentSection[i] = toupper(currentSection[i]);
		
		if ( currentSection == section + "]" ) {
			(this->*parsingMethod)(levelFile);
			endRead = true;
		}
	}
}


void ActiveLevel::parseData ( ifstream& levelFile )
{
	while ( not levelFile.eof() ) {
		string currentProperty;
		levelFile >> currentProperty;
		for ( int i = 0; i < currentProperty.length(); i ++ )
			currentProperty[i] = toupper(currentProperty[i]);
		
		levelFile.ignore(3);
		
		if ( currentProperty == "WIDTH" )
			levelFile >> data_.width;
		else if ( currentProperty == "HEIGHT" )
			levelFile >> data_.height;
		else if ( currentProperty == "FLOORIMAGEFILENAME" ) {
			string floorImageFilename;
			getline(levelFile, floorImageFilename);
			floorImage_ = Sdl::loadImage(floorImageFilename);
		}
		else if ( currentProperty == "MAXAICOUNT" )
			levelFile >> data_.maxAiCount;
	}
}


void ActiveLevel::parsePlayer ( ifstream& levelFile)
{
	Vector initPosition(2);
	Vector initVelocity(2);
	string spriteSheetFilename;
	
	while ( not levelFile.eof() ) {
		string currentProperty;
		levelFile >> currentProperty;
		for ( int i = 0; i < currentProperty.length(); i ++ )
			currentProperty[i] = toupper(currentProperty[i]);
		
		levelFile.ignore(3);
		
		if ( currentProperty == "INITPOSITION" ) {
			levelFile >> initPosition[0] >> initPosition[1];
		}
		else if ( currentProperty == "INITVELOCITY" ) {
			levelFile >> initVelocity[0] >> initVelocity[1];
		}
		else if ( currentProperty == "SPRITESHEETFILENAME" )
			getline(levelFile, spriteSheetFilename);
	}
	
	player_ = Player(initPosition, initVelocity, Player::INIT_RADIUS,
	                 spriteSheetFilename);
}


void ActiveLevel::parseObstacles ( ifstream& levelFile )
{
	levelFile.ignore(numeric_limits<streamsize>::max(), '=');
	
	int numberOfObstacles;
	levelFile >> numberOfObstacles;
	
	allObstacles_ = new Obstacle[numberOfObstacles];
	
	for ( int i = 0; i < Obstacle::count; i ++ ) {
		levelFile.clear();
		levelFile.seekg(0);
		while ( not ws(levelFile).eof() ) {
			levelFile.ignore(numeric_limits<streamsize>::max(), '[');
			
			string currentSection;
			levelFile >> currentSection;
			for ( int j = 0; j < currentSection.length(); j ++ )
				currentSection[j] = toupper(currentSection[j]);
			
			if ( currentSection == "OBSTACLE" + to_string(i + 1) + "]" ) {
				this->readObstacleSection(levelFile, i);
			}
		}
	}
}


void ActiveLevel::readObstacleSection ( ifstream& levelFile, int obstacleIndex )
{
	string imageFileName;
	Vector initPosition(2);
	double initWidth;
	double initHeight;
	
	uint8_t endBits = 0;
	
	while ( endBits != 15 ) {
		string currentProperty;
		levelFile >> currentProperty;
		for ( int i = 0; i < currentProperty.length(); i ++ )
			currentProperty[i] = toupper(currentProperty[i]);
		
		levelFile.ignore(3);
		
		if ( currentProperty == "IMAGEFILENAME" ) {
			getline(levelFile, imageFileName);
			endBits += exp2(0);
		}
		else if ( currentProperty == "POSITION" ) {
			levelFile >> initPosition[0] >> initPosition[1];
			endBits += exp2(1);
		}
		else if ( currentProperty == "WIDTH" ) {
			levelFile >> initWidth;
			endBits += exp2(2);
		}
		else if ( currentProperty == "HEIGHT" ) {
			levelFile >> initHeight;
			endBits += exp2(3);
		}
	}
	
	allObstacles_[obstacleIndex] = Obstacle(imageFileName, initPosition,
	                                        initWidth, initHeight);
}


void ActiveLevel::applyUi ( )
{
	SDL_Rect* panelRect = new SDL_Rect;
	*panelRect = {0, 0, SCREEN_WIDTH, PANEL_HEIGHT};
	SDL_FillRect(screen, panelRect, SDL_MapRGB(screen->format, 10, 10, 10));
	
	Sdl::applyText("HP" + string(player_.getHealth(), '|'), uiFont_,
	               screen, 20, 20, {255, 10, 10});
	Sdl::applyText("AP" + string(player_.getArmor(), '|'), uiFont_,
	               screen, 220, 20, {0, 100, 255});
	Sdl::applyText("SP" + string(player_.getStamina(), '|'), uiFont_,
	               screen, 420, 20, {10, 255, 10});
}


void ActiveLevel::applyBackground ( )
{
	SDL_Rect clip = {player_.getPosition()[0] - SCREEN_WIDTH / 2,
	                 player_.getPosition()[1] - (SCREEN_HEIGHT + PANEL_HEIGHT)
	                 / 2, SCREEN_WIDTH, SCREEN_HEIGHT};
	
	Sdl::applySurface(floorImage_, screen, 0, 0, &clip);
}

//}




//{ Functions

inline
void setNextState ( int nextGameStateCode )
{
	if ( GameState::currentStateCode != STATE_EXIT )
		GameState::currentStateCode = nextGameStateCode;
}


GameState* getNewGameState ( )
{
	switch ( GameState::currentStateCode ) {
		case STATE_INTRO :
			return new Intro;
			break;
		case STATE_MAIN_MENU :
			return new MainMenu;
			break;
		case STATE_LEVEL_1 :
		case STATE_LEVEL_2 :
			return new ActiveLevel;
			break;
		default :
			return NULL;
			break;
	}
}

//}


