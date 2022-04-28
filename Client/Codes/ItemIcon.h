#pragma once
#include "GameObject.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CMesh_Static;
	class CRect_Texture;
	class CTexture;
	class CShader;
}

class CItemIcon final : public CGameObject
{
private:
	explicit CItemIcon(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CItemIcon(const CItemIcon& rhs);
	virtual ~CItemIcon(void) = default;
public:
	ITEMINFO* Get_ItemInfo(void) {
		return &m_tUnitInfo;	}
public:
	void Set_ItemInfo(const _tchar* pMeshTag, const ITEMINFO* pInfo);
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(CTransform* pTransform, LPD3DXEFFECT pEffect);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CTransform*				m_pGraphicTransformCom = nullptr;
	CMesh_Static*			m_pMeshCom = nullptr;
	CRect_Texture*			m_pBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CShader*				m_pGraphicShaderCom = nullptr;
private:
	ITEMINFO				m_tUnitInfo;
public:
	static CItemIcon* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

