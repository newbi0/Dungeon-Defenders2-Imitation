#include "stdafx.h"
#include "Defines.h"
#include "Profile.h"
#include "Renderer.h"
#include "Transform.h"
#include "Rect_Texture.h"
#include "Texture.h"
#include "Shader.h"
#include "KeyChecker.h"
#include "Unit.h"

#include "Object_Manager.h"
#include "Inventory_Blank.h"
#include "Inventory_Button.h"
#include "Inventory.h"

const _uint BLANK_MAX_NUM = 4;

CEquipmentBox::CEquipmentBox(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CEquipmentBox::CEquipmentBox(const CEquipmentBox & rhs)
	: CUI(rhs)
	, m_vecItem(rhs.m_vecItem)
{
	for (_uint i = 0; i < BLANK_MAX_NUM; ++i)
		memcpy(&m_vBlankPosition[i], rhs.m_vBlankPosition[i], sizeof(_vec3));
}

void CEquipmentBox::Set_SeeAble(_bool isSeeAble)
{
	if (m_isSee == isSeeAble)
		return;

	m_isSee = isSeeAble;

	if (true == m_isSee)
	{
		//자리 세팅
		m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_Inventory_Blank");
		Set_EquipmentBox();
	}
	
	//지우는 작업
	else
	{
		m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_Inventory_Blank");
	}
}

HRESULT CEquipmentBox::Add_Item(ITEMINFO * pItemInfo)
{
	if (nullptr == pItemInfo)
		return E_FAIL;

	ITEMINFO*	pNewItemInfo = new ITEMINFO;
	memcpy(pNewItemInfo, pItemInfo, sizeof(ITEMINFO));

	m_vecItem.push_back(pNewItemInfo);

	return NOERROR;
}

HRESULT CEquipmentBox::Delete_Item(const _uint & iIdx)
{
	auto iter = m_vecItem.begin();

	for (_uint i = 0; i < iIdx; ++i)
		++iter;

	m_vecItem.erase(iter);

	return NOERROR;
}

void CEquipmentBox::Update_Profile(void)
{
	m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_Inventory_Blank");

	Set_EquipmentBox();
}

HRESULT CEquipmentBox::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_vBlankPosition[ITEM_DEF_CHEST] = _vec3(0.f, 0.f, 0.f);
	m_vBlankPosition[ITEM_DEF_BOOTS] = _vec3(0.1f, -0.5f, 0.f);
	m_vBlankPosition[ITEM_DEF_HEAD] = _vec3(0.05f, 0.4f, 0.f);
	m_vBlankPosition[ITEM_ATT_WEAPON] = _vec3(-0.15f, 0.2f, 0.f);

	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(0.f, 0.f, 0.f));
	m_fViewZ = 0.3f;

	return NOERROR;
}

HRESULT CEquipmentBox::Ready_GameObject_Clone(const _vec3 * pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer");
	m_pBufferCom->AddRef();

	m_pTextureCom = (Engine::CTexture*)Get_Component(L"Com_Texture");
	m_pTextureCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pKeyCheckerCom = (Engine::CKeyChecker*)Get_Component(L"Com_KeyChecker");
	m_pKeyCheckerCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	m_pObject_Manager->AddRef();

	m_pBufferCom->Resize_Buffer(_vec2(-0.3f, -0.8f), _vec2(0.3f, 0.8f));


	return NOERROR;
}

HRESULT CEquipmentBox::Ready_GameObject_Clone(const _matrix & matWorld)
{
	Ready_GameObject_Clone();
	return NOERROR;
}

HRESULT CEquipmentBox::Ready_Component(void)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	// For.Com_Renderer
	m_pRendererCom = (Engine::CRenderer*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Renderer");
	if (nullptr == m_pRendererCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Renderer", m_pRendererCom);

	// For.Com_Transform	
	m_pTransformCom = (Engine::CTransform*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Transform");
	if (nullptr == m_pTransformCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Transform", m_pTransformCom);

	// For.Com_Buffer
	m_pBufferCom = (Engine::CRect_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Buffer_RectTexture");
	if (nullptr == m_pBufferCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Buffer", m_pBufferCom);

	// For.Com_Textrue
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_ProfileBG");
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Shader_UI");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	// For.Com_KeyChecker
	m_pKeyCheckerCom = (Engine::CKeyChecker*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_KeyChecker");
	if (nullptr == m_pKeyCheckerCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_KeyChecker", m_pKeyCheckerCom);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	m_pObject_Manager->AddRef();

	return NOERROR;
}

void CEquipmentBox::Set_EquipmentBox(void)
{
	for (_uint i = 0; i < BLANK_MAX_NUM; ++i)
	{
		if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Inventory_Blank", L"Object_Inventory_Blank", &m_vBlankPosition[i])))
			return;
	}

	_uint	iIdx = 0;

	for (auto pInfo : m_vecItem)
	{
		switch (pInfo->iItemKind)
		{
		case(CInventory::ITEM_DEF_CHEST):
		{
			Set_Blank(m_vBlankPosition[ITEM_DEF_CHEST], iIdx);
			break;
		}
		case(CInventory::ITEM_DEF_BOOTS):
		{
			Set_Blank(m_vBlankPosition[ITEM_DEF_BOOTS], iIdx);
			break;
		}
		default:
			break;
		}
		++iIdx;
	}
}

void CEquipmentBox::Set_Blank(const _vec3& vPos, const _uint& iIdx)
{
	CInventory_Blank*	pBlank = (CInventory_Blank*)m_pObject_Manager->Get_GameObject_ApplyToPosition(SCENE_STAGE, L"Layer_Inventory_Blank", &vPos);
	if (nullptr == pBlank)
		return;

	_tchar	szTextureTag[128] = L"Com_Texture_Icon_";
	lstrcat(szTextureTag, m_vecItem[iIdx]->pItemName);

	pBlank->Set_InventoryBlank(szTextureTag, iIdx, CInventory_Blank::BELONG_PROFILE);

}


_int CEquipmentBox::Update_GameObject(const _float & fTimeDelta)
{
	if (true == m_isSee)
	{
		if (true == m_pKeyCheckerCom->Check_MouseButton_Once(CKeyChecker::DIM_LBUTTON))
		{

		}
	}
	return _RUN;
}

_int CEquipmentBox::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (true == m_isSee)
	{
		if (nullptr == m_pRendererCom)
			return _EXIT;

		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this);
	}

	return _RUN;
}

void CEquipmentBox::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pTextureCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pBufferCom)
		return;

	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld")))
		return;

	_matrix			matTmp;
	D3DXMatrixIdentity(&matTmp);

	pEffect->SetMatrix("g_matView", &matTmp);
	pEffect->SetMatrix("g_matProj", &matTmp);

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture")))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();	
}

CEquipmentBox * CEquipmentBox::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CEquipmentBox* pInstance = new CEquipmentBox(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CEquipmentBox Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquipmentBox::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CEquipmentBox(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CEquipmentBox Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEquipmentBox::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CEquipmentBox(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CEquipmentBox Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEquipmentBox::Free(void)
{
	for (auto pItem : m_vecItem)
	{
		Safe_Delete(pItem);
	}

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pKeyCheckerCom);

	CUI::Free();
}
