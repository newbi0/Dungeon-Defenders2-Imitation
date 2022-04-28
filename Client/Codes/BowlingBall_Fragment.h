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

class CBowlingBall_Fragment final : public CUnit
{
public:
	enum BALLTYPE { TYPE_FULL, TYPE_FRAGMENT_0, TYPE_FRAGMENT_1, TYPE_FRAGMENT_2, TYPE_FRAGMENT_3, TYPE_FRAGMENT_4, TYPE_END };
private:
	explicit CBowlingBall_Fragment(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CBowlingBall_Fragment(const CBowlingBall_Fragment& rhs);
	virtual ~CBowlingBall_Fragment(void) = default;
public:
	void Set_Scaling(const _vec3& vScale);
	virtual void Add_Hp(const _float& fHpAmount);
private:
	virtual HRESULT Ready_GameObject_Prototype(const _tchar* pMashTag, const BALLTYPE& eType, const _bool& isBoss);
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
private:
	BALLTYPE				m_eType = TYPE_END;
	_bool					m_isBoss = false;
public:
	static CBowlingBall_Fragment* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pMashTag, const BALLTYPE& eType, const _bool& isBoss);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

