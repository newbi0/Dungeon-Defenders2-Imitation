#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Renderer.h"
#include "Object_Manager.h"
#include "Component_Manager.h"

// �� ���� �θ𰡵ȴ�.

_BEGIN(Engine)

class _ENGINE_DLL CScene : public CBase
{
protected:
	explicit CScene(LPDIRECT3DDEVICE9 pGraphic_Device, CRenderer* pRenderer);
	virtual ~CScene();

public: // �����δ� �ڽĿ��� �������̵��ؼ� �����Ǿ��� �Լ���
	virtual HRESULT Ready_Scene(void); 
	virtual _int Update_Scene(const _float& fTimeDelta);
	virtual _int LastUpdate_Scene(const _float& fTimeDelta);
	virtual void Render_Scene(void);
protected:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;
	CRenderer*					m_pRenderer = nullptr;
	CObject_Manager*			m_pObject_Manager = nullptr;
	CComponent_Manager*			m_pComponent_Manager = nullptr;
protected:

protected:
	virtual void Free(void);
};

_END