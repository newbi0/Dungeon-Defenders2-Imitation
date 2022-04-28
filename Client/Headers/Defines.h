#pragma once

extern HWND g_hWnd;
extern HINSTANCE g_hInst;

enum SCENEID { SCENE_STATIC, SCENE_LOGO, SCENE_STAGE, SCENE_END };
enum eTURN { TURN_BUILD, TURN_WAVE, TURN_END };

const unsigned int g_iBackCX = 1280;
const unsigned int g_iBackCY = 768;

#pragma comment (lib, "Engine.lib")

#include <process.h>
#include <stdlib.h>
#include <time.h>

#include "Engine_Defines.h"
#include "Struct.h"