#pragma once

#pragma warning (disable : 4251)

#include <d3dx9.h>
#include <d3d9.h>

#include <mmsystem.h>
#include <dsound.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <algorithm>

using namespace std;

#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

#pragma comment(lib,"dsound")
#pragma comment(lib,"winmm")


#include "Engine_Typedef.h"
#include "Engine_Macro.h"
#include "Engine_Function.h"
#include "Engine_Functor.h"
#include "Engine_Struct.h"

using namespace Engine;




#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h> 
#include <crtdbg.h>

#endif

#ifdef _DEBUG 
#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#endif
#endif