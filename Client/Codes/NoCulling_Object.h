#pragma once
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CMesh_Static;
	class CShader;
}

class CNoCulling_Object final : public CGameObject
{
private:
	explicit CNoCulling_Object(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CNoCulling_Object(const CNoCulling_Object& rhs);
	virtual ~CNoCulling_Object(void) = default;
private:
	virtual HRESULT Ready_GameObject_Prototype(const _tchar* pMashTag);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(const _tchar* pMashTag);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_LightAndMaterialInfo_OnShader(LPD3DXEFFECT pEffect);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CMesh_Static*			m_pMeshCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
private:
	_vec3					m_vHitZonePos;
	_float					m_fHitZoneSize = 0.f;
public:
	static CNoCulling_Object* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pMashTag);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

