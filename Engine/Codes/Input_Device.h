#pragma once
#include "Base.h"

_BEGIN(Engine)

class _ENGINE_DLL CInput_Device final :
	public CBase
{
	_DECLARE_SINGLETON(CInput_Device)
public:
	enum MOUSESTATE { DIM_LBUTTON, DIM_RBUTTON, DIM_MBUTTON, DIM_XBUTTON, DIM_END };
	enum MOUSEMOVE { DIMM_X, DIMM_Y, DIMM_Z, DIMM_END };
private:
	explicit CInput_Device(void);
	virtual ~CInput_Device(void) = default;
public:
	_byte Get_KeyState(_ubyte byIndex) {
		return m_KeyState[byIndex];	}
	_byte Get_MouseState(_ubyte byIndex) {
		return m_MouseState.rgbButtons[byIndex];	}
	_long Get_MouseMove(_ubyte byIndex) {
		return *((_long*)&m_MouseState + byIndex);	}
public:
	HRESULT Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	HRESULT SetUp_Input_State(void);
private:
	LPDIRECTINPUT8			m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8	m_pKeyBoard = nullptr;
	LPDIRECTINPUTDEVICE8	m_pMouse = nullptr;
private:
	_byte					m_KeyState[256];
	DIMOUSESTATE			m_MouseState;
private:
	virtual void Free(void);
};

_END