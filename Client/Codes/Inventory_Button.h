#pragma once
#include "GameObject.h"

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

class CInventory_Button final : public CGameObject
{
public:
	enum BUTTONKIND {	BUTTON_PUTTING, BUTTON_THROWAWAY, BUTTON_PROPAGE, BUTTON_NEXTPAGE, BUTTON_JUSTFONT, BUTTON_END	};
private:
	explicit CInventory_Button(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CInventory_Button(const CInventory_Button& rhs);
	virtual ~CInventory_Button(void) = default;
public:
	HRESULT	Set_InventoryButton(BUTTONKIND eButtonKind);
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
private:
	void Putting(void);
	void ThrowAway(void);
	void Turn_Page(_bool isNext);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRect_Texture*			m_pBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRect_Texture*			m_pOrderFontBufferCom = nullptr;
	CTexture*				m_pOrderFontTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CKeyChecker*			m_pKeyCheckerCom = nullptr;
	CObject_Manager*		m_pObject_Manager = nullptr;
private:
	_vec2					m_vMin, m_vMax;
	BUTTONKIND				m_eKind = BUTTON_END;
public:
	static CInventory_Button* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

