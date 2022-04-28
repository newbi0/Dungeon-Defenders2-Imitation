#pragma once

#include "Engine_Defines.h"
#include "Base.h"

_BEGIN(Engine)

class CScene;
class _ENGINE_DLL CManagement : public CBase
{
	_DECLARE_SINGLETON(CManagement)
private:
	explicit CManagement();
	virtual ~CManagement() = default;
public:
	HRESULT SetUp_CurrentScene(CScene* pScene);
	_int Update_CurrentScene(const _float& fTimeDelta);
	void Render_CurrentScene(void);
private:
	CScene*			m_pScene = nullptr;	
public:
	virtual void Free(void);
};

_END