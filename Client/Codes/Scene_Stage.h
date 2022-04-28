#pragma once
#include "Scene.h"

namespace Engine
{
	class CInput_Device;
	class CNavigation;
}

class CTurn;
class CScene_Stage final : public CScene
{
private:
	explicit CScene_Stage(LPDIRECT3DDEVICE9 pGraphic_Device, Engine::CRenderer* pRenderer);
	virtual ~CScene_Stage(void) = default;
private:
	virtual HRESULT Ready_Scene(void);
	HRESULT Ready_Component(void);
	HRESULT Ready_Prototype_GameObject(void);
	HRESULT Ready_Layer_GameObject(void);
public:
	virtual _int Update_Scene(const _float& fTimeDelta);
	virtual _int LastUpdate_Scene(const _float & fTimeDelta);
	virtual void Render_Scene(void);
private:
	CInput_Device*		m_pInput_Device = nullptr;
	CNavigation*		m_pNaviCom = nullptr;
	CTurn*				m_pTurn = nullptr;
public:
	static CScene_Stage* Create(LPDIRECT3DDEVICE9 pGraphic_Device, Engine::CRenderer* pRenderer);
private:
	virtual void Free(void);
};

