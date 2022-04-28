#include "VIBuffer.h"

_USING(Engine)

CVIBuffer::CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer & rhs)
	: CComponent(rhs)
	, m_iNumVertex(rhs.m_iNumVertex)
	, m_iSizeOfVertex(rhs.m_iSizeOfVertex)
	, m_dwVertexFVF(rhs.m_dwVertexFVF)
	, m_iNumPolygon(rhs.m_iNumPolygon)
	, m_iSizeOfPolygonIdx(rhs.m_iSizeOfPolygonIdx)
	, m_IndexFormat(rhs.m_IndexFormat)
{
}

HRESULT CVIBuffer::Ready_VIBuffer(void)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iSizeOfVertex * m_iNumVertex, 0, m_dwVertexFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iSizeOfPolygonIdx * m_iNumPolygon, 0, m_IndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	return NOERROR;
}

void CVIBuffer::Free(void)
{
	if(nullptr != m_pVB)
		Safe_Release(m_pVB);
	if (nullptr != m_pIB)
		Safe_Release(m_pIB);

	CComponent::Free();
}
