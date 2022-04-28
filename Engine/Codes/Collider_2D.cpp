#include "Collider_2D.h"
#include "Shader.h"

_USING(Engine)

CCollider_2D::CCollider_2D(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CCollider_2D::CCollider_2D(const CCollider_2D & rhs)
	: CComponent(rhs)
	, m_pShaderCom(rhs.m_pShaderCom)
	, m_pVB(rhs.m_pVB)
	, m_pIB(rhs.m_pIB)
	, m_matWorld(rhs.m_matWorld)
{
}

HRESULT CCollider_2D::Ready_Collider(CShader * pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	m_pShaderCom = pShader;
	m_pShaderCom->AddRef();

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(sizeof(VTXCOL) * 4, 0, D3DFVF_XYZ | D3DFVF_TEX0, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXCOL*	pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _vec3(-0.5f, 0.5f, 0.f);
	pVertices[0].dwColor = 0;

	pVertices[1].vPosition = _vec3(0.5f, 0.5f, 0.f);
	pVertices[1].dwColor = 0;

	pVertices[2].vPosition = _vec3(0.5f, -0.5f, 0.f);
	pVertices[2].dwColor = 0;

	pVertices[3].vPosition = _vec3(-0.5f, -0.5f, 0.f);
	pVertices[3].dwColor = 0;

	m_pVB->Unlock();

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(sizeof(INDEX16) * 2, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	INDEX16*	pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	pIndices[0]._0 = 0;
	pIndices[0]._1 = 1;
	pIndices[0]._2 = 2;

	pIndices[1]._0 = 0;
	pIndices[1]._1 = 2;
	pIndices[1]._2 = 3;

	m_pIB->Unlock();

	D3DXMatrixIdentity(&m_matWorld);

	return NOERROR;
}

void CCollider_2D::Render_Collider(void)
{
	LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	_matrix		matView, matProj;
	//m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	//m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);
	D3DXMatrixIdentity(&matView);
	D3DXMatrixIdentity(&matProj);

	pEffect->SetMatrix("g_matWorld", &m_matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->Begin(nullptr, 0);

	pEffect->BeginPass(0);

	if (FAILED(m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, sizeof(VTXCOL))))
		return;
	if (FAILED(m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX0)))
		return;
	if (FAILED(m_pGraphic_Device->SetIndices(m_pIB)))
		return;
	if (FAILED(m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2)))
		return;

	pEffect->EndPass();
	pEffect->End();
}

CCollider_2D * CCollider_2D::Create(LPDIRECT3DDEVICE9 pGraphic_Device, CShader * pShader)
{
	CCollider_2D*		pInstance = new CCollider_2D(pGraphic_Device);

	if (FAILED(pInstance->Ready_Collider(pShader)))
	{
		_MSG_BOX("CCollider Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCollider_2D * CCollider_2D::Clone(void)
{
	return Create(m_pGraphic_Device, m_pShaderCom);
}

void CCollider_2D::Free(void)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
	Safe_Release(m_pShaderCom);

	CComponent::Free();
}
