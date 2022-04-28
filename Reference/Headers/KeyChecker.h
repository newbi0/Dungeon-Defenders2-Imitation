#pragma once
#include "Component.h"

_BEGIN(Engine)

class CInput_Device;
class _ENGINE_DLL CKeyChecker :
	public CComponent
{
public:
	enum MOUSESTATE { DIM_LBUTTON, DIM_RBUTTON, DIM_MBUTTON, DIM_XBUTTON, DIM_END };
private:
	explicit CKeyChecker(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CKeyChecker(const CKeyChecker& rhs);
	virtual ~CKeyChecker(void) = default;
public:
	_bool Check_MouseButton_Once(const  MOUSESTATE& eMouseButton);
	_bool Check_KeyBoardButton_Once(const _uint& iDIKButton);
	_bool isMouse_InClient(void);
	_bool isMouse_InRect(HWND hWnd, RECT rcRect);
private:
	CInput_Device*		m_pInput_Device = nullptr;
private:
	_bool				m_isMouseButton[DIM_END];
	_bool				m_isKeyboardButton[0xEE];
	RECT				m_rectClient;
public:
	static CKeyChecker* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const RECT& rectClient);
	virtual CKeyChecker* Clone(void);
private:
	HRESULT Ready_KeyChecker(const RECT& rectClient);
	virtual void Free(void);
};

_END