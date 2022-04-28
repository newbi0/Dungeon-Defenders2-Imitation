#include "Mesh_Dynamic.h"
#include "HierarchyLoader.h"
#include "AnimationCtrl.h"

_USING(Engine)

CMesh_Dynamic::CMesh_Dynamic(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CMesh_Dynamic::CMesh_Dynamic(const CMesh_Dynamic & rhs)
	: CComponent(rhs)
	, m_pLoader(rhs.m_pLoader)
	, m_pRootFrame(rhs.m_pRootFrame)
	, m_listMeshContainer(rhs.m_listMeshContainer)
	, m_matBase(rhs.m_matBase)
	, m_vMin(rhs.m_vMin)
	, m_vMax(rhs.m_vMax)
{
	m_pAniCtrl = CAnimationCtrl::Clone(*(rhs.m_pAniCtrl));
}

const _uint CMesh_Dynamic::Get_CurrentAniIdx(void) const
{
	return m_pAniCtrl->Get_CurrentAniIdx();
}

_matrix * CMesh_Dynamic::Get_FrameCombinedMatrix(const char * pFrameName) const
{
	return &((D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName))->matCombinedTransformationMatrix;
}

void CMesh_Dynamic::Set_AdjustPeriod(const _double & dPeriod)
{
	m_pAniCtrl->Set_AdjustPeriod(dPeriod);
}

HRESULT CMesh_Dynamic::Ready_Mesh(const _tchar * pFilePath, const _tchar * pFileName)
{
	_tchar	szFullPath[MAX_PATH] = L"";

	lstrcpy(szFullPath, pFilePath);
	lstrcat(szFullPath, pFileName);

	m_pLoader = CHierarchyLoader::Create(m_pGraphic_Device, pFilePath);
	if (nullptr == m_pLoader)
		return E_FAIL;

	m_pAniCtrl = CAnimationCtrl::Create(m_pGraphic_Device);
	if (nullptr == m_pAniCtrl)
		return E_FAIL;

 	if (FAILED(D3DXLoadMeshHierarchyFromX(szFullPath, 0, m_pGraphic_Device, m_pLoader, nullptr, (D3DXFRAME**)&m_pRootFrame, m_pAniCtrl->Get_AnimationCtrl())))
		return E_FAIL;

	D3DXMatrixIdentity(&m_matBase);

	Update_CombinedTransformationMatrix(m_pRootFrame, &m_matBase);

	SetUp_CombinedTransformationMatrixPointer(m_pRootFrame);

	Compute_MinMax(szFullPath);

	return NOERROR;
}

void CMesh_Dynamic::Render_Mesh(LPD3DXEFFECT pEffect, const _uint& iPass)
{
	for (auto& pMeshContainer : m_listMeshContainer)
	{
		// 그리기 위한 행렬 = 피부를 뼈대에 붙이기 위한 행렬 * 피부에 영향을 주는 행렬
		for (_uint i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			pMeshContainer->pRenderingMatrices[i] = pMeshContainer->pOffSetMatrices[i] * (*pMeshContainer->ppCombinedTransformationMatrices[i]);
 			_int a = 0;
		}

		void	*pVerticesSrc, *pVerticesDest;

		pMeshContainer->pOriMesh->LockVertexBuffer(0, &pVerticesSrc);
		pMeshContainer->MeshData.pMesh->LockVertexBuffer(0, &pVerticesDest);

		pMeshContainer->pSkinInfo->UpdateSkinnedMesh(pMeshContainer->pRenderingMatrices, nullptr, pVerticesSrc, pVerticesDest);

		pMeshContainer->pOriMesh->UnlockVertexBuffer();
		pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();

		for (_uint i = 0; i < pMeshContainer->NumMaterials; ++i)
		{
			pEffect->SetTexture("g_BaseTexture", pMeshContainer->ppTexture[i]);
			pEffect->BeginPass(iPass);
			pEffect->CommitChanges();

			pMeshContainer->MeshData.pMesh->DrawSubset(i);

			pEffect->EndPass();
		}
	}
}

void CMesh_Dynamic::SetUp_AnimationSet(const _uint & iIndex, const _float& fWeight)
{
	if (nullptr == m_pAniCtrl)
		return;

	m_pAniCtrl->SetUp_AnimationSet(iIndex, fWeight);
}

void CMesh_Dynamic::Play_Animation(const _float & fTimeDelta)
{
  	if (nullptr == m_pAniCtrl)
		return;

	m_pAniCtrl->Play_Animation(fTimeDelta);

	Update_CombinedTransformationMatrix(m_pRootFrame, &m_matBase);
}

void CMesh_Dynamic::Compute_MinMax(const _tchar * pFilePath)
{
	// MAX_FVF_DECL_SIZE :  정점하나에 정의될 수 있는 fvf최대갯수
	LPD3DXMESH				pMesh;
	LPD3DXBUFFER			pAdjancey = nullptr;
	LPD3DXBUFFER			pMaterialBuffers = nullptr;
	_ulong					dwNumMaterials = 0;

	if (FAILED(D3DXLoadMeshFromX(pFilePath, 0, m_pGraphic_Device, &pAdjancey, &pMaterialBuffers, nullptr, &dwNumMaterials, &pMesh)))
		return;

	D3DVERTEXELEMENT9	Element[MAX_FVF_DECL_SIZE];
	pMesh->GetDeclaration(Element);

	_ushort		wOffset = 0;

	for (_uint i = 0; i < MAX_FVF_DECL_SIZE; ++i)
	{
		if (Element[i].Usage == D3DDECLUSAGE_POSITION)
		{
			wOffset = Element[i].Offset;
			break;
		}
	}

	_ulong dwVertexSize = D3DXGetFVFVertexSize(pMesh->GetFVF());

	void*	pVertices = nullptr;

	pMesh->LockVertexBuffer(0, &pVertices);

	HRESULT hr = D3DXComputeBoundingBox((_vec3*)((_byte*)pVertices + wOffset), pMesh->GetNumVertices(), dwVertexSize, &m_vMin, &m_vMax);

	pMesh->UnlockVertexBuffer();

	Safe_Release(pAdjancey);
	Safe_Release(pMaterialBuffers);
	Safe_Release(pMesh);

	if (FAILED(hr))
		return;
}

_bool CMesh_Dynamic::is_Finished(void)
{
	if (nullptr == m_pAniCtrl)
		return true;

	return m_pAniCtrl->is_Finished();
}

void CMesh_Dynamic::Update_CombinedTransformationMatrix(D3DXFRAME_DERIVED * pFrame, const _matrix * pParent_Matrix)
{
	pFrame->matCombinedTransformationMatrix = pFrame->TransformationMatrix * (*pParent_Matrix);

	if (nullptr != pFrame->pFrameSibling)
		Update_CombinedTransformationMatrix((D3DXFRAME_DERIVED*)pFrame->pFrameSibling, pParent_Matrix);

	if (nullptr != pFrame->pFrameFirstChild)
		Update_CombinedTransformationMatrix((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild, &pFrame->matCombinedTransformationMatrix);
}

void CMesh_Dynamic::SetUp_CombinedTransformationMatrixPointer(D3DXFRAME_DERIVED * pFrame)
{
	if (nullptr != pFrame->pMeshContainer)
	{
		D3DXMESHCONTAINER_DERIVED*	pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

		for (_uint i = 0; i < pMeshContainer->dwNumBones; ++i)
		{
			const char* pFrameName = pMeshContainer->pSkinInfo->GetBoneName(i);

			D3DXFRAME_DERIVED*	pFindedFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pRootFrame, pFrameName);

			pMeshContainer->ppCombinedTransformationMatrices[i] = &pFindedFrame->matCombinedTransformationMatrix;
		}

		m_listMeshContainer.push_back(pMeshContainer);
	}

	if (nullptr != pFrame->pFrameSibling)
		SetUp_CombinedTransformationMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameSibling);

	if (nullptr != pFrame->pFrameFirstChild)
		SetUp_CombinedTransformationMatrixPointer((D3DXFRAME_DERIVED*)pFrame->pFrameFirstChild);
}

CMesh_Dynamic * CMesh_Dynamic::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath, const _tchar * pFileName)
{
	CMesh_Dynamic*		pInstance = new CMesh_Dynamic(pGraphic_Device);

	if (FAILED(pInstance->Ready_Mesh(pFilePath, pFileName)))
	{
		_MSG_BOX("CMesh_Dynamic Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CMesh_Dynamic::Clone(void)
{
	return new CMesh_Dynamic(*this);
}

void CMesh_Dynamic::Free(void)
{
	if (false == m_isClone)
	{
		m_pLoader->DestroyFrame(m_pRootFrame);
		Safe_Release(m_pLoader);
	}

	Safe_Release(m_pAniCtrl);

	m_listMeshContainer.clear();

	CComponent::Free();
}
