////////////////////////////////////////////////////////////////////////////////
//        FILE : SdlUtility.hpp
//      AUTHOR : Charles Hosson
//        DATE :   Creation : June 22 2013
//               Last Entry : July 8 2013
// DESCRIPTION : Various tools to use with SDL.
//     REMARKS : All classes and functions in this library are under the 
//               namespace 'Sdl'.
////////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace std;


//{ Includes

#include <string>
#include <cmath>

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
class Timer; class Button; class StringInput;


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
	// Constructor and Destructor
	Button ( string, int, int, int, int );
	~Button ( );
	
	// Modifying methods
	void handleInput ( Uint8, int, int );
	
	// Non-modifying methods
	 int getPositionX ( );
	 int getPositionY ( );
	 int getHeight ( );
	 int getWidth ( );
	bool isClicked ( );
	void apply ( SDL_Surface* );

protected:
	// Input attributes
	bool isMouseOver_;
	bool isPressed_;
	bool isClicked_;
	
	// Position attributes
	int positionX_;
	int positionY_;
	int height_;
	int width_;
	
	// Graphics attributes
	SDL_Surface* spriteSheet_;
	    SDL_Rect clips_[3];
};


class StringInput
{
public:
	// Constructor
	StringInput ( SDL_Color, SDL_Color, TTF_Font*, int, int, int );
	
	// Modifying methods
	void activate ( );
	void deactivate ( );
	void handleInput ( SDL_Event );
	
	// Non-modifying methods
	  bool isActive ( );
	string getText ( );
	  void apply ( SDL_Surface* );

protected:
	// Text attributes
	string text_;
	
	// Input attributes
	bool isActive_;
	
	// Position attributes
	int positionX_;
	int positionY_;
	int height_;
	int width_;
	
	// Graphics attributes
	SDL_Color textColor_;
	SDL_Color backgroundColor_;
	TTF_Font* displayFont_;
};

//}


//{ Functions

SDL_Surface* loadImage ( string );

void applySurface ( SDL_Surface*, SDL_Surface*, int, int, SDL_Rect* = NULL );

void applyText ( string, TTF_Font*, SDL_Surface*, int, int,
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

//{ Button::Constructor and Destructor

Button::Button ( string spriteFileName, int initX, int initY, int initWidth,
                 int initHeight )
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


inline
Button::~Button ( )
{
	SDL_FreeSurface ( spriteSheet_ );
}

//}


//{ Button::Modifying methods

void Button::handleInput ( Uint8 mouseState, int mouseX, int mouseY )
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


//{ StringInput

//{ StringInput::Constructor

StringInput::StringInput ( SDL_Color initTextColor, SDL_Color initBackColor,
                           TTF_Font* initDisplayFont, int initPositionX,
                           int initPositionY, int initWidth )
{
	text_ = "";
	
	textColor_ = initTextColor;
	backgroundColor_ = initBackColor;
	
	displayFont_ = initDisplayFont;
	
	positionX_ = initPositionX;
	positionY_ = initPositionY;
	width_ = initWidth;
	height_ = TTF_FontLineSkip(displayFont_) +
	          int(ceil(TTF_FontLineSkip(displayFont_) / 5.0));
	
	isActive_ = false;
}

//}


//{ StringInput::Modifying methods

inline
void StringInput::activate ( )
{
	isActive_ = true;
}


inline
void StringInput::deactivate ( )
{
	isActive_ = false;
	text_.clear();
}


void StringInput::handleInput ( SDL_Event input )
{
	if ( isActive_ ) {
		//TODO: save text input.
		if ( input.type == SDL_KEYDOWN ) {
			if ( input.key.keysym.sym == SDLK_BACKSPACE and text_.length() != 0 )
				text_.pop_back();
			else if ( 
			input.key.keysym.unicode >= 0x20 and 
			input.key.keysym.unicode <= 0x7E )
				text_ += char(input.key.keysym.unicode);
		}
	}
}

//}


//{ StringInput::Non-modifying methods

inline
bool StringInput::isActive ( )
{
	return isActive_;
}


inline
string StringInput::getText ( )
{
	return text_;
}


void StringInput::apply ( SDL_Surface* destination )
{
	if ( isActive_ == true ) {
		SDL_Rect zone = {positionX_, positionY_, width_, height_};
		Uint32 formattedBackColor = SDL_MapRGB(destination->format,
		                                       backgroundColor_.r,
		                                       backgroundColor_.g,
		                                       backgroundColor_.b);
		
		SDL_FillRect(destination, &zone, formattedBackColor);
		applyText(text_, displayFont_, destination, 
		          positionX_ + TTF_FontLineSkip(displayFont_) / 5,
		          positionY_ + TTF_FontLineSkip(displayFont_) / 5, textColor_);
	}
}

//}

//}




//{ Functions

SDL_Surface* loadImage ( string filename )
{
	SDL_Surface* loadedImage = IMG_Load(filename.c_str());
	
	SDL_Surface* optimisedImage = SDL_DisplayFormatAlpha(loadedImage);
	
	SDL_FreeSurface(loadedImage);
	
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
	
	SDL_FreeSurface(textSurface);
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

