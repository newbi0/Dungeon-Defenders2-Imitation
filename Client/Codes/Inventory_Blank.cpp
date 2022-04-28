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
#include "Sound_Device.h"

#include "Inventory.h"
#include "Profile.h"
#include "Unit.h"

CInventory_Blank::CInventory_Blank(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CInventory_Blank::CInventory_Blank(const CInventory_Blank & rhs)
	: CGameObject(rhs)
	, m_eBelong(rhs.m_eBelong)
{
}

HRESULT CInventory_Blank::Set_InventoryBlank(const _tchar * pItemTextrureComTag, const _uint& iIdx, BELONG eBelong)
{
	m_iIdx = iIdx;
	m_eBelong = eBelong;

	if (nullptr == pItemTextrureComTag)
		return E_FAIL;

	m_pItemTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, pItemTextrureComTag);
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_ItemTexture", m_pTextureCom);

	return NOERROR;
}

HRESULT CInventory_Blank::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_fViewZ = 0.2f;

	return NOERROR;
}

HRESULT CInventory_Blank::Ready_GameObject_Clone(const _vec3* pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer");
	m_pBufferCom->AddRef();

	m_pItemBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_ItemBuffer");
	m_pItemBufferCom->AddRef();

	m_pTextureCom = (Engine::CTexture*)Get_Component(L"Com_Texture");
	m_pTextureCom->AddRef();

	m_pKeyCheckerCom = (Engine::CKeyChecker*)Get_Component(L"Com_KeyChecker");
	m_pKeyCheckerCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	m_pObject_Manager->AddRef();

	m_pSound_Device = CSound_Device::Get_Instance();
	m_pSound_Device->AddRef();

	if (nullptr != pPos)
	{
		m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, pPos);
	}

	m_pBufferCom->Resize_Buffer(_vec2(-0.1f, -0.15f), _vec2(0.1f, 0.15f));
	m_pItemBufferCom->Resize_Buffer(_vec2(-0.1f, -0.15f), _vec2(0.1f, 0.15f));

	m_isSee = true;

	return NOERROR;
}

HRESULT CInventory_Blank::Ready_GameObject_Clone(const _matrix& matWorld)
{
	Ready_GameObject_Clone();

	return NOERROR;
}

HRESULT CInventory_Blank::Ready_Component(void)
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

	// For.Com_Buffer
	m_pItemBufferCom = (Engine::CRect_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Buffer_RectTexture");
	if (nullptr == m_pItemBufferCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_ItemBuffer", m_pItemBufferCom);

	// For.Com_Texture
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Inventory_Blank");
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

	//For.Com_KeyChecker
	m_pKeyCheckerCom = (Engine::CKeyChecker*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_KeyChecker");
	if (nullptr == m_pKeyCheckerCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_KeyChecker", m_pKeyCheckerCom);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	m_pObject_Manager->AddRef();
	
	m_pSound_Device = CSound_Device::Get_Instance();
	m_pSound_Device->AddRef();

	return NOERROR;
}

HRESULT CInventory_Blank::SetUp_ConstantTable(LPD3DXEFFECT pEffect)
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

_int CInventory_Blank::Update_GameObject(const _float & fTimeDelta)
{
	if (false == m_isSee)
		return _RUN;

	if (true == m_pKeyCheckerCom->Check_MouseButton_Once(CKeyChecker::DIM_LBUTTON))
	{
		m_pSound_Device->SoundPlay(17, 0);
		RECT rcRect = { 0 };

		//Local Position
		_vec3	vPos = *m_pTransformCom->Get_Infomation(CTransform::INFO_POSITION);

		rcRect.left = g_iBackCX * (1.f + vPos.x - 0.1f) / 2;
		rcRect.top = g_iBackCY * (1.f - vPos.y - 0.15f) / 2;
		rcRect.right = g_iBackCX * (1.f + vPos.x + 0.1f) / 2;
		rcRect.bottom = g_iBackCY * (1.f - vPos.y + 0.15f) / 2;

		if (true == m_pKeyCheckerCom->isMouse_InRect(g_hWnd, rcRect))
		{
			if (BELONG_INVEN == m_eBelong)
			{
				CGameObject*	pSign = m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Sign_Check");
				if (nullptr != pSign)
					pSign->Set_isDead(true);

				//본인 체크가 되어있었으면 풀기
				if (true == m_isChecked)
				{
					m_isChecked = false;
					return _RUN;
				}
				//다른 체크가 되어있었으면 풀기
				_uint iBlankNum = m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_Inventory_Blank");
				for (_uint i = 0; i < iBlankNum; ++i)
				{
					CInventory_Blank*	pBlank = (CInventory_Blank*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Inventory_Blank", i));
					if (nullptr == pBlank)
						continue;
					if (true == pBlank->Get_isChecked())
					{
						pBlank->Set_isChecked(false);
						break;
					}
				}

				//체크
				if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Sign_Check", L"Object_Sign_Check", &vPos)))
					return _RUN;

				m_isChecked = true;
			}

			else if (BELONG_PROFILE == m_eBelong)
			{
				CEquipmentBox*	pProfile = (CEquipmentBox*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Profile"));
				if (nullptr == pProfile)
					return _RUN;

				//스탯 재조정
				ITEMINFO*	pInfo = pProfile->Get_ItemInfo(m_iIdx);

				CUnit*	pPlayer = (CUnit*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
				if (nullptr == pPlayer)
					return _RUN;

				UNITINFO	tNewInfo;
				ZeroMemory(&tNewInfo, sizeof(UNITINFO));

				if (CInventory::ITEM_DEF_CHEST == pInfo->iItemKind
					|| CInventory::ITEM_DEF_HEAD == pInfo->iItemKind
					|| CInventory::ITEM_DEF_BOOTS == pInfo->iItemKind)
				{
					tNewInfo.fDef = pInfo->fItemStats * -1.f;
					pPlayer->Add_Info(&tNewInfo);
				}
				else if (CInventory::ITEM_ATT_WEAPON == pInfo->iItemKind)
				{
					tNewInfo.fAtt = pInfo->fItemStats * -1.f;
					pPlayer->Add_Info(&tNewInfo);
				}

				//장비 빼기
				pProfile->Delete_Item(m_iIdx);
				if (nullptr != m_pItemTextureCom)
					Safe_Release(m_pItemTextureCom);

				//다시 인벤토리에 추가
				CInventory*	pInven = (CInventory*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Inventory"));
				if (nullptr == pInven)
					return _RUN;
				pInven->Add_Item(pInfo);

				//pInven->Update_Inventory(); //blank돌고잇는데 다 지워버려서 오류남.
			}
		}
	}

	return _RUN;
}

_int CInventory_Blank::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return -1;

	m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this);

	return _int();
}

void CInventory_Blank::Render_GameObject(void)
{
	if (false == m_isSee)
		return;

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
		_MSG_BOX("CInventory_Blank SetUp_ConstantTable Failed");
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
	if (false == m_isEmpty)
	{
		if (nullptr == m_pItemBufferCom
			|| nullptr == m_pItemTextureCom)
			return;

		if (FAILED(m_pItemTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture")))
			return;

		pEffect->Begin(nullptr, 0);
		pEffect->BeginPass(0);

		m_pItemBufferCom->Render_VIBuffer();

		pEffect->EndPass();
		pEffect->End();
	}
}

CInventory_Blank * CInventory_Blank::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInventory_Blank* pInstance = new CInventory_Blank(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CInventory_Blank Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInventory_Blank::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CInventory_Blank(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CInventory_Blank Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CInventory_Blank::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CInventory_Blank(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CInventory_Blank Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInventory_Blank::Free(void)
{
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pBufferCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pItemBufferCom);
	Safe_Release(m_pItemTextureCom);
	Safe_Release(m_pKeyCheckerCom);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pSound_Device);

	CGameObject::Free();
}
