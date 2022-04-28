#include "Terrain_Buffer.h"

_USING(Engine)

CTerrain_Buffer::CTerrain_Buffer(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CTerrain_Buffer::CTerrain_Buffer(const CTerrain_Buffer & rhs)
	: CVIBuffer(rhs)
{
	Ready_VIBuffer();
}

HRESULT CTerrain_Buffer::Ready_VIBuffer(void)
{
	_uint iNumX = 129;
	_uint iNumZ = 129;
	_float fVtxInterval = 1.f;

	m_iNumVertex = iNumX * iNumZ;
	m_iSizeOfVertex = sizeof(VTXTEX);
	m_dwVertexFVF = D3DFVF_XYZ | D3DFVF_TEX1;

	m_iNumPolygon = (iNumX - 1) * (iNumZ - 1) * 2;
	m_iSizeOfPolygonIdx = sizeof(INDEX16);
	m_IndexFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;

	VTXTEX*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	for (_uint i = 0; i < iNumZ; ++i)
	{
		for (_uint j = 0; j < iNumX; ++j)
		{
			_uint iIndex = i * iNumX + j;

			pVertex[iIndex].vPosition = _vec3(j * fVtxInterval, 0.f, i * fVtxInterval);
			pVertex[iIndex].vTexUV = _vec2(j / _float(iNumX - 1), i / _float(iNumZ - 1));
		}
	}

	m_pVB->Unlock();

	INDEX16*	pIndex = nullptr;

	m_pIB->Lock(0,  0, (void**)&pIndex, 0);

	_uint	iArrayIndex = 0;

	for (_uint i = 0; i < iNumZ - 1; i++)
	{
		for (_uint j = 0; j < iNumX - 1; j++)
		{
			_uint		iIndex = i * iNumX + j;

			_uint	iVtxIdx[] =
			{
				iIndex,
				iIndex + 1,
				iIndex + iNumX + 1,
				iIndex + iNumX
			};

			pIndex[iArrayIndex]._0 = iVtxIdx[3];
			pIndex[iArrayIndex]._1 = iVtxIdx[1];
			pIndex[iArrayIndex]._2 = iVtxIdx[0];

			++iArrayIndex;

			pIndex[iArrayIndex]._0 = iVtxIdx[3];
			pIndex[iArrayIndex]._1 = iVtxIdx[2];
			pIndex[iArrayIndex]._2 = iVtxIdx[1];

			++iArrayIndex;
		}
	}

	m_pIB->Unlock();

	return NOERROR;
}

void CTerrain_Buffer::Render_VIBuffer(void)
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

CTerrain_Buffer * CTerrain_Buffer::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTerrain_Buffer*		pInstance = new CTerrain_Buffer(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		_MSG_BOX("CTerrain_Buffer Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTerrain_Buffer::Clone(void)
{
	return new CTerrain_Buffer(*this);
}

void CTerrain_Buffer::Free(void)
{
	CVIBuffer::Free();
}
