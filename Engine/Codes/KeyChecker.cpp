#include "KeyChecker.h"
#include "Input_Device.h"


CKeyChecker::CKeyChecker(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CKeyChecker::CKeyChecker(const CKeyChecker & rhs)
	: CComponent(rhs)
{
	m_rectClient = rhs.m_rectClient;

	m_pInput_Device = CInput_Device::Get_Instance();
	m_pInput_Device->AddRef();

	memcpy(&m_isKeyboardButton, &rhs.m_isKeyboardButton, sizeof(_bool) * 0xEE);
	memcpy(&m_isMouseButton, &rhs.m_isMouseButton, sizeof(_bool) * DIM_END);
}

_bool CKeyChecker::Check_MouseButton_Once(const MOUSESTATE & eMouseButton)
{
	if (m_pInput_Device->Get_MouseState(eMouseButton) & 0x80)
	{
		if (true == m_isMouseButton[eMouseButton])
			return false;

		if (false == isMouse_InClient())
			return false;

		m_isMouseButton[eMouseButton] = true;
		return true;
	}

	m_isMouseButton[eMouseButton] = false;

	return m_isMouseButton[eMouseButton];
}

_bool CKeyChecker::Check_KeyBoardButton_Once(const _uint & iDIKButton)
{
	if (m_pInput_Device->Get_KeyState(iDIKButton) & 0x80)
	{
		if (true == m_isKeyboardButton[iDIKButton])
			return false;

		m_isKeyboardButton[iDIKButton] = true;
		return true;
	}

	m_isKeyboardButton[iDIKButton] = false;

	return m_isKeyboardButton[iDIKButton];
}

_bool CKeyChecker::isMouse_InClient(void)
{
	POINT	ptMouse;
	GetCursorPos(&ptMouse);

	if (false == PtInRect(&m_rectClient, ptMouse))
		return false;

	return true;
}

_bool CKeyChecker::isMouse_InRect(HWND hWnd, RECT rcRect)
{
	POINT	ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	if (false == PtInRect(&rcRect, ptMouse))
		return false;

	return true;
}

CKeyChecker * CKeyChecker::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const RECT& rectClient)
{
	CKeyChecker*	pInstance = new CKeyChecker(pGraphic_Device);
	if (FAILED(pInstance->Ready_KeyChecker( rectClient)))
	{
		_MSG_BOX("CKeyChecker Creating Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CKeyChecker * CKeyChecker::Clone(void)
{
	return new CKeyChecker(*this);
}

HRESULT CKeyChecker::Ready_KeyChecker(const RECT& rectClient)
{
	m_rectClient = rectClient;

	m_pInput_Device = CInput_Device::Get_Instance();
	if (nullptr == m_pInput_Device)
		return E_FAIL;
	m_pInput_Device->AddRef();

	ZeroMemory(&m_isMouseButton, sizeof(_bool) * DIM_END);
	ZeroMemory(&m_isKeyboardButton, sizeof(_bool) * 0xEE);

	return NOERROR;
}

void CKeyChecker::Free(void)
{
	Safe_Release(m_pInput_Device);

	CComponent::Free();
}
