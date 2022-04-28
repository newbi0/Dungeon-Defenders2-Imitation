#include "Rect_Texture.h"

_USING(Engine)

CRect_Texture::CRect_Texture(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CRect_Texture::CRect_Texture(const CRect_Texture & rhs)
	: CVIBuffer(rhs)
{
	Ready_VIBuffer();
}

HRESULT CRect_Texture::Ready_VIBuffer(void)
{
	m_iNumVertex = 4;
	m_iSizeOfVertex = sizeof(VTXTEX);
	m_dwVertexFVF = D3DFVF_XYZ | D3DFVF_TEX1;

	m_iNumPolygon = 2;
	m_iSizeOfPolygonIdx = sizeof(INDEX16);
	m_IndexFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;

	VTXTEX*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec3(-1.f, 1.f, 0.f);
	pVertex[0].vTexUV = _vec2(0.0f, 0.0f);

	pVertex[1].vPosition = _vec3(1.f, 1.f, 0.f);
	pVertex[1].vTexUV = _vec2(1.0f, 0.0f);

	pVertex[2].vPosition = _vec3(1.f, -1.f, 0.f);
	pVertex[2].vTexUV = _vec2(1.0f, 1.0f);

	pVertex[3].vPosition = _vec3(-1.f, -1.f, 0.f);
	pVertex[3].vTexUV = _vec2(0.0f, 1.0f);

	m_pVB->Unlock();

	INDEX16*	pIndex = nullptr;

	m_pIB->Lock(0,  0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	pIndex[1]._0 = 0;
	pIndex[1]._1 = 2;
	pIndex[1]._2 = 3;

	m_pIB->Unlock();

	return NOERROR;
}

void CRect_Texture::Render_VIBuffer(void)
{
	if (FAILED(m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iSizeOfVertex)))
		return;
	if (FAILED(m_pGraphic_Device->SetFVF(m_dwVertexFVF)))
		return;
	if (FAILED(m_pGraphic_Device->SetIndices(m_pIB)))
		return;
	if (FAILED(m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0,0,m_iNumVertex, 0, m_iNumPolygon)))
		return;
}

void CRect_Texture::Resize_Buffer(const _vec2 & vMin, const _vec2 & vMax)
{
	VTXTEX*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec3(vMin.x, vMax.y, 0.f);
	pVertex[1].vPosition = _vec3(vMax.x, vMax.y, 0.f);
	pVertex[2].vPosition = _vec3(vMax.x, vMin.y, 0.f);
	pVertex[3].vPosition = _vec3(vMin.x, vMin.y, 0.f);

	m_pVB->Unlock();
}

void CRect_Texture::Resize_Buffer(const _vec3 & vMin, const _vec3 & vMax)
{
	VTXTEX*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec3(vMin.x, vMax.y, vMax.z);
	pVertex[1].vPosition = _vec3(vMax.x, vMax.y, vMax.z);
	pVertex[2].vPosition = _vec3(vMax.x, vMin.y, vMin.z);
	pVertex[3].vPosition = _vec3(vMin.x, vMin.y, vMin.z);

	m_pVB->Unlock();
}

CRect_Texture * CRect_Texture::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CRect_Texture*		pInstance = new CRect_Texture(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		_MSG_BOX("CRect_Texture Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CRect_Texture::Clone(void)
{
	return new CRect_Texture(*this);
}

void CRect_Texture::Free(void)
{
	CVIBuffer::Free();
}
