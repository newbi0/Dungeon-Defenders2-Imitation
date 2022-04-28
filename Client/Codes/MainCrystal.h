#pragma once
#include "Unit.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CMesh_Static;
	class CShader;
	class CFrustum_Culling;
	class CCollider;
	class CObject_Manager;
}

class CMainCrystal final : public CUnit
{
private:
	explicit CMainCrystal(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMainCrystal(const CMainCrystal& rhs);
	virtual ~CMainCrystal(void) = default;
private:
	virtual HRESULT Ready_GameObject_Prototype(const _tchar* pMashTag);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(const _tchar* pMashTag);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CMesh_Static*			m_pMeshCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CFrustum_Culling*		m_pCullingCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CObject_Manager*		m_pObject_Manager = nullptr;
public:
	static CMainCrystal* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pMashTag);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

