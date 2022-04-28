#pragma once
#include "UI.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CRect_Texture;
	class CTexture;
	class CShader;
	class CObject_Manager;
}

class CInventory final : public CUI
{
public:
	enum ITEMKIND	{ ITEM_DEF_HEAD, ITEM_DEF_CHEST, ITEM_DEF_BOOTS, ITEM_ATT_WEAPON, ITEM_END	};
private:
	explicit CInventory(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CInventory(const CInventory& rhs);
	virtual ~CInventory(void) = default;
public:
	ITEMINFO* Get_ItemInfo(const _uint& iIdx) {
		return m_vecItem[iIdx];	}
public:
	virtual void Set_SeeAble(_bool isSeeAble);
	void Turn_Page(_bool isNext);
public:
	HRESULT Add_Item(ITEMINFO* pItemInfo);
	HRESULT Delete_Item(const _uint& iIdx);
	void Update_Inventory(void);
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
private:
	void Set_Blank(void);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRect_Texture*			m_pBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRect_Texture*			m_pPageBufferCom = nullptr;
	CTexture*				m_pPageTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CObject_Manager*		m_pObject_Manager = nullptr;
private:
	_uint					m_iPageIdx = 1;
	vector<ITEMINFO*>		m_vecItem;
public:
	static CInventory* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

