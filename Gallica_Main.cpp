////////////////////////////////////////////////////////////////////////////////
//        FILE : Gallica_Main.cpp
//      AUTHOR : Charles Hosson
//        DATE :   Creation : June 20 2013
//               Last Entry : July 16 2013
// DESCRIPTION : 
////////////////////////////////////////////////////////////////////////////////

using namespace std;


#include <string>
#include <cmath>
#include <fstream>
#include <iomanip>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

#include "SdlUtility.hpp"
#include "LinearAlgebra.hpp"

#include "Gallica_Constants.hpp"
#include "Gallica_CharacterClasses.hpp"




SDL_Surface* screen = NULL;




void initialise ( );

void applyBackground ( SDL_Surface*, Vector );

void applyUi ( Player, TTF_Font* );




int main ( int, char** )
{
  initialise();
	SDL_Surface* floor = Sdl::loadImage("background.png");
	
	ofstream logFile("TEST.txt");  // This is used to record the framerate.
	
	Player player({50.0, 50.0}, {0.0, 0.0}, 25.0, "player.png", 1, 5, 5, 5,
	            400.0);
	
	Obstacle* allObstacles = new Obstacle[2];
	
	allObstacles[0] = Obstacle("Obstacle.png", {500.0, 500.0}, 200.0, 200.0);
	allObstacles[1] = Obstacle("Obstacle.png", {1000.0, 1000.0}, 200.0, 200.0);
	
	// We apply the obstacle images before the game loop begins because they
	// won't move at anytime. 
	for ( int i = 0; i < Obstacle::counter; i ++ )
		allObstacles[i].apply(floor);
	
	SDL_Event input;
	bool* keyState = NULL;
	int mouseX, mouseY;
	uint8_t mouseState;
	
	Sdl::Button startButton("StartButton.png", 500, 400, 200, 50);
	TTF_Font* titleFont = TTF_OpenFont("prstartk.ttf", 40);
	
	// This is the menu loop.
	bool end = false;
	while ( not end ) {
		SDL_PollEvent(&input);
		if ( input.type == SDL_QUIT )
			end = true;
		
		mouseState = SDL_GetMouseState(&mouseX, &mouseY);
		
		startButton.handleInput(mouseState, mouseX, mouseY);
		if ( startButton.isClicked() )
			end = true;
		
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
		
		Sdl::applyText("Hello, World!", titleFont, screen, 450, 300,
		               {255, 15, 15});
		
		startButton.apply(screen);
		
		SDL_Flip(screen);
	}
	
	TTF_Font* uiFont = TTF_OpenFont("prstartk.ttf", 20);
	
	Sdl::Timer framerateTimer;
	
	// This is the game loop.
	end = false;
	while ( not end ) {
		framerateTimer.start();
		
		SDL_PollEvent(&input);
		if ( input.type == SDL_QUIT )
			end = true;
		
		keyState = (bool*)(SDL_GetKeyState(NULL));
		
		player.handleInput(keyState);
		player.updatePosition();
		player.handleBorderCollisions();
		
		player.handleObstacleCollisions(allObstacles);
		
		SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 75, 75, 75));
		
		applyBackground(floor, player.getPosition());
		
		player.apply(screen, 0);
		
		applyUi(player, uiFont);
		
		SDL_Flip(screen);
		
		logFile << framerateTimer.getTicks() << " ms" << "\n"
		        << player.getVelocity()[0] << ", " << player.getVelocity()[1]
		        << "\n\n";
		
		Sdl::wait(1000 / SCREEN_FRAMERATE - framerateTimer.getTicks());
	}
	
	SDL_Quit();
	
	return 0;
}




void initialise ( )
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);
	
	screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP,
	                          SDL_HWSURFACE);
	
	SDL_WM_SetCaption("Gallica", NULL);
}


void applyBackground ( SDL_Surface* backgroundImage, Vector playerPosition )
{
	SDL_Rect clip = {playerPosition[0] - SCREEN_WIDTH / 2,
	                 playerPosition[1] - (SCREEN_HEIGHT + PANEL_HEIGHT) / 2,
	                 SCREEN_WIDTH, SCREEN_HEIGHT};
	
	Sdl::applySurface(backgroundImage, screen, 0, 0, &clip);
}


void applyUi ( Player player, TTF_Font* uiFont)
{
	SDL_Rect panel = {0, 0, SCREEN_WIDTH, PANEL_HEIGHT};
	SDL_FillRect(screen, &panel, SDL_MapRGB(screen->format, 10, 10, 10));
	
	Sdl::applyText("HP" + string(player.getHealth(), '|'), uiFont,
	               screen, 20, 20, {255, 10, 10});
	Sdl::applyText("AP" + string(player.getArmor(), '|'), uiFont,
	               screen, 220, 20, {80, 80, 255});
	Sdl::applyText("SP" + string(player.getStamina(), '|'), uiFont,
	               screen, 420, 20, {10, 255, 10});
}

