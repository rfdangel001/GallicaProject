////////////////////////////////////////////////////////////////////////////////
//        FILE : Gallica_Main.cpp
//      AUTHOR : Charles Hosson
//        DATE :   Creation : June 20 2013
//               Last Entry : August 7 2013
// DESCRIPTION : 
////////////////////////////////////////////////////////////////////////////////

using namespace std;


#include <string>
#include <cmath>
#include <fstream>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

#include "SdlUtility.hpp"
#include "LinearAlgebra.hpp"

#include "Gallica_Globals.hpp"
#include "Gallica_CharacterClasses.hpp"
#include "Gallica_StateMachine.hpp"




void initialise ( );




int main ( int, char** )
{
	initialise();
	
	GameState* activeState = NULL;
	
	setNextState(STATE_INTRO);
	
	try {
		while ( GameState::currentStateCode != STATE_EXIT ) {
			delete activeState;
			activeState = getNewGameState();
			
			activeState->loadFiles();
			
			activeState->executeLoop();
		}
	} catch ( LinAlgError error ) {
		ofstream errLog("GallicaLog.txt");
		error.print(errLog);
		errLog.close();
	}
	
	TTF_Font* goodbyeFont = TTF_OpenFont("TestFont.ttf", 40);
	
	SDL_FillRect(screen, NULL, 0);
	
	Sdl::applyText("Goodbye", goodbyeFont, screen, 500, 300);
	
	SDL_Flip(screen);
	
	Sdl::wait(1000);
	
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

