////////////////////////////////////////////////////////////////////////////////
//        FILE : Gallica_Globals.hpp
//      AUTHOR : Charles Hosson
//        DATE :   Creation : July 28 2013
//               Last Entry : August 1 2013
// DESCRIPTION : Contains all the constants and global variables used by the
//               Gallica program.
////////////////////////////////////////////////////////////////////////////////

#pragma once

using namespace std;


#include <string>
#include <cmath>




SDL_Surface* screen = NULL;




const int LEVEL_WIDTH = 2000;
const int LEVEL_HEIGHT = 2000;

const    int SCREEN_WIDTH = 1200;
const    int SCREEN_HEIGHT = 700;
const    int SCREEN_BPP = 32;
const double SCREEN_FRAMERATE = 40.0;

const int PANEL_HEIGHT = 100;


const string UI_FONT_FILENAME = "TestFont.ttf";
const string INTRO_FONT_FILENAME = "TestFont.ttf";
const string MENU_FONT_FILENAME = "TestFont.ttf";
const string PAUSE_FONT_FILENAME = "TestFont.ttf";

const string INTRO_BACKGROUND_FILENAME = "IntroBackground.png";


enum StateCode {STATE_NULL,
                STATE_LEVEL_1,
                STATE_LEVEL_2,
                STATE_EXIT,
                STATE_INTRO,
                STATE_MAIN_MENU};

