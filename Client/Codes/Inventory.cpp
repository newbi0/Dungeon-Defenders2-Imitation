#include "stdafx.h"
#include "Defines.h"
#include "Inventory.h"
#include "Renderer.h"
#include "Transform.h"
#include "Rect_Texture.h"
#include "Texture.h"
#include "Shader.h"
#include "Unit.h"

#include "Object_Manager.h"
#include "Inventory_Blank.h"
#include "Inventory_Button.h"

const _uint	ITEM_NUM_MAX = 12;
const _uint	ITEM_ROW_NUM_MAX = 4;
const _uint	ITEM_COL_NUM_MAX = 3;


CInventory::CInventory(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CUI(pGraphic_Device)
{
}

CInventory::CInventory(const CInventory & rhs)
	: CUI(rhs)
	, m_vecItem(rhs.m_vecItem)
	, m_iPageIdx(rhs.m_iPageIdx)
{
}

void CInventory::Set_SeeAble(_bool isSeeAble)
{
	if (m_isSee == isSeeAble)
		return;

	m_isSee = isSeeAble;

	if (true == m_isSee)
	{
		//자리 세팅
		Set_Blank();

		//버튼
		if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Inventory_Button", L"Object_Inventory_Button")))
			return;
		CInventory_Button*	pButton = (CInventory_Button*)(m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_Inventory_Button"));
		if (nullptr == pButton)
			return;
		pButton->Set_InventoryButton(CInventory_Button::BUTTON_PUTTING);

		if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Inventory_Button", L"Object_Inventory_Button")))
			return;
		pButton = (CInventory_Button*)(m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_Inventory_Button"));
		if (nullptr == pButton)
			return;
		pButton->Set_InventoryButton(CInventory_Button::BUTTON_THROWAWAY);

		if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Inventory_Button", L"Object_Inventory_Button")))
			return;
		pButton = (CInventory_Button*)(m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_Inventory_Button"));
		if (nullptr == pButton)
			return;
		pButton->Set_InventoryButton(CInventory_Button::BUTTON_PROPAGE);

		if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Inventory_Button", L"Object_Inventory_Button")))
			return;
		pButton = (CInventory_Button*)(m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_Inventory_Button"));
		if (nullptr == pButton)
			return;
		pButton->Set_InventoryButton(CInventory_Button::BUTTON_NEXTPAGE);
	}
	
	//지우는 작업
	else
	{
		m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_Inventory_Button");
		m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_Inventory_Blank");
	}
}

void CInventory::Turn_Page(_bool isNext)
{
	if (true == isNext)
	{
		++m_iPageIdx;
		_uint	iMaxPage = (m_vecItem.size() / 12) + 1;
		if (m_iPageIdx > iMaxPage)
			m_iPageIdx = iMaxPage;
	}
	else
	{
		if (1 == m_iPageIdx)
			return;
		--m_iPageIdx;
	}

	Update_Inventory();
}

HRESULT CInventory::Add_Item(ITEMINFO * pItemInfo)
{
	if (nullptr == pItemInfo)
		return E_FAIL;

	ITEMINFO*	pNewItemInfo = new ITEMINFO;
	memcpy(pNewItemInfo, pItemInfo, sizeof(ITEMINFO));

	m_vecItem.push_back(pNewItemInfo);

	return NOERROR;
}

HRESULT CInventory::Delete_Item(const _uint & iIdx)
{
	auto iter = m_vecItem.begin();

	for (_uint i = 0; i < iIdx; ++i)
		++iter;

	m_vecItem.erase(iter);

	return NOERROR;
}

void CInventory::Update_Inventory(void)
{
	m_pObject_Manager->Clear_GameObject(SCENE_STAGE, L"Layer_Inventory_Blank");

	Set_Blank();
}

HRESULT CInventory::Ready_GameObject_Prototype(void)
{
	if (FAILED(Ready_Component()))
		return E_FAIL;

	m_pTransformCom->Set_Infomation(CTransform::INFO_POSITION, &_vec3(0.f, 0.f, 0.f));
	m_fViewZ = 0.3f;

	return NOERROR;
}

HRESULT CInventory::Ready_GameObject_Clone(const _vec3 * pPos)
{
	m_pRendererCom = (Engine::CRenderer*)Get_Component(L"Com_Renderer");
	m_pRendererCom->AddRef();

	m_pTransformCom = (Engine::CTransform*)Get_Component(L"Com_Transform");
	m_pTransformCom->AddRef();

	m_pBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_Buffer");
	m_pBufferCom->AddRef();

	m_pTextureCom = (Engine::CTexture*)Get_Component(L"Com_Texture");
	m_pTextureCom->AddRef();

	m_pPageBufferCom = (Engine::CRect_Texture*)Get_Component(L"Com_PageBuffer");
	m_pPageBufferCom->AddRef();

	m_pPageTextureCom = (Engine::CTexture*)Get_Component(L"Com_PageTexture");
	m_pPageTextureCom->AddRef();

	m_pShaderCom = (Engine::CShader*)Get_Component(L"Com_Shader");
	m_pShaderCom->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	m_pObject_Manager->AddRef();

	m_pBufferCom->Resize_Buffer(_vec2(-0.8f, -0.8f), _vec2(0.8f, 0.8f));
	m_pPageBufferCom->Resize_Buffer(_vec2(-0.2f, -0.65f), _vec2(-0.1f, -0.55f));

	return NOERROR;
}

HRESULT CInventory::Ready_GameObject_Clone(const _matrix & matWorld)
{
	Ready_GameObject_Clone();
	return NOERROR;
}

HRESULT CInventory::Ready_Component(void)
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
	m_pTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Inventory");
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Texture", m_pTextureCom);

	// For.Com_Buffer
	m_pPageBufferCom = (Engine::CRect_Texture*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Buffer_RectTexture");
	if (nullptr == m_pPageBufferCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_PageBuffer", m_pPageBufferCom);

	// For.Com_Textrue
	m_pPageTextureCom = (Engine::CTexture*)m_pComponent_Manager->Clone_Component(SCENE_STAGE, L"Com_Texture_Font_Damage");
	if (nullptr == m_pPageTextureCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_PageTexture", m_pPageTextureCom);

	// For.Com_Shader
	m_pShaderCom = (Engine::CShader*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Shader_UI");
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	Engine::CGameObject::Add_Component(L"Com_Shader", m_pShaderCom);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	if (nullptr == m_pObject_Manager)
		return E_FAIL;
	m_pObject_Manager->AddRef();

	return NOERROR;
}

void CInventory::Set_Blank(void)
{
	//the start number of m_iPageIdx is 1
	//the start number of iPageIdx is 0
	_uint	iPageIdx = m_iPageIdx - 1;	
	_uint	iSize = m_vecItem.size();

	//Current page item number = Total item number - Previous page item number
	_uint	iPageItemNum = iSize - (iPageIdx * ITEM_NUM_MAX);

	//Compute column
	_uint	iCol_Limit_Cnt = 1;

	iCol_Limit_Cnt = (iPageItemNum / ITEM_ROW_NUM_MAX) + 1;

	if (iCol_Limit_Cnt > ITEM_COL_NUM_MAX)
		iCol_Limit_Cnt = ITEM_COL_NUM_MAX;

	for (_uint i = 0; i < iCol_Limit_Cnt; ++i)
	{
		//Compute row
		_uint iWidth_Limit_Cnt = 0;

		//If i is the last row
		if (i == iCol_Limit_Cnt - 1)
		{
			//Total item number - Previous page item number - Previous row item number
			iWidth_Limit_Cnt = iSize - (ITEM_NUM_MAX * iPageIdx)
				- (ITEM_ROW_NUM_MAX * (iCol_Limit_Cnt - 1));

			if (iWidth_Limit_Cnt > ITEM_ROW_NUM_MAX)
				iWidth_Limit_Cnt = ITEM_ROW_NUM_MAX;
		}
		else
			iWidth_Limit_Cnt = ITEM_ROW_NUM_MAX;

		for (_uint j = 0; j < iWidth_Limit_Cnt; ++j)
		{
			//Item index = (Previous row item number + Current row item number)
			//											+ Previous page item number;
			_uint	iIdx = ((ITEM_ROW_NUM_MAX * i) + j) + (ITEM_NUM_MAX * iPageIdx);
			if (iIdx >= m_vecItem.size())
				return;

			//Add Blank
			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Inventory_Blank", L"Object_Inventory_Blank"
				, &_vec3(-0.55f + (j * 0.25f), 0.3f - (i * 0.3f), 0.f))))
				return;

			//Get the blank information added
			CInventory_Blank*	pBlank = dynamic_cast<CInventory_Blank*>
				((m_pObject_Manager->Get_LastGameObject(SCENE_STAGE, L"Layer_Inventory_Blank")));
			if (nullptr == pBlank)
				return;

			//Set the item name and information
			_tchar	szTextureTag[128] = L"Com_Texture_Icon_";
			lstrcat(szTextureTag, m_vecItem[iIdx]->pItemName);
			pBlank->Set_InventoryBlank(szTextureTag, iIdx, CInventory_Blank::BELONG_INVEN);
		}
	}
}


_int CInventory::Update_GameObject(const _float & fTimeDelta)
{
	if (true == m_isSee)
	{
		
	}
	return _RUN;
}

_int CInventory::LastUpdate_GameObject(const _float & fTimeDelta)
{
	if (true == m_isSee)
	{
		if (nullptr == m_pRendererCom)
			return _EXIT;

		m_pRendererCom->Add_RenderGroup(Engine::CRenderer::RENDER_UI, this);
	}

	return _RUN;
}

void CInventory::Render_GameObject(void)
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

	//페이지 표시
	if (FAILED(m_pPageTextureCom->SetUp_OnShader(pEffect, "g_BaseTexture", m_iPageIdx)))
		return;

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(1);

	m_pPageBufferCom->Render_VIBuffer();

	pEffect->EndPass();
	pEffect->End();
}

CInventory * CInventory::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CInventory* pInstance = new CInventory(pGraphic_Device);

	if (FAILED(pInstance->Ready_GameObject_Prototype()))
	{
		_MSG_BOX("CInventory Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInventory::Clone(const _vec3 * pPos)
{
	CGameObject*	pInstance = new CInventory(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(pPos)))
	{
		_MSG_BOX("CInventory Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CInventory::Clone(const _matrix & matWorld)
{
	CGameObject*	pInstance = new CInventory(*this);

	if (FAILED(pInstance->Ready_GameObject_Clone(matWorld)))
	{
		_MSG_BOX("CInventory Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CInventory::Free(void)
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

	CUI::Free();
}
