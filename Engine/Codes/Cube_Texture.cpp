#include "Cube_Texture.h"

_USING(Engine)

CCube_Texture::CCube_Texture(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CVIBuffer(pGraphic_Device)
{
}

CCube_Texture::CCube_Texture(const CCube_Texture & rhs)
	: CVIBuffer(rhs)
{
	Ready_VIBuffer();
}

HRESULT CCube_Texture::Ready_VIBuffer(void)
{
	m_iNumVertex = 8;
	m_iSizeOfVertex = sizeof(VTXCUBETEX);
	m_dwVertexFVF = D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE3(0);

	m_iNumPolygon = 12;
	m_iSizeOfPolygonIdx = sizeof(INDEX16);
	m_IndexFormat = D3DFMT_INDEX16;

	if (FAILED(CVIBuffer::Ready_VIBuffer()))
		return E_FAIL;

	VTXCUBETEX*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec3(-0.5f, 0.5f, -0.5f);
	pVertex[0].vTexUV = pVertex[0].vPosition;

	pVertex[1].vPosition = _vec3(0.5f, 0.5f, -0.5f);
	pVertex[1].vTexUV = pVertex[1].vPosition;

	pVertex[2].vPosition = _vec3(0.5f, -0.5f, -0.5f);
	pVertex[2].vTexUV = pVertex[2].vPosition;

	pVertex[3].vPosition = _vec3(-0.5f, -0.5f, -0.5f);
	pVertex[3].vTexUV = pVertex[3].vPosition;

	pVertex[4].vPosition = _vec3(-0.5f, 0.5f, 0.5f);
	pVertex[4].vTexUV = pVertex[4].vPosition;

	pVertex[5].vPosition = _vec3(0.5f, 0.5f, 0.5f);
	pVertex[5].vTexUV = pVertex[5].vPosition;

	pVertex[6].vPosition = _vec3(0.5f, -0.5f, 0.5f);
	pVertex[6].vTexUV = pVertex[6].vPosition;

	pVertex[7].vPosition = _vec3(-0.5f, -0.5f, 0.5f);
	pVertex[7].vTexUV = pVertex[7].vPosition;

	m_pVB->Unlock();

	INDEX16*	pIndex = nullptr;

	m_pIB->Lock(0,  0, (void**)&pIndex, 0);

	// +x
	pIndex[0]._0 = 1; pIndex[0]._1 = 5; pIndex[0]._2 = 6;
	pIndex[1]._0 = 1; pIndex[1]._1 = 6; pIndex[1]._2 = 2;
	// -x
	pIndex[2]._0 = 4; pIndex[2]._1 = 0; pIndex[2]._2 = 3;
	pIndex[3]._0 = 4; pIndex[3]._1 = 3; pIndex[3]._2 = 7;
	// +y
	pIndex[4]._0 = 4; pIndex[4]._1 = 5; pIndex[4]._2 = 1;
	pIndex[5]._0 = 4; pIndex[5]._1 = 1; pIndex[5]._2 = 0;
	// -y
	pIndex[6]._0 = 3; pIndex[6]._1 = 2; pIndex[6]._2 = 6;
	pIndex[7]._0 = 3; pIndex[7]._1 = 6; pIndex[7]._2 = 7;
	// +z
	pIndex[8]._0 = 7; pIndex[8]._1 = 6; pIndex[8]._2 = 5;
	pIndex[9]._0 = 7; pIndex[9]._1 = 5; pIndex[9]._2 = 4;
	// +z
	pIndex[10]._0 = 0; pIndex[10]._1 = 1; pIndex[10]._2 = 2;
	pIndex[11]._0 = 0; pIndex[11]._1 = 2; pIndex[11]._2 = 3;

	m_pIB->Unlock();

	return NOERROR;
}

void CCube_Texture::Render_VIBuffer(void)
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

void CCube_Texture::Resize_Buffer(const _vec2 & vMin, const _vec2 & vMax)
{
	VTXCUBETEX*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = _vec3(vMin.x, vMax.y, 0.f);
	pVertex[1].vPosition = _vec3(vMax.x, vMax.y, 0.f);
	pVertex[2].vPosition = _vec3(vMax.x, vMin.y, 0.f);
	pVertex[3].vPosition = _vec3(vMin.x, vMin.y, 0.f);

	m_pVB->Unlock();
}

CCube_Texture * CCube_Texture::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CCube_Texture*		pInstance = new CCube_Texture(pGraphic_Device);

	if (FAILED(pInstance->Ready_VIBuffer()))
	{
		_MSG_BOX("CCube_Texture Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCube_Texture::Clone(void)
{
	return new CCube_Texture(*this);
}

void CCube_Texture::Free(void)
{
	CVIBuffer::Free();
}
