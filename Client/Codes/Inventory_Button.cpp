#include "stdafx.h"
#include "Defines.h"
#include "Inventory_Blank.h"

#include "Renderer.h"
#include "Mesh_Static.h"
#include "Shader.h"
#include "Transform.h"
#include "Texture.h"
#include "Rect_Texture.h"
#include "KeyChecker.h"
#include "Object_Manager.h"
#include "Inventory_Button.h"
#include "Inventory.h"
#include "Profile.h"

#include "Unit.h"

CInventory_Button::CInventory_Button(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CInventory_Button::CInventory_Button(const CInventory_Button & rhs)
	: CGameObject(rhs)
	, m_eKind(rhs.m_eKind)
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
{
}

HRESULT CInventory_Button::Set_InventoryButton(BUTTONKIND eButtonKind)
{
	m_eKind = eButtonKind;

	switch(eButtonKind)
	{
	case(BUTTON_PUTTING):
	{
		// For.Com_Buffer
		m_pOrderFontBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer");
		m_pOrderFontBufferCom->AddRef();

		// For.Com_Texture
		m_pOrderFontTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Button_Font_Putting");
		if (nullptr == m_pOrderFontTextureCom)
			return E_FAIL;
		Engine::CGameObject::Add_Component(L"Com_OrderFontTexture", m_pOrderFontTextureCom);

		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(0.52f, 0.3f, 0.f));

		m_vMin = _vec2(-0.14f, -0.06f);
		m_vMax = _vec2(0.14f, 0.06f);

		m_pBufferCom->Resize_Buffer(m_vMin, m_vMax);
		m_pOrderFontBufferCom->Resize_Buffer(m_vMin, m_vMax);

		break;
	}
	case(BUTTON_THROWAWAY):
	{
		// For.Com_Buffer
		m_pOrderFontBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer");
		m_pOrderFontBufferCom->AddRef();

		// For.Com_Texture
		m_pOrderFontTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Button_Font_ThrowAway");
		if (nullptr == m_pOrderFontTextureCom)
			return E_FAIL;
		Engine::CGameObject::Add_Component(L"Com_OrderFontTexture", m_pOrderFontTextureCom);

		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(0.52f, 0.f, 0.f));

		m_vMin = _vec2(-0.14f, -0.06f);
		m_vMax = _vec2(0.14f, 0.06f);

		m_pBufferCom->Resize_Buffer(m_vMin, m_vMax);
		m_pOrderFontBufferCom->Resize_Buffer(m_vMin, m_vMax);

		break;
	}
	case(BUTTON_PROPAGE):
	{
		Safe_Release(m_pTextureCom);
			
		// For.Com_Texture
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Button_Left");
		if (nullptr == m_pTextureCom)
			return E_FAIL;
		Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(-0.3f, -0.6f, 0.f));

		m_vMin = _vec2(-0.05f, -0.05f);
		m_vMax = _vec2(0.05f, 0.05f);

		m_pBufferCom->Resize_Buffer(m_vMin, m_vMax);

		break;
	}
	case(BUTTON_NEXTPAGE):
	{
		Safe_Release(m_pTextureCom);

		// For.Com_Texture
		m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Button_Right");
		if (nullptr == m_pTextureCom)
			return E_FAIL;
		Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(0.f, -0.6f, 0.f));

		m_vMin = _vec2(-0.05f, -0.05f);
		m_vMax = _vec2(0.05f, 0.05f);

		m_pBufferCom->Resize_Buffer(m_vMin, m_vMax);

		break;
	}
	default:
		break;
	}

	return NOERROR;
}

HRESULT CInventory_Button::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fViewZ = 0.2f;

	return NOERROR;
}

HRESULT CInventory_Button::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer");
	m_pBufferCom->AddRef();

	m_pTextureCom = (Engine::CTexture*)Get_Component(L"Com_Texture");
	m_pTextureCom->AddRef();

	m_pKeyCheckerCom = (Engine::CKeyChecker*)Get_Component(L"Com_KeyChecker");
	m_pKeyCheckerCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	m_pObject_Manager->AddRef();

	if (nullptr != pPos)
	{
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);
	}

	return NOERROR;
}

HRESULT CInventory_Button::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	return NOERROR;
}

HRESULT CInventory_Button::Ready_Component(void)
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
	
	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Shader_UI");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	// For.Com_Buffer
	m_pBufferCom = (Engine::CRect_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Buffer_RectTexture");
	if (nullptr == m_pBufferCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Buffer", m_pBufferCom);

	// For.Com_Texture
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Button_Font_Frame");
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

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

HRESULT CInventory_Button::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
{
	if (FAILED(m_pTransformCom->SetUp_OnShader(pEffect, "g_matWorld")))
		return E_FAIL;

	_matrix		matTmp;
	D3DXMatrixIdentity(&matTmp);

	if (FAILED(pEffect->SetMatrix("g_matView", &matTmp)))
		return E_FAIL;
	if (FAILED(pEffect->SetMatrix("g_matProj", &matTmp)))
		return E_FAIL;

	return NOERROR;
}

void CInventory_Button::Putting(void)
{
	_uint iBlankNum = m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_Inventory_Blank");
	for (_uint i = 0; i < iBlankNum; ++i)
	{
		CInventory_Blank*	pBlank = (CInventory_Blank*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Inventory_Blank", i));
		if (nullptr == pBlank)
			continue;
		if (true == pBlank->Get_isChecked())
		{
			CEquipmentBox*	pProfile = (CEquipmentBox*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Profile"));
			if (nullptr == pProfile)
				break;

			CInventory*	pInven = (CInventory*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Inventory"));
			if (nullptr == pInven)
				break;

			_uint	iBlankIdx = pBlank->Get_iIndex();

			ITEMINFO*	pInfo = pInven->Get_ItemInfo(iBlankIdx);
			pProfile->Add_Item(pInfo);


			pInven->Delete_Item(iBlankIdx);
			pInven->Update_Inventory();

			CUnit*	pPlayer = (CUnit*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
			if (nullptr == pPlayer)
				break;

			UNITINFO	tNewInfo;
			ZeroMemory(&tNewInfo, sizeof(UNITINFO));

			if (CInventory::ITEM_DEF_CHEST == pInfo->iItemKind
				|| CInventory::ITEM_DEF_HEAD== pInfo->iItemKind
				|| CInventory::ITEM_DEF_BOOTS== pInfo->iItemKind)
			{
				tNewInfo.fDef = pInfo->fItemStats;
				pPlayer->Add_Info(&tNewInfo);
			}
			else if (CInventory::ITEM_ATT_WEAPON == pInfo->iItemKind)
			{
				tNewInfo.fAtt = pInfo->fItemStats;
				pPlayer->Add_Info(&tNewInfo);
			}

			CGameObject*	pSign = m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Sign_Check");
			if (nullptr != pSign)
				pSign->Set_isDead(true);

			break;
		}
	}
}

void CInventory_Button::ThrowAway(void)
{
	_uint iBlankNum = m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_Inventory_Blank");
	for (_uint i = 0; i < iBlankNum; ++i)
	{
		CInventory_Blank*	pBlank = (CInventory_Blank*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Inventory_Blank", i));
		if (nullptr == pBlank)
			continue;
		if (true == pBlank->Get_isChecked())
		{
			_uint	iBlankIdx = pBlank->Get_iIndex();
			CInventory*	pInven = (CInventory*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Inventory"));
			if (nullptr == pInven)
				break;
			pInven->Delete_Item(iBlankIdx);
			pInven->Update_Inventory();

			CGameObject*	pSign = m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Sign_Check");
			if (nullptr != pSign)
				pSign->Set_isDead(true);

			break;
		}
	}
}

void CInventory_Button::Turn_Page(_bool isNext)
{
	CInventory*	pInven = (CInventory*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Inventory"));
	if (nullptr == pInven)
		return;

	pInven->Turn_Page(isNext);
}

_int CInventory_Button::Update_GameObject(const _float & fTimeDelta)
{
	if (true == m_pKeyCheckerCom->Check_MouseButton_Once(CKeyChecker::DIM_LBUTTON))
	{
		RECT rcRect = { 0 };

		//Local Position
		_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

		rcRect.left = g_iBackCX * (1.f + vPos.x - m_vMax.x) / 2;
		rcRect.top = g_iBackCY * (1.f - vPos.y - m_vMax.y) / 2;
		rcRect.right = g_iBackCX * (1.f + vPos.x + m_vMax.x) / 2;
		rcRect.bottom = g_iBackCY * (1.f - vPos.y + m_vMax.y) / 2;

		if (true == m_pKeyCheckerCom->isMouse_InRect(g_hWnd, rcRect))
		{
			switch (m_eKind)
			{
			case(BUTTON_PUTTING):
			{
				Putting();
				break;
			}
			case(BUTTON_THROWAWAY):
			{
				ThrowAway();
				break;
			}
			case(BUTTON_PROPAGE):
			{
				Turn_Page(false);
				break;
			}
			case(BUTTON_NEXTPAGE):
			{
				Turn_Page(true);
				break;
			}
			default:
				break;
			}
		}
	}


	return _RUN;
}

_int CInventory_Button::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this);

	return _int();
}

void CInventory_Button::Render_GameObject(void)
{
	if (nullptr == m_pTransformCom
		|| nullptr == m_pShaderCom
		|| nullptr == m_pBufferCom
		|| nullptr == m_pTextureCom)
		return;

	//프레임
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	if (FAILED(SetUp_ConstantTable(pEffect)))
	{
		_MSG_BOX("CInventory_Button SetUp_ConstantTable Failed");
		return;
	}

	if (FAILED(m_pTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture")))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();

	//아이콘
	if (nullptr == m_pOrderFontBufferCom
		|| nullptr == m_pOrderFontTextureCom)
		return;
	
	if (FAILED(m_pOrderFontTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture")))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(1);

	m_pOrderFontBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();
}

CInventory_Button * CInventory_Button::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInventory_Button* pInstance = new CInventory_Button(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CInventory_Button Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInventory_Button::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CInventory_Button(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CInventory_Button Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CInventory_Button::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CInventory_Button(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CInventory_Button Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInventory_Button::Free(void)
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pKeyCheckerCom);
	Safe_Release(m_pObject_Manager);

	if(nullptr != m_pOrderFontBufferCom)
		Safe_Release(m_pOrderFontBufferCom);
	if (nullptr != m_pOrderFontTextureCom)
		Safe_Release(m_pOrderFontTextureCom);

	CGameObject::Free();
}
