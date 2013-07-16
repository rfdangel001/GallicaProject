////////////////////////////////////////////////////////////////////////////////
//        FILE : Gallica_CharacterClasses.hpp
//      AUTHOR : Charles Hosson & Rada Florin-Daniel
//        DATE :   Creation : June 22 2013
//               Last Entry : July 16 2013
// DESCRIPTION : Defines the classes for the characters and scoreboard
//               in the Gallica project.
////////////////////////////////////////////////////////////////////////////////

/*  REMARKS TO THE TEAM 
 * ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
 *   The acceleration and deceleration of the characters are proportionate to
 * their max speed. That will change with the powerups and the floor properties.
 *
 *   For now, the Player class methods and constructor don't take into account
 * the powerups or the level properties since those aren't finished.
 *
 *   The collision detection and handling is applied on every obstacle on the 
 * map. Obviously that's not very efficient, so it will change soon.
 *
 *   The Obstacle class only uses rectangular obstacles, that will also change.
 */

#pragma once

using namespace std;


#include <cmath>
#include <string>
#include <vector>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"

#include "LinearAlgebra.hpp"

#include "Gallica_Constants.hpp"
//TODO: #include "powerUps.hpp"




// Forward declarations
class ScoreBoard; class Character; class Player; class Obstacle;


// scoreboard class just basic for now
class ScoreBoard
{
public:
	ScoreBoard(int x, int y, int length, int width);
	~ScoreBoard() { SDL_FreeSurface(spriteSheet_);}
	void SetX(int x);
	void SetY(int y);
	void SetLength(int length);
	void SetWidth(int width);
	 int GetX(){return posX_;}
	 int GetY(){return posY_;}
	 int GetLength(){return length_;}
	 int GetWidth(){return width_;}

private:
	SDL_Surface* spriteSheet_;
	      string playerName_; // player name
	         int score_; // The actual score
	         int posX_; // X Position of scoreboard window while ingame
	         int posY_; // Y Position of scoreboard window while ingame
	         int length_; // Length of the scoreboard window while ingame
	         int width_; // Length of the Scoreboard window while ingame



};


class Character
{
public:
	// Modifying methods
	void updatePosition ( );
	
	// Non-modifying methods
	Vector getPosition ( );
	Vector getVelocity ( );
	double getRadius ( );
	   int getHealth ( );
	   int getArmor ( );
	   int getStamina ( );

protected:
	// Space attributes
	Vector position_;
	Vector velocity_;
	double radius_;

	// Graphics attributes
	SDL_Surface* spriteSheet_;
	   SDL_Rect* clips_;
	
	// Gameplay attributes
	   int healthPoints_;
	   int armorPoints_;
	   int staminaPoints_;
	
	// 
	double maxSpeed_;
};


class Player : public Character
{
public:
	// Constructor and destructor
	Player ( Vector, Vector, double, string, int, int, int, int, double );
	
	// Modifying methods
	void handleInput ( bool* );
	void handleBorderCollisions ( );
	void handleObstacleCollisions ( Obstacle* );
	
	void removeNullPowerUps ( );
	
	// Non-modifying method
	void apply ( SDL_Surface*, int );

protected:
	
};


class Obstacle
{
public:
	// Static attribute
	static int counter;
	
	// Constructor and destructor
	Obstacle ( );
	Obstacle ( string, Vector, double, double );
	~Obstacle ( );
	
	// Modifying methods
	
	// Non-modifying methods
	        Vector getPosition ( ) const;
	        double getWidth ( ) const;
	        double getHeight ( ) const;
	Sdl::Rectangle getRectangle ( ) const;
	          void apply ( SDL_Surface* );
	
	// Modifying operator
	Obstacle& operator = ( const Obstacle& );

protected:
	// Space attributes
	Vector position_;
	double width_;
	double height_;
	
	// Graphics attributes
	SDL_Surface* spriteSheet_;
};
int Obstacle::counter = 0;




void Character::updatePosition ( )
{
	if ( velocity_.magnitude() > maxSpeed_ ) {
		velocity_.normalise();
		velocity_ *= maxSpeed_;
	}
	
	position_ += velocity_ / SCREEN_FRAMERATE;
}


inline
Vector Character::getPosition ( )
{
	return position_;
}


inline
Vector Character::getVelocity ( )
{
	return velocity_;
}


inline
double Character::getRadius ( )
{
	return radius_;
}


inline
int Character::getHealth ( )
{
	return healthPoints_;
}


inline
int Character::getArmor ( )
{
	return armorPoints_;
}


inline
int Character::getStamina ( )
{
	return staminaPoints_;
}




Player::Player ( Vector position, Vector velocity, double radius,
                 string imageFileName, int clipsNumber, int healthPoints,
                 int armorPoints, int staminaPoints, double maxSpeed )
{
	position_ = position;
	velocity_ = velocity;
	radius_ = radius;
	
	spriteSheet_ = Sdl::loadImage(imageFileName);
	
	clips_ = new SDL_Rect[clipsNumber];
	for ( int i = 0; i < clipsNumber; i ++ )
		clips_[i] = {0, i * 2 * radius_, 2 * radius_, 2 * radius_};
	
	healthPoints_ = healthPoints;
	armorPoints_ = armorPoints;
	staminaPoints_ = staminaPoints;
	
	maxSpeed_ = maxSpeed;
}


void Player::handleInput ( bool* keyState )
{
	if ( keyState[SDLK_UP] )
		velocity_[1] -= 3 * maxSpeed_ / SCREEN_FRAMERATE;
	if ( keyState[SDLK_DOWN] )
		velocity_[1] += 3 * maxSpeed_ / SCREEN_FRAMERATE;
	if ( keyState[SDLK_RIGHT] )
		velocity_[0] += 3 * maxSpeed_ / SCREEN_FRAMERATE;
	if ( keyState[SDLK_LEFT] )
		velocity_[0] -= 3 * maxSpeed_ / SCREEN_FRAMERATE;
	
	if ( not keyState[SDLK_UP] and velocity_[1] < 0.0 ) {
		velocity_[1] += maxSpeed_ / SCREEN_FRAMERATE;
		if ( velocity_[1] > 0.0 )
			velocity_[1] = 0.0;
	}
	if ( not keyState[SDLK_DOWN] and velocity_[1] > 0.0 ) {
		velocity_[1] -= maxSpeed_ / SCREEN_FRAMERATE;
		if ( velocity_[1] < 0.0 )
			velocity_[1] = 0.0;
	}
	if ( not keyState[SDLK_RIGHT] and velocity_[0] > 0.0 ) {
		velocity_[0] -= maxSpeed_ / SCREEN_FRAMERATE;
		if ( velocity_[0] < 0.0 )
			velocity_[0] = 0.0;
	}
	if ( not keyState[SDLK_LEFT] and velocity_[0] < 0.0 ) {
		velocity_[0] += maxSpeed_ / SCREEN_FRAMERATE;
		if ( velocity_[0] > 0.0 )
			velocity_[0] = 0.0;
	}
}


void Player::handleBorderCollisions ( )
{
	if ( position_[0] - radius_ <= 0.0 ) {
		position_[0] = 0.0 + radius_;
		velocity_[0] = 0.0;
	}
	if ( position_[0] + radius_ >= LEVEL_WIDTH ) {
		position_[0] = LEVEL_WIDTH - radius_;
		velocity_[0] = 0.0;
	}
	if ( position_[1] - radius_ <= 0.0 ) {
		position_[1] = 0.0 + radius_;
		velocity_[1] = 0.0;
	}
	if ( position_[1] + radius_ >= LEVEL_HEIGHT ) {
		position_[1] = LEVEL_HEIGHT - radius_;
		velocity_[1] = 0.0;
	}
}


void Player::handleObstacleCollisions ( Obstacle* obstacles )
{
	for ( int i = 0; i < Obstacle::counter; i ++ ) {
		Sdl::CollisionData results;
		results = testCollision({position_[0], position_[1], radius_},
		                        obstacles[i].getRectangle());
		
		if ( results.flags bitand Sdl::COLLISION_TRUE ) {
			Vector distance = position_ - Vector({results.closestXToFirst,
			                                     results.closestYToFirst});
			distance.normalise();
			
			position_ = Vector({results.closestXToFirst, results.closestYToFirst}) +
			            distance * radius_;
			
			if (
			results.flags bitand Sdl::COLLISION_X_POS or
			results.flags bitand Sdl::COLLISION_X_NEG )
				velocity_[0] = 0.0;
			else if (
			results.flags bitand Sdl::COLLISION_Y_POS or
			results.flags bitand Sdl::COLLISION_Y_NEG )
				velocity_[1] = 0.0;
		}
	}
}


inline
void Player::apply ( SDL_Surface* destination, int clipIndex )
{
	Sdl::applySurface(spriteSheet_, destination, SCREEN_WIDTH / 2 - radius_,
	                  (SCREEN_HEIGHT + PANEL_HEIGHT) / 2 - radius_,
	                  &clips_[clipIndex]);
}





Obstacle::Obstacle ( )
{
	width_ = 0.0;
	height_ = 0.0;
	
	spriteSheet_ = NULL;
	
	counter ++;
}


Obstacle::Obstacle ( string imageFileName, Vector initPosition,
                     double initWidth, double initHeight )
{
	position_ = initPosition;
	width_ = initWidth;
	height_ = initHeight;
	
	spriteSheet_ = Sdl::loadImage(imageFileName);
	
	counter ++;
}


inline
Obstacle::~Obstacle ( )
{
	SDL_FreeSurface(spriteSheet_);
	
	counter --;
}


inline
Vector Obstacle::getPosition ( ) const
{
	return position_;
}


inline
double Obstacle::getWidth ( ) const
{
	return width_;
}


inline
double Obstacle::getHeight ( ) const
{
	return height_;
}


inline
Sdl::Rectangle Obstacle::getRectangle ( ) const
{
	return {position_[0], position_[1], width_, height_};
}


void Obstacle::apply ( SDL_Surface* destination )
{
	Sdl::applySurface(spriteSheet_, destination, position_[0] - width_ / 2,
	                  position_[1] - height_ / 2);
}


Obstacle& Obstacle::operator = ( const Obstacle& model)
{
	position_ = model.position_;
	width_ = model.width_;
	height_ = model.height_;
	
	spriteSheet_ = SDL_DisplayFormatAlpha(model.spriteSheet_);
	
	return *this;
}
