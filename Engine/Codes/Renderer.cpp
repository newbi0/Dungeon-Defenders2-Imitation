#include "Renderer.h"
#include "GameObject.h"
#include "Target_Manager.h"
#include "Shader.h"

_USING(Engine)

CRenderer::CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
{
	m_pTarget_Manager->AddRef();
}

HRESULT CRenderer::Ready_Renderer(void)
{
	m_pShader_Blend = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_Blend.fx");
	if (nullptr == m_pShader_Blend)
		return E_FAIL;

	D3DVIEWPORT9		ViewPort;
	m_pGraphic_Device->GetViewport(&ViewPort);

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(sizeof(VTXVIEWPORT) * 4, 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXVIEWPORT*			pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _vec4(0.0f - 0.5f, 0.0f - 0.5f, 0.f, 1.f);
	pVertices[0].vTexUV = _vec2(0.f, 0.f);

	pVertices[1].vPosition = _vec4(ViewPort.Width - 0.5f, 0.0f - 0.5f, 0.f, 1.f);
	pVertices[1].vTexUV = _vec2(1.f, 0.f);

	pVertices[2].vPosition = _vec4(ViewPort.Width - 0.5f, ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertices[2].vTexUV = _vec2(1.f, 1.f);

	pVertices[3].vPosition = _vec4(0.f - 0.5f, ViewPort.Height - 0.5f, 0.f, 1.f);
	pVertices[3].vTexUV = _vec2(0.f, 1.f);

	m_pVB->Unlock();

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(sizeof(INDEX16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	INDEX16*			pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	if (FAILED(m_pTarget_Manager->Add_Target(m_pGraphic_Device, L"Target_Shadow", ViewPort.Width, ViewPort.Height, D3DFMT_A8R8G8B8, D3DXCOLOR(0.f, 0.f, 0.f, 0.f))))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Ready_DebugBuffer(L"Target_Shadow", 0.f, 0.f, 200.f, 200.f)))
		return E_FAIL;
	if (FAILED(m_pTarget_Manager->Add_MRT(L"MRT_Deferred", L"Target_Shadow")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CRenderer::Add_RenderGroup(RENDERGROUP eGroup, CGameObject * pGameObject)
{
	if (eGroup >= RENDER_END)
		return E_FAIL;

	m_RenderList[eGroup].push_back(pGameObject);

	pGameObject->AddRef();

	return NOERROR;
}

void CRenderer::Render_Renderer(void)
{
	Render_Sky();

	Render_Priority();

	//m_pTarget_Manager->Begin_MRT(L"MRT_Deferred");
	Render_Unit();
	//m_pTarget_Manager->End_MRT(L"MRT_Deferred");

	Render_NonAlpha();

	//Render_Blend();

	Render_Alpha();
	Render_Effect();
	Render_UI_Priority();
	Render_UI();

	//m_pTarget_Manager->Render_DebugBuffer(L"MRT_Deferred");

	Clear_Renderer();
}

void CRenderer::Render_Sky(void)
{
	for (auto& pGameObject : m_RenderList[RENDER_SKY])
	{
		pGameObject->Render_GameObject();
	}
}

void CRenderer::Render_Priority(void)
{
	for (auto& pGameObject : m_RenderList[RENDER_PRIORITY])
	{
		pGameObject->Render_GameObject();
	}
}

void CRenderer::Render_Unit(void)
{
	for (auto& pGameObject : m_RenderList[RENDER_UNIT])
	{
		pGameObject->Render_GameObject();
	}
}

void CRenderer::Render_NonAlpha(void)
{
	for (auto& pGameObject : m_RenderList[RENDER_NONALPHA])
	{
		pGameObject->Render_GameObject();
	}
}

void CRenderer::Render_Alpha(void)
{
	m_RenderList[RENDER_ALPHA].sort([](CGameObject* pSrc, CGameObject* pDest)
		{ return pSrc->Get_ViewZ() > pDest->Get_ViewZ(); });

	for (auto& pGameObject : m_RenderList[RENDER_ALPHA])
	{
		pGameObject->Render_GameObject();
	}
}

void CRenderer::Render_Effect(void)
{
	m_RenderList[RENDER_EFFECT].sort([](CGameObject* pSrc, CGameObject* pDest) { return pSrc->Get_ViewZ() > pDest->Get_ViewZ(); });

	for (auto& pGameObject : m_RenderList[RENDER_EFFECT])
	{
		pGameObject->Render_GameObject();
	}
}

void CRenderer::Render_UI_Priority(void)
{
	m_RenderList[RENDER_UI_PRIORITY].sort([](CGameObject* pSrc, CGameObject* pDest) { return pSrc->Get_ViewZ() > pDest->Get_ViewZ(); });

	for (auto& pGameObject : m_RenderList[RENDER_UI_PRIORITY])
	{
		pGameObject->Render_GameObject();
	}
}

void CRenderer::Render_UI(void)
{
	m_RenderList[RENDER_UI].sort([](CGameObject* pSrc, CGameObject* pDest) { return pSrc->Get_ViewZ() > pDest->Get_ViewZ(); });

	for (auto& pGameObject : m_RenderList[RENDER_UI])
	{
		pGameObject->Render_GameObject();
	}
}

void CRenderer::Render_Blend(void)
{
	LPD3DXEFFECT pEffect = m_pShader_Blend->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	m_pTarget_Manager->SetUp_OnShader(pEffect, L"Target_Shadow", "g_ShadowTexture");

	pEffect->Begin(nullptr, 0);
	pEffect->BeginPass(0);

	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, sizeof(VTXTEX));
	m_pGraphic_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	m_pGraphic_Device->SetIndices(m_pIB);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);

	pEffect->EndPass();
	pEffect->End();
}

void CRenderer::Clear_Renderer(void)
{
	for (size_t i = 0; i < RENDER_END; ++i)
	{
		for (auto& pGameObject : m_RenderList[i])
		{
			Safe_Release(pGameObject);
		}
		m_RenderList[i].clear();
	}
}

CRenderer * CRenderer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CRenderer*		pInstance = new CRenderer(pGraphic_Device);

	// FAILED : ()안에 결과로 받은 hr가 0보다 작을 경우에 조건문안으로 들어오기위한 매크로다.
	if (FAILED(pInstance->Ready_Renderer()))
	{
		_MSG_BOX("CRenderer Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CRenderer::Clone(void)
{
	AddRef();

	return this;
}

void CRenderer::Free(void)
{
	Clear_Renderer();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pShader_Blend);

	CComponent::Free();
}
