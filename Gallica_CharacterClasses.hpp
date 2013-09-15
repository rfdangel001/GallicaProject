////////////////////////////////////////////////////////////////////////////////
//        FILE : Gallica_CharacterClasses.hpp
//      AUTHOR : Charles Hosson & Rada Florin-Daniel
//        DATE :   Creation : June 22 2013
//               Last Entry : August 10 2013
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
#include "SdlUtility.hpp"

#include "Gallica_Globals.hpp"
//TODO: #include "powerUps.hpp"




// Forward declarations
class ScoreBoard; class Character; class Player; class Obstacle;
Vector vec1 = {1.0, 2.0};


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
	// Constructor
	Character ( );
	
	// Modifying methods
	void updatePosition ( );
	
	// Non-modifying methods
	Vector getPosition ( ) const;
	Vector getVelocity ( ) const;
	double getRadius ( ) const;
	   int getHealth ( ) const;
	   int getArmor ( ) const;
	   int getStamina ( ) const;

protected:
	// Space attributes
	Vector position_;
	Vector velocity_;
	double radius_;
	double maxSpeed_;
	
	// Graphics attributes
	SDL_Surface* spriteSheet_;
	   SDL_Rect* clips_;
	
	// Gameplay attributes
	int healthPoints_;
	int armorPoints_;
	int staminaPoints_;
};


class Player : public Character
{
public:
	// Sprite clipping constants
	static constexpr int CLIPS_COUNT = 1;
	static constexpr int CLIP_DEFAULT = 0;
	
	// Default attributes constants
	static constexpr    int INIT_HEALTH = 5;
	static constexpr    int INIT_ARMOR = 5;
	static constexpr    int INIT_STAMINA = 5;
	static constexpr double INIT_MAX_SPEED = 400.0;
	static constexpr double INIT_RADIUS = 25.0;
	
	static constexpr double ACCELERATION = 4.0;
	static constexpr double DECELERATION = 1.5;
	
	// Constructor and destructor
	Player ( ) {};
	Player ( Vector, Vector, double, string );
	~Player ( );
	
	// Modifying methods
	void handleInput ( uint8_t* );
	void handleBorderCollisions ( double, double );
	void handleObstacleCollisions ( Obstacle* );
	
	void removeNullPowerUps ( );
	
	// Non-modifying method
	void apply ( int );
	
	// Modifying operators
	Player& operator = ( const Player& );
};


class Obstacle
{
public:
	// Static attribute
	static int count;
	
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
int Obstacle::count = 0;




Character::Character ( )
{
	position_ = {0, 0};
	velocity_ = {0, 0};
	radius_ = 0.0;
	maxSpeed_ = 0.0;
	
	spriteSheet_ = NULL;
	clips_ = NULL;
	
	healthPoints_ = 0;
	armorPoints_ = 0;
	staminaPoints_ = 0;
}


void Character::updatePosition ( )
{
	if ( velocity_.magnitude() > maxSpeed_ ) {
		velocity_.normalise();
		velocity_ *= maxSpeed_;
	}
	
	position_ += velocity_ / SCREEN_FRAMERATE;
}


inline
Vector Character::getPosition ( ) const
{
	return position_;
}


inline
Vector Character::getVelocity ( ) const
{
	return velocity_;
}


inline
double Character::getRadius ( ) const
{
	return radius_;
}


inline
int Character::getHealth ( ) const
{
	return healthPoints_;
}


inline
int Character::getArmor ( ) const
{
	return armorPoints_;
}


inline
int Character::getStamina ( ) const
{
	return staminaPoints_;
}




Player::Player ( Vector position, Vector velocity, double radius,
                 string imageFileName )
{
	position_ = position;
	velocity_ = velocity;
	radius_ = INIT_RADIUS;
	
	spriteSheet_ = Sdl::loadImage(imageFileName);
	
	clips_ = new SDL_Rect[CLIPS_COUNT];
	for ( int i = 0; i < CLIPS_COUNT; i ++ )
		clips_[i] = {0, i * 2 * radius_, 2 * radius_, 2 * radius_};
	
	healthPoints_ = INIT_HEALTH;
	armorPoints_ = INIT_ARMOR;
	staminaPoints_ = INIT_STAMINA;
	
	maxSpeed_ = INIT_MAX_SPEED;
}


Player::~Player ( )
{
	SDL_FreeSurface(spriteSheet_);
	
	delete []clips_;
}


void Player::handleInput ( uint8_t* keyState )
{
	if ( keyState[SDLK_UP] )
		velocity_[1] -= ACCELERATION * maxSpeed_ / SCREEN_FRAMERATE;
	if ( keyState[SDLK_DOWN] )
		velocity_[1] += ACCELERATION * maxSpeed_ / SCREEN_FRAMERATE;
	if ( keyState[SDLK_RIGHT] )
		velocity_[0] += ACCELERATION * maxSpeed_ / SCREEN_FRAMERATE;
	if ( keyState[SDLK_LEFT] )
		velocity_[0] -= ACCELERATION * maxSpeed_ / SCREEN_FRAMERATE;
	
	if ( not keyState[SDLK_UP] and velocity_[1] < 0.0 ) {
		velocity_[1] += DECELERATION * maxSpeed_ / SCREEN_FRAMERATE;
		if ( velocity_[1] > 0.0 )
			velocity_[1] = 0.0;
	}
	if ( not keyState[SDLK_DOWN] and velocity_[1] > 0.0 ) {
		velocity_[1] -= DECELERATION * maxSpeed_ / SCREEN_FRAMERATE;
		if ( velocity_[1] < 0.0 )
			velocity_[1] = 0.0;
	}
	if ( not keyState[SDLK_RIGHT] and velocity_[0] > 0.0 ) {
		velocity_[0] -= DECELERATION * maxSpeed_ / SCREEN_FRAMERATE;
		if ( velocity_[0] < 0.0 )
			velocity_[0] = 0.0;
	}
	if ( not keyState[SDLK_LEFT] and velocity_[0] < 0.0 ) {
		velocity_[0] += DECELERATION * maxSpeed_ / SCREEN_FRAMERATE;
		if ( velocity_[0] > 0.0 )
			velocity_[0] = 0.0;
	}
}


void Player::handleBorderCollisions ( double levelWidth, double levelHeight )
{
	if ( position_[0] - radius_ <= 0.0 ) {
		position_[0] = 0.0 + radius_;
		velocity_[0] = 0.0;
	}
	if ( position_[0] + radius_ >= levelWidth ) {
		position_[0] = levelWidth - radius_;
		velocity_[0] = 0.0;
	}
	if ( position_[1] - radius_ <= 0.0 ) {
		position_[1] = 0.0 + radius_;
		velocity_[1] = 0.0;
	}
	if ( position_[1] + radius_ >= levelHeight ) {
		position_[1] = levelHeight - radius_;
		velocity_[1] = 0.0;
	}
}


void Player::handleObstacleCollisions ( Obstacle* obstacles )
{
	for ( int i = 0; i < Obstacle::count; i ++ ) {
		Sdl::CollisionData results;
		results = testCollision({position_[0], position_[1], radius_},
		                        obstacles[i].getRectangle());
		
		if ( results.flags & Sdl::COLLISION_TRUE ) {
			Vector distance = position_ - Vector({results.closestXToFirst,
			                                     results.closestYToFirst});
			distance.normalise();
			
			position_ = Vector({results.closestXToFirst,
			                   results.closestYToFirst}) + distance * radius_;
			
			if (
			results.flags & Sdl::COLLISION_X_POS or
			results.flags & Sdl::COLLISION_X_NEG )
				velocity_[0] = 0.0;
			else if (
			results.flags & Sdl::COLLISION_Y_POS or
			results.flags & Sdl::COLLISION_Y_NEG )
				velocity_[1] = 0.0;
		}
	}
}


inline
void Player::apply ( int clipIndex )
{
	Sdl::applySurface(spriteSheet_, screen, SCREEN_WIDTH / 2 - radius_,
	                  (SCREEN_HEIGHT + PANEL_HEIGHT) / 2 - radius_,
	                  &clips_[clipIndex]);
}


Player& Player::operator = ( const Player& model )
{
	position_ = model.position_;
	velocity_ = model.velocity_;
	radius_ = model.radius_;
	
	spriteSheet_ = SDL_DisplayFormatAlpha(model.spriteSheet_);
	
	delete[] clips_;
	clips_ = new SDL_Rect[CLIPS_COUNT];
	for ( int i = 0; i < CLIPS_COUNT; i ++ )
		clips_[i] = model.clips_[i];
	
	healthPoints_ = model.healthPoints_;
	armorPoints_ = model.armorPoints_;
	staminaPoints_ = model.staminaPoints_;
	
	maxSpeed_ = model.maxSpeed_;
	
	return *this;
}




Obstacle::Obstacle ( )
{
	width_ = 0.0;
	height_ = 0.0;
	
	spriteSheet_ = NULL;
	
	count ++;
}


Obstacle::Obstacle ( string imageFileName, Vector initPosition,
                     double initWidth, double initHeight )
{
	position_ = initPosition;
	width_ = initWidth;
	height_ = initHeight;
	
	spriteSheet_ = Sdl::loadImage(imageFileName);
	
	count ++;
}


inline
Obstacle::~Obstacle ( )
{
	SDL_FreeSurface(spriteSheet_);
	
	count --;
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
