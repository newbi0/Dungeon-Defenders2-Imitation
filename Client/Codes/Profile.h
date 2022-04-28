#pragma once
#include "UI.h"

namespace Engine
{
	class CRenderer;
	class CTransform;
	class CRect_Texture;
	class CTexture;
	class CShader;
	class CKeyChecker;
	class CObject_Manager;
}

class CEquipmentBox final : public CUI
{
public:
	enum ITEMKIND { ITEM_DEF_HEAD, ITEM_DEF_CHEST, ITEM_DEF_BOOTS, ITEM_ATT_WEAPON, ITEM_END };
private:
	explicit CEquipmentBox(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CEquipmentBox(const CEquipmentBox& rhs);
	virtual ~CEquipmentBox(void) = default;
public:
	virtual void Set_SeeAble(_bool isSeeAble);
	ITEMINFO* Get_ItemInfo(const _uint& iIdx) {
		return m_vecItem[iIdx];	}
public:
	HRESULT Add_Item(ITEMINFO* pItemInfo);
	HRESULT Delete_Item(const _uint& iIdx);
	void Update_Profile(void);
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
private:
	void Set_EquipmentBox(void);
	void Set_Blank(const _vec3& vPos, const _uint& iIdx);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRect_Texture*			m_pBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CKeyChecker*			m_pKeyCheckerCom = nullptr;
	CObject_Manager*		m_pObject_Manager = nullptr;
	vector<ITEMINFO*>		m_vecItem;
	_vec3					m_vBlankPosition[4];
public:
	static CEquipmentBox* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

