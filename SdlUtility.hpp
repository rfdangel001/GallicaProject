////////////////////////////////////////////////////////////////////////////////
//        FILE : SdlUtility.hpp
//      AUTHOR : Charles Hosson
//        DATE :   Creation : June 22 2013
//               Last Entry : July 4 2013
// DESCRIPTION : Various tools to use with SDL.
//     REMARKS : All classes and functions in this library are under the 
//               namespace 'Sdl'.
////////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace std;


//{ Includes

#include <string>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

//}


namespace Sdl
{

//{ Declarations

//{ Constants



//}


//{ Classes

// Forward declarations
class Timer; class Button;


class Timer
{
public:
	// Constructor
	Timer ( );
	
	// Modifying methods
	void start ( );
	void stop ( );
	void pause ( );
	void unpause ( );
	
	// Non-modifying methods
	bool isStarted ( );
	bool isPaused ( );
	 int getTicks ( );

protected:
	// Attributes
	bool isStarted_;
	bool isPaused_;
	 int startTicks_;
	 int pausedTicks_;
};


class Button
{
public:
	// Constructors
	Button ( string, int, int, int, int );
	
	// Modifying methods
	void checkInput ( Uint8, int, int );
	
	// Non-modifying methods
	 int getPositionX ( );
	 int getPositionY ( );
	 int getHeight ( );
	 int getWidth ( );
	bool isClicked ( );
	void apply ( SDL_Surface* );

protected:
	// Position attributes
	int positionX_;
	int positionY_;
	int height_;
	int width_;
	
	// Input attributes
	bool isMouseOver_;
	bool isPressed_;
	bool isClicked_;
	
	// Graphics attributes
	SDL_Surface* spriteSheet_;
	    SDL_Rect clips_[3];
	
};

//}


//{ Functions

SDL_Surface* loadImage ( string );

void applySurface ( SDL_Surface*, SDL_Surface*, int = 0, int = 0,
                    SDL_Rect* = NULL );

void applyText ( string, TTF_Font*, SDL_Surface*, int = 0, int = 0,
                 SDL_Color = {255, 255, 255} );

void wait ( int );

//}

//}




//{ Timer

//{ Timer::Constructor

Timer::Timer ( )
{
	isStarted_ = false;
	isPaused_ = false;
	startTicks_ = 0;
	pausedTicks_ = 0;
}

//}


//{ Timer::Modifying methods

void Timer::start ( )
{
	isStarted_ = true;
	isPaused_ = false;
	
	startTicks_ = SDL_GetTicks();
}


void Timer::stop ( )
{
	isStarted_ = false;
	isPaused_ = false;
	
	startTicks_ = 0;
	pausedTicks_ = 0;
}


void Timer::pause ( )
{
	if ( isStarted_ == true and isPaused_ == false ) {
		isPaused_ = true;
		
		pausedTicks_ = SDL_GetTicks() - startTicks_;
	}
}


void Timer::unpause ( )
{
	if ( isPaused_ == true ) {
		isPaused_ = false;
		
		startTicks_ = SDL_GetTicks() - pausedTicks_;
		
		pausedTicks_ = 0;
	}
}

//}


//{ Timer::Non-modifying methods

inline
bool Timer::isStarted ( )
{
	return isStarted_;
}


inline
bool Timer::isPaused ( )
{
	return isPaused_;
}


int Timer::getTicks ( )
{
	if ( isStarted_ == true ) {
		if ( isPaused_ == true )
			return pausedTicks_;
		else
			return SDL_GetTicks() - startTicks_;
	}
	else
		return 0;
}

//}

//}


//{ Button

//{ Button::Constructor

Button::Button ( string spriteFileName, int initX, int initY, int initHeight,
                 int initWidth )
{
	positionX_ = initX;
	positionY_ = initY;
	height_ = initHeight;
	width_ = initWidth;
	
	isMouseOver_ = false;
	isPressed_ = false;
	isClicked_ = false;
	
	spriteSheet_ = loadImage(spriteFileName);
	
	clips_[0] = {0, 0, 200, 50};
	clips_[1] = {0, 50, 200, 50};
	clips_[2] = {0, 100, 200, 50};
}

//}


//{ Button::Modifying methods

void Button::checkInput ( Uint8 mouseState, int mouseX, int mouseY )
{
	isClicked_ = false;
	
	if (
	(mouseX >= positionX_ and mouseX <= positionX_ + width_) and
	(mouseY >= positionY_ and mouseY <= positionY_ + height_) )
		isMouseOver_ = true;
	else
		isMouseOver_ = false;
	
	if ( (mouseState bitand SDL_BUTTON(1)) and isMouseOver_ )
		isPressed_ = true;
	else if ( not (mouseState bitand SDL_BUTTON(1)) and isPressed_ ) {
		isClicked_ = true;
		isPressed_ = false;
	}
	else
		isPressed_ = false;
}

//}


//{ Button::Non-modifying methods

inline
int Button::getPositionX ( )
{
	return positionX_;
}


inline
int Button::getPositionY ( )
{
	return positionY_;
}


inline
int Button::getHeight ( )
{
	return height_;
}


inline
int Button::getWidth ( )
{
	return width_;
}


inline
bool Button::isClicked ( )
{
	return isClicked_;
}


void Button::apply ( SDL_Surface* destination )
{
	if ( isPressed_ )
	applySurface(spriteSheet_, destination, positionX_, positionY_, 
	             &clips_[2]);
	else if ( isMouseOver_ )
	applySurface(spriteSheet_, destination, positionX_, positionY_, 
	             &clips_[1]);
	else
	applySurface(spriteSheet_, destination, positionX_, positionY_, 
	             &clips_[0]);
}

//}

//}




//{ Functions

SDL_Surface* loadImage ( string filename )
{
	SDL_Surface* loadedImage = IMG_Load(filename.c_str());
	
	SDL_Surface* optimisedImage = SDL_DisplayFormat(loadedImage);
	
	return optimisedImage;
}


void applySurface ( SDL_Surface* source, SDL_Surface* destination, int offsetX,
                    int offsetY, SDL_Rect* clip )
{
	SDL_Rect offsetPosition;
	
	offsetPosition.x = offsetX;
	offsetPosition.y = offsetY;
	
	SDL_BlitSurface(source, clip, destination, &offsetPosition);
}


void applyText ( string text, TTF_Font* font, SDL_Surface* destination,
                 int offsetX, int offsetY, SDL_Color textColor )
{
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(),
	                                                textColor);
	
	applySurface(textSurface, destination, offsetX, offsetY);
}


void wait ( int milliseconds )
{
	Timer timer;
	timer.start();
	
	bool end = false;
	while ( not end )
		if ( timer.getTicks() > milliseconds )
			end = true;
}

//}

}

