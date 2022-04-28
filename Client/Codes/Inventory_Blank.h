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
	class CSound_Device;
}

class CInventory_Blank final : public CGameObject
{
public:
	enum BELONG {	BELONG_INVEN, BELONG_PROFILE, BELONG_END	};
private:
	explicit CInventory_Blank(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CInventory_Blank(const CInventory_Blank& rhs);
	virtual ~CInventory_Blank(void) = default;
public:
	_bool Get_isChecked(void) {
		return m_isChecked;	}
	_uint Get_iIndex(void) {
		return m_iIdx;	}
public:
	void Set_isChecked(const _bool& isChecked) {
		m_isChecked = isChecked;	}
	void Set_isEmpty(const _bool& isEmpty) {
		m_isEmpty = isEmpty;	}
	void Set_isSeeAble(const _bool& isSeeAble) {
		m_isSee = isSeeAble;	}
public:
	HRESULT	Set_InventoryBlank(const _tchar* pItemTextrureComTag, const _uint& iIdx, BELONG eBelong);
private:
	virtual HRESULT Ready_GameObject_Prototype(void);
	virtual HRESULT Ready_GameObject_Clone(const _vec3* pPos = nullptr);
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld);
	HRESULT Ready_Component(void);
	HRESULT SetUp_ConstantTable(LPD3DXEFFECT pEffect);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
	virtual void Render_GameObject(void);
private:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRect_Texture*			m_pBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRect_Texture*			m_pItemBufferCom = nullptr;
	CTexture*				m_pItemTextureCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CKeyChecker*			m_pKeyCheckerCom = nullptr;
	CObject_Manager*		m_pObject_Manager = nullptr;
	CSound_Device*			m_pSound_Device = nullptr;
private:
	_uint					m_iIdx = 0; //Inventory의 vector Idx
	_bool					m_isChecked = false;
	_bool					m_isEmpty = false;
	_bool					m_isSee = false;
	BELONG					m_eBelong = BELONG_END; //어디에 속해있는지
public:
	static CInventory_Blank* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual void Free(void);
};

