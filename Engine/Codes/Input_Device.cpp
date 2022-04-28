#include "Input_Device.h"

_USING(Engine)

_IMPLEMENT_SINGLETON(CInput_Device)


CInput_Device::CInput_Device()
{
}

HRESULT CInput_Device::Ready_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	if(FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyBoard, nullptr)))
		return E_FAIL;
	if(FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	if(FAILED(m_pKeyBoard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	if(FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	if(FAILED(m_pKeyBoard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;
	if(FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	if(FAILED(m_pKeyBoard->Acquire()))
		return E_FAIL;
	if(FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CInput_Device::SetUp_Input_State(void)
{
	if (nullptr == m_pKeyBoard
		|| nullptr == m_pMouse)
		return E_FAIL;

	m_pKeyBoard->GetDeviceState(256, m_KeyState);
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);

	return NOERROR;
}

void CInput_Device::Free(void)
{
	if (0 != Safe_Release(m_pMouse))
		_MSG_BOX("m_pMouse Release Failed");

	if (0 != Safe_Release(m_pKeyBoard))
		_MSG_BOX("m_pKeyBoard Release Failed");

	if (0 != Safe_Release(m_pInputSDK))
		_MSG_BOX("m_pInputSDK Release Failed");
}
