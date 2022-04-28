#pragma once

#include "Defines.h"
#include "Base.h"
#include "Graphic_Device.h"

namespace Engine
{
	class CComponent_Manager;
	class CObject_Manager;
	class CManagement;
}

class CMainApp : public CBase
{
public:
	explicit CMainApp(void);
	virtual ~CMainApp(void) = default;
public:
	HRESULT Ready_MainApp(void);
	_int Update_MainApp(const _float& fTimeDelta);
	void Render_MainApp(void);
private:
	//함수 인자는 전방선언X
	HRESULT Ready_DefaultSetting(CGraphic_Device::WINMODE eMode, const _uint& iBackSizeX, const _uint& iBackSizeY);
	HRESULT Ready_StaticComponent(void);
	HRESULT Ready_StartScene(void);
private:
	LPDIRECT3DDEVICE9	m_pGraphic_Device = nullptr;
private:
	CComponent_Manager*	m_pComponentMgr = nullptr;
	CObject_Manager*	m_pObjectMgr = nullptr;
	CManagement*		m_pManagement = nullptr;
private:
	_ulong				m_dwRenderCnt = 0;
	_float				m_fTimeAcc = 0.f;
	_tchar				m_szFPS[128] = L"";
public:
	static CMainApp* Create(void);
protected:
	virtual void Free(void);
};