#include "Mesh_Static.h"

CMesh_Static::CMesh_Static(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CMesh_Static::CMesh_Static(const CMesh_Static & rhs)
	: CComponent(rhs)
	, m_pMesh(rhs.m_pMesh)
	, m_pAdjacency(rhs.m_pAdjacency)
	, m_pMaterialBuffers(rhs.m_pMaterialBuffers)
	, m_dwNumMaterial(rhs.m_dwNumMaterial)
	, m_pMaterials(rhs.m_pMaterials)
	, m_ppTextrues(rhs.m_ppTextrues)
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
{
}

HRESULT CMesh_Static::Ready_Mesh(const _tchar * pFilePath, const _tchar * pFileName, const _uint iFileNum)
{
	_tchar		szFullPath[MAX_PATH] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);
	wsprintf(szFullPath, szFullPath, iFileNum);

	if (FAILED(D3DXLoadMeshFromX(szFullPath, 0, m_pGraphic_Device, &m_pAdjacency, &m_pMaterialBuffers, nullptr, &m_dwNumMaterial, &m_pMesh)))
		return E_FAIL;

	m_pMaterials = new D3DXMATERIAL[m_dwNumMaterial];
	m_pMaterials = (D3DXMATERIAL*)m_pMaterialBuffers->GetBufferPointer();

	m_ppTextrues = new LPDIRECT3DTEXTURE9[m_dwNumMaterial];
	ZeroMemory(m_ppTextrues, sizeof(LPDIRECT3DTEXTURE9) * m_dwNumMaterial);

	for (_uint i = 0; i < m_dwNumMaterial; ++i)
	{
		_tchar		szTextureFullPath[MAX_PATH] = L"";
		_tchar		szFileName[MAX_PATH] = L"";

		MultiByteToWideChar(CP_ACP, 0, m_pMaterials[i].pTextureFilename, strlen(m_pMaterials[i].pTextureFilename),
			szFileName, MAX_PATH);

		lstrcpy(szTextureFullPath, pFilePath);
		lstrcat(szTextureFullPath, szFileName);

		if (FAILED(D3DXCreateTextureFromFile(m_pGraphic_Device, szTextureFullPath, &m_ppTextrues[i])))
			return E_FAIL;
	}

	if (FAILED(Compute_MinMax()))
		return E_FAIL;

	return NOERROR;
}

HRESULT CMesh_Static::Compute_MinMax(void)
{
	if (nullptr == m_pMesh)
		return E_FAIL;

	D3DVERTEXELEMENT9	Element[MAX_FVF_DECL_SIZE];
	m_pMesh->GetDeclaration(Element);

	_ushort		wOffset = 0;

	for (_uint i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (Element[i].Usage == D3DDECLUSAGE_POSITION)
		{
			wOffset = Element[i].Offset;
			break;
		}
	}

	_ulong dwVertexSize = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	void*	pVertices = nullptr;

	m_pMesh->LockVertexBuffer(0, &pVertices);

	HRESULT hr = D3DXComputeBoundingBox((_vec3*)((_byte*)pVertices + wOffset), m_pMesh->GetNumVertices(), dwVertexSize, &m_vMin, &m_vMax);

	m_pMesh->UnlockVertexBuffer();

	if (FAILED(hr))
		return E_FAIL;
	
	return NOERROR;
}

void CMesh_Static::Render_Mesh(LPD3DXEFFECT pEffect, const _uint& iNumPass)
{
	for (_uint i = 0; i < m_dwNumMaterial; ++i)
	{
		if (FAILED(pEffect->SetTexture("g_BaseTexture", m_ppTextrues[i])))
			int a = 0;

		if (FAILED(pEffect->CommitChanges()))
			int a = 0;

		if (FAILED(pEffect->BeginPass(iNumPass)))
			int a = 0;

		if (FAILED(m_pMesh->DrawSubset(i)))
			int a = 0;

		if (FAILED(pEffect->EndPass()))
			int a = 0;
	}
}

CMesh_Static * CMesh_Static::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath, const _tchar * pFileName, const _uint iFileNum)
{
	CMesh_Static*		pInstance = new CMesh_Static(pGraphic_Device);

	if (FAILED(pInstance->Ready_Mesh(pFilePath, pFileName, iFileNum)))
	{
		_MSG_BOX("CMesh_Static Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance; return nullptr;
}

CComponent * CMesh_Static::Clone(void)
{
	return new CMesh_Static(*this);
}

void CMesh_Static::Free(void)
{
	CComponent::Free();
}

