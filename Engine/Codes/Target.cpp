#include "Target.h"

_USING(Engine)

CTarget::CTarget(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

HRESULT CTarget::Ready_Target(const _ulong & dwSizeX, const _ulong & dwSizeY, D3DFORMAT Format, D3DXCOLOR Color)
{
	if (FAILED(D3DXCreateTexture(m_pGraphic_Device, dwSizeX, dwSizeY, 1, D3DUSAGE_RENDERTARGET, Format, D3DPOOL_DEFAULT, &m_pTarget_Texture)))
		return E_FAIL;

	if (FAILED(m_pTarget_Texture->GetSurfaceLevel(0, &m_pTarget_Surface)))
		return E_FAIL;

	m_ClearColor = Color;

	Safe_Release(m_pTarget_Texture);

	return NOERROR;
}

HRESULT CTarget::Ready_DebugBuffer(const _float & fX, const _float & fY, const _float & fSizeX, const _float & fSizeY)
{
	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(sizeof(VTXVIEWPORT) * 4, 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXVIEWPORT*			pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _vec4(fX, fY, 0.f, 1.f);
	pVertices[0].vTexUV = _vec2(0.f, 0.f);

	pVertices[1].vPosition = _vec4(fX + fSizeX, fY, 0.f, 1.f);
	pVertices[1].vTexUV = _vec2(1.f, 0.f);

	pVertices[2].vPosition = _vec4(fX + fSizeX, fY + fSizeY, 0.f, 1.f);
	pVertices[2].vTexUV = _vec2(1.f, 1.f);

	pVertices[3].vPosition = _vec4(fX, fY + fSizeY, 0.f, 1.f);
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

	return NOERROR;
}

HRESULT CTarget::SetUp_OnGraphicDevice(const _ulong & dwIndex)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->GetRenderTarget(dwIndex, &m_pOld_Surface);

	m_pGraphic_Device->SetRenderTarget(dwIndex, m_pTarget_Surface);

	return NOERROR;
}

HRESULT CTarget::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName)
{
	if (FAILED(pEffect->SetTexture(pConstantName, m_pTarget_Texture)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTarget::SetUp_OnShader_Matrix(LPD3DXEFFECT pEffect, const char * pConstantName, const _matrix & matValue)
{
	if (FAILED(pEffect->SetMatrix(pConstantName, &matValue)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CTarget::Release_OnGraphicDevice(const _ulong & dwIndex)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->SetRenderTarget(dwIndex, m_pOld_Surface);

	Safe_Release(m_pOld_Surface);

	return NOERROR;
}

HRESULT CTarget::Clear_Target()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	m_pGraphic_Device->GetRenderTarget(0, &m_pOld_Surface);

	m_pGraphic_Device->SetRenderTarget(0, m_pTarget_Surface);

	m_pGraphic_Device->Clear(0, nullptr, D3DCLEAR_TARGET, m_ClearColor, 1.f, 0);

	m_pGraphic_Device->SetRenderTarget(0, m_pOld_Surface);

	Safe_Release(m_pOld_Surface);

	return NOERROR;
}

void CTarget::Render_DebugBuffer(void)
{
	m_pGraphic_Device->SetTexture(0, m_pTarget_Texture);
	m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, sizeof(VTXVIEWPORT));
	m_pGraphic_Device->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	m_pGraphic_Device->SetIndices(m_pIB);
	m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 4, 0, 2);
}

CTarget * CTarget::Create(LPDIRECT3DDEVICE9 pGraphic_Deivce, const _ulong & dwSizeX, const _ulong & dwSizeY, D3DFORMAT Format, D3DXCOLOR Color)
{
	CTarget*		pInstance = new CTarget(pGraphic_Deivce);

	if (FAILED(pInstance->Ready_Target(dwSizeX, dwSizeY, Format, Color)))
	{
		_MSG_BOX("CTarget Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTarget::Free(void)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	Safe_Release(m_pTarget_Texture);
	//Safe_Release(m_pTarget_Surface);
	Safe_Release(m_pGraphic_Device);
}
