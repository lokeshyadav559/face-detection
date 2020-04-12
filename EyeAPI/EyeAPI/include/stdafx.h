// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#include "cv.h"
#include "highgui.h"
#include "direct.h"

#define BUFFERSIZE 5
#define DIR getcwd(NULL,0)
#ifndef MAXWIDTH
//capture source
enum {MEMORY=-1, CAMERA=0, MOVIE=1,PICTURE=2};
//resolution
#define MAXWIDTH 2000
#define MAXHEIGHT 2000
//player
enum {PLAY=0,PAUSE=1,STOP=2,RECORD=3};
#endif
