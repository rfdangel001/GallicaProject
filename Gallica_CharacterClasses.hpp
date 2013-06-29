////////////////////////////////////////////////////////////////////////////////
//        FILE : Gallica_CharacterClasses.hpp
//      AUTHOR : Charles Hosson & Rada Florin-Daniel
//        DATE :   Creation : June 22 2013
//               Last Entry : June 22 2013
// DESCRIPTION : Defines the classes for the characters and scoreboard
//               in the Gallica project.
////////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace std;


#include <cmath>
#include <string>
#include <vector>

#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
#include "powerUps.hpp"

#include "LinearAlgebra.hpp"

// scoreboard class just basic for now
class ScoreBoard
{
  public:
		ScoreBoard(int x, int y, int length, int width);
		~ScoreBoard();
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
		std::string playerName_; // player name
		int score_; // The actual score
		int posX_; // X Position of scoreboard window while ingame
		int posY_; // Y Position of scoreboard window while ingame
		int length_; // Length of the scoreboard window while ingame
		int width_; // Length of the Scoreboard window while ingame



};

class Character
{
public:
	// Worry about that later...

protected:
	// Navigational attributes
	Vector position_;
	Vector speed_;
	double radius_;

	// Graphical attributes
	SDL_Surface* spriteSheet_;		// We don't worry about all the graphical
	SDL_Rect* headingClips_;		// stuff for now, since we need the artists'
	SDL_Rect* animationClips_;	// input for this.

	// Gameplay attributes
	int healthPoints_;
	int armorPoints_;
	int staminaPoints_;
};

class Player : public Character
{
public:
	Player();
	Player(std::string name);
	Player(Vector position, Vector speed, double radius, int health, int armor, int stamina);
	~Player();
	void AddPowerUp(powerUp* pUp); // add power up to player - used when picking up the powerup

	// setter
	void setPosition(Vector position){position_ = position;}
	void setPosition(initializer_list<double> ilD){position_ = ilD;}
	void setSpriteSheet(SDL_Surface* spriteSheet){spriteSheet_ = spriteSheet;}
	void setHeadingClips(SDL_Rect* headingClips){headingClips_ = headingClips;}
	void setAnimationClips(SDL_Rect* animationClips){animationClips_ = animationClips;}
	void setHealth(int health){healthPoints_ = health;}
	void setArmor(int armor){armorPoints_ = armor;}
	void setStamina(int stamina){staminaPoints_ = stamina;}
	void setName(std::string name){playerName_ = name;}
	void setSpeed(Vector speed){speed_ = speed;}
	void setSpeed(initializer_list<double> ilD){speed_ = ilD;}
	void setRadius(double radius){radius_ = radius;}
	// getters
	Vector getPosition(){return position_;}
	SDL_Surface* getSpriteSheet(){return spriteSheet_;}
	SDL_Rect* getHeadingClips(){return headingClips_;}
	SDL_Rect* getAnimationClips(){return animationClips_;}
	int getHealth(){return healthPoints_;}
	int getArmor(){return armorPoints_;}
	int getStamina(){return staminaPoints_;}
	std::string getName(){return playerName_;}
	Vector getSpeed(){return speed_;}
	double getRadius(){return radius_;}


protected:
    void removeNullPowerUps();
    std::vector<powerUp*> powerUpList_;
	std::string playerName_;
};

Player::Player(Vector position, Vector speed, double radius, int health, int armor, int stamina)
{
    position_ = position;
    speed_ = speed;
    radius_ = radius;
    healthPoints_ = health;
    armorPoints_ = armor;
    staminaPoints_ = stamina;
}

void Player::AddPowerUp(powerUp* pUp)
{
    powerUpList_.push_back(pUp);
}

void Player::removeNullPowerUps()
{
    std::vector<powerUp*> temp;
    for(int i = 0; i < powerUpList_.size();i++)
    {
        if(powerUpList_[i] == NULL)
        {
            continue;
        }
        else
        {
            temp.push_back(powerUpList_[i]);
        }
    }
    powerUpList_.clear();
    powerUpList_ = temp;
}
