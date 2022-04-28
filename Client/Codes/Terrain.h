#pragma once
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CTerrain_Buffer;
	class CTexture;
	class CShader;
}

class CTerrain :
	public CGameObject
{
private:
	explicit CTerrain(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTerrain(const CTerrain& rhs);
	virtual ~CTerrain(void) = default;
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
	HRESULT SetUp_LightAndMaterialInfo_OnShader(LPD3DXEFFECT pEffect);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	CRenderer*			m_pRendererCom	= nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CTerrain_Buffer*	m_pBufferCom	= nullptr;
	CTexture*			m_pTextureCom	= nullptr;
	CShader*			m_pShaderCom	= nullptr;
public:
	static CTerrain* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

