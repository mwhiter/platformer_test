// base.h
// Defines basic data types

#pragma once

#ifndef __BASE_H_
#define __BASE_H_

#include <stdint.h>
#include <random>

// Basic types
typedef unsigned short	int word;
typedef unsigned		int dword;
typedef unsigned		int uint;
typedef std::mt19937		RNG;			// Mersenne Twister pseudo random number generator

#define PI 3.14159265358979323846

// Color definitions
#define COLOR_RED				255,0,0
#define COLOR_RED_ORANGE		255,69,0

#define COLOR_GREEN				0,255,0
#define COLOR_GREEN_LAWN		124,252,0
#define COLOR_GREEN_LIME		50,205,50
#define COLOR_GREEN_SEA			46,139,87
#define COLOR_GREEN_YELLOW		154,205,50
#define COLOR_GREEN_FOREST		34,139,34
#define COLOR_GREEN_KHAKI		189,183,107

#define COLOR_BLUE				0,0,255
#define COLOR_NAVY_BLUE			0,0,128
#define COLOR_SKY_BLUE			135,206,250
#define COLOR_CYAN				0,255,255

#define COLOR_ORANGE			255,165,0
#define COLOR_DARK_ORANGE		255,140,0
#define COLOR_ORANGE_RED		255,69,0

#define COLOR_YELLOW			255,255,0
#define COLOR_GOLD				255,215,0
#define COLOR_LIGHT_YELLOW		255,255,224
#define COLOR_GOLDENROD			218,165,32

#endif