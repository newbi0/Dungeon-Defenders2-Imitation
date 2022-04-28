#include "HierarchyLoader.h"

_USING(Engine)

CHierarchyLoader::CHierarchyLoader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

HRESULT CHierarchyLoader::Ready_HierarchyLoader(const _tchar * pFilePath)
{
	lstrcpy(m_szFilePath, pFilePath);

	return NOERROR;
}

// 내갛 호출하는 함수는 아니지만, 내가 뼈의 공간을 확보하는 작업을 해주어야하며, 기초적은 데이터를 셋해놓는다.
STDMETHODIMP CHierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
{
	D3DXFRAME_DERIVED*		pNewFrame = new D3DXFRAME_DERIVED;
	ZeroMemory(pNewFrame, sizeof(D3DXFRAME_DERIVED));

	//pNewFrame->

	Allocate_Name(&pNewFrame->Name, Name);

	pNewFrame->matCombinedTransformationMatrix = *D3DXMatrixIdentity(&pNewFrame->TransformationMatrix);

	*ppNewFrame = pNewFrame;

	return NOERROR;
}

STDMETHODIMP CHierarchyLoader::CreateMeshContainer(LPCSTR Name
	, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer)
{
	D3DXMESHCONTAINER_DERIVED*		pNewMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	ZeroMemory(pNewMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));

	// D3DXMesh를 받자.
	pNewMeshContainer->MeshData.Type = pMeshData->Type;
	pNewMeshContainer->MeshData.pMesh = pMeshData->pMesh;

	pNewMeshContainer->NumMaterials = NumMaterials == 0 ? 1 : NumMaterials;

	pNewMeshContainer->pMaterials = new D3DXMATERIAL[pNewMeshContainer->NumMaterials];
	pNewMeshContainer->ppTexture = new LPDIRECT3DTEXTURE9[pNewMeshContainer->NumMaterials];

	if (0 != NumMaterials)
	{
		for (size_t i = 0; i < NumMaterials; i++)
		{
			pNewMeshContainer->pMaterials[i].MatD3D = pMaterials[i].MatD3D;
			pNewMeshContainer->pMaterials[i].pTextureFilename = pMaterials[i].pTextureFilename;

			_tchar			szTextureFullPath[MAX_PATH] = L"";
			_tchar			szFileName[MAX_PATH] = L"";

			MultiByteToWideChar(CP_ACP, 0, pMaterials[i].pTextureFilename, strlen(pMaterials[i].pTextureFilename)
				, szFileName, MAX_PATH);

			lstrcpy(szTextureFullPath, m_szFilePath);
			lstrcat(szTextureFullPath, szFileName);

			if (FAILED(D3DXCreateTextureFromFile(m_pGraphic_Device, szTextureFullPath, &pNewMeshContainer->ppTexture[i])))
				return E_FAIL;
		}
	}

	else
	{
		pNewMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
		pNewMeshContainer->pMaterials[0].MatD3D.Power = 1.f;
		pNewMeshContainer->pMaterials[0].pTextureFilename = nullptr;
	}

	_ulong dwNumFaces = pNewMeshContainer->MeshData.pMesh->GetNumFaces();

	pNewMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3];
	memcpy(pNewMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * (dwNumFaces * 3));

	_ulong dwFVF = pNewMeshContainer->MeshData.pMesh->GetFVF();

	if (!(dwFVF & D3DFVF_NORMAL))
	{
		pNewMeshContainer->MeshData.pMesh->CloneMeshFVF(pNewMeshContainer->MeshData.pMesh->GetOptions(), dwFVF | D3DFVF_NORMAL, m_pGraphic_Device, &pNewMeshContainer->MeshData.pMesh);
	}

	if (nullptr == pSkinInfo)
		return E_FAIL;

	pNewMeshContainer->MeshData.pMesh->CloneMeshFVF(pNewMeshContainer->MeshData.pMesh->GetOptions(), pNewMeshContainer->MeshData.pMesh->GetFVF(), m_pGraphic_Device, &pNewMeshContainer->pOriMesh);
	pNewMeshContainer->MeshData.pMesh->AddRef();

	pNewMeshContainer->pSkinInfo = pSkinInfo;
	pNewMeshContainer->pSkinInfo->AddRef();

	pNewMeshContainer->dwNumBones = pNewMeshContainer->pSkinInfo->GetNumBones();

	pNewMeshContainer->ppCombinedTransformationMatrices = new D3DXMATRIX*[pNewMeshContainer->dwNumBones];
	pNewMeshContainer->pOffSetMatrices = new D3DXMATRIX[pNewMeshContainer->dwNumBones];
	pNewMeshContainer->pRenderingMatrices = new D3DXMATRIX[pNewMeshContainer->dwNumBones];

	for (size_t i = 0; i < pNewMeshContainer->dwNumBones; i++)
	{
		pNewMeshContainer->pOffSetMatrices[i] = *pNewMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i);
	}

	*ppNewMeshContainer = pNewMeshContainer;

	return NOERROR;
}

STDMETHODIMP CHierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
{

	Safe_Delete_Array(pFrameToFree->Name);

	if (nullptr != pFrameToFree->pMeshContainer)
		DestroyMeshContainer(pFrameToFree->pMeshContainer);

	if (nullptr != pFrameToFree->pFrameSibling)
		DestroyFrame(pFrameToFree->pFrameSibling);

	if (nullptr != pFrameToFree->pFrameFirstChild)
		DestroyFrame(pFrameToFree->pFrameFirstChild);

	Safe_Delete(pFrameToFree);

	return NOERROR;
}

STDMETHODIMP CHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
{
	D3DXMESHCONTAINER_DERIVED* pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;

	Safe_Release(pMeshContainer->pOriMesh);

	for (size_t i = 0; i < pMeshContainer->NumMaterials; ++i)
	{
		Safe_Release(pMeshContainer->ppTexture[i]);
	}

	Safe_Delete_Array(pMeshContainer->ppTexture);
	Safe_Delete_Array(pMeshContainer->ppCombinedTransformationMatrices);
	Safe_Delete_Array(pMeshContainer->pOffSetMatrices);
	Safe_Delete_Array(pMeshContainer->pRenderingMatrices);

	Safe_Delete_Array(pMeshContainer->Name);
	Safe_Release(pMeshContainer->MeshData.pMesh);
	Safe_Delete_Array(pMeshContainer->pMaterials);
	Safe_Delete_Array(pMeshContainer->pAdjacency);

	Safe_Release(pMeshContainer->pSkinInfo);

	Safe_Delete(pMeshContainer);

	return NOERROR;
}

void CHierarchyLoader::Allocate_Name(char ** pSrcName, const char * pDestName)
{
	if (nullptr == pDestName)
		return;

	_uint iLength = strlen(pDestName);

	*pSrcName = new char[iLength + 1];

	strcpy_s(*pSrcName, iLength + 1, pDestName);
}

CHierarchyLoader * CHierarchyLoader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath)
{
	CHierarchyLoader*		pInstance = new CHierarchyLoader(pGraphic_Device);

	if (FAILED(pInstance->Ready_HierarchyLoader(pFilePath)))
	{
		_MSG_BOX("CHierarchyLoader Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

_ulong CHierarchyLoader::Release(void)
{
	Safe_Release(m_pGraphic_Device);

	delete this;

	return _ulong();
}


//#include "HierarchyLoader.h"
//
//_USING(Engine)
//
//CHierarchyLoader::CHierarchyLoader(LPDIRECT3DDEVICE9 pGraphic_Device)
//	: m_pGraphic_Device(pGraphic_Device)
//{
//	m_pGraphic_Device->AddRef();
//}
//
//HRESULT CHierarchyLoader::Ready_HierarchyLoader(const _tchar * pFilePath)
//{
//	lstrcpy(m_szFilePath, pFilePath);
//
//	return NOERROR;
//}
//
//STDMETHODIMP CHierarchyLoader::CreateFrame(LPCSTR Name, LPD3DXFRAME * ppNewFrame)
//{
//	//새로운 프레임(ppNewFrame)에 기초적인 데이터를 Set.
//	//뼈의 공간을 확보하는 작업.
//
//	D3DXFRAME_DERIVED*	pNewFrame = new D3DXFRAME_DERIVED;
//	ZeroMemory(pNewFrame, sizeof(D3DXFRAME_DERIVED));
//
//	Allocate_Name(&pNewFrame->Name, Name);
//
//	pNewFrame->matCombinedTransformationMatrix = *D3DXMatrixIdentity(&pNewFrame->TransformationMatrix);
//
//	*ppNewFrame = pNewFrame;
//
//	return NOERROR;
//}
//
//STDMETHODIMP CHierarchyLoader::CreateMeshContainer(LPCSTR Name, CONST D3DXMESHDATA * pMeshData, CONST D3DXMATERIAL * pMaterials, CONST D3DXEFFECTINSTANCE * pEffectInstances, DWORD NumMaterials, CONST DWORD * pAdjacency, LPD3DXSKININFO pSkinInfo, LPD3DXMESHCONTAINER * ppNewMeshContainer)
//{
//	D3DXMESHCONTAINER_DERIVED*		pNewMeshContainer = new D3DXMESHCONTAINER_DERIVED;
//	ZeroMemory(pNewMeshContainer, sizeof(D3DXMESHCONTAINER_DERIVED));
//
//	pNewMeshContainer->MeshData.Type = pMeshData->Type;
//	pNewMeshContainer->MeshData.pMesh = pMeshData->pMesh;
//
//	//재질을 최소 1개라도 갖도록
//	if (NumMaterials == 0)
//		pNewMeshContainer->NumMaterials = 1;
//	else
//		pNewMeshContainer->NumMaterials = NumMaterials;
//
//	pNewMeshContainer->pMaterials = new D3DXMATERIAL[pNewMeshContainer->NumMaterials];
//	pNewMeshContainer->ppTexture = new LPDIRECT3DTEXTURE9[pNewMeshContainer->NumMaterials];
//
//	//머테리얼과 텍스처 정보 채우기
//	if (0 != NumMaterials)
//	{
//		for (_uint i = 0; i < NumMaterials; ++i)
//		{
//			pNewMeshContainer->pMaterials[i].MatD3D = pMaterials[i].MatD3D;
//			pNewMeshContainer->pMaterials[i].pTextureFilename = pMaterials[i].pTextureFilename;
//
//			_tchar	szTextureFullPath[MAX_PATH] = L"";
//			_tchar szFileName[MAX_PATH] = L"";
//
//			MultiByteToWideChar(CP_ACP, 0, pMaterials[i].pTextureFilename, strlen(pMaterials[i].pTextureFilename),
//				szFileName, MAX_PATH);
//
//			lstrcpy(szTextureFullPath, m_szFilePath);
//			lstrcat(szTextureFullPath, szFileName);
//
//			if (FAILED(D3DXCreateTextureFromFile(m_pGraphic_Device, szTextureFullPath, &pNewMeshContainer->ppTexture[i])))
//				return E_FAIL;
//		}
//	}
//	else
//	{
//		pNewMeshContainer->pMaterials[0].MatD3D.Ambient = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
//		pNewMeshContainer->pMaterials[0].MatD3D.Diffuse = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
//		pNewMeshContainer->pMaterials[0].MatD3D.Emissive = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
//		pNewMeshContainer->pMaterials[0].MatD3D.Specular = D3DXCOLOR(1.f, 0.f, 0.f, 1.f);
//		pNewMeshContainer->pMaterials[0].MatD3D.Power = 1.f;
//		pNewMeshContainer->pMaterials[0].pTextureFilename = nullptr;
//	}
//
//	//폴리곤마다 갖고 있는 배열
//	_ulong	dwNumFaces = pNewMeshContainer->MeshData.pMesh->GetNumFaces();
//	pNewMeshContainer->pAdjacency = new _ulong[dwNumFaces * 3];
//	memcpy(pNewMeshContainer->pAdjacency, pAdjacency, sizeof(_ulong) * dwNumFaces * 3);
//
//	_ulong	dwFVF = pNewMeshContainer->MeshData.pMesh->GetFVF();
//
//	//법선 정보가 없다면
//	if (!(dwFVF & D3DFVF_NORMAL))
//	{
//		//매시컨테이너에 노말이 있다고 복사해줌
//		pNewMeshContainer->MeshData.pMesh->CloneMeshFVF(pNewMeshContainer->MeshData.pMesh->GetOptions(), dwFVF | D3DFVF_NORMAL,
//			m_pGraphic_Device, &pNewMeshContainer->MeshData.pMesh);
//	}
//
//	pNewMeshContainer->MeshData.pMesh->CloneMeshFVF(pNewMeshContainer->MeshData.pMesh->GetOptions(), pNewMeshContainer->MeshData.pMesh->GetFVF(),
//		m_pGraphic_Device, &pNewMeshContainer->pOriMesh);
//	pNewMeshContainer->MeshData.pMesh->AddRef();
//
//	if (nullptr == pSkinInfo)
//		return E_FAIL;
//
//	pNewMeshContainer->pSkinInfo = pSkinInfo;
//	pNewMeshContainer->pSkinInfo->AddRef();
//
//	pNewMeshContainer->dwNumBones = pNewMeshContainer->pSkinInfo->GetNumBones();
//
//	pNewMeshContainer->ppCombinedTransformationMatrices = new D3DXMATRIX*[pNewMeshContainer->dwNumBones];
//	pNewMeshContainer->pOffSetMatrices = new D3DXMATRIX[pNewMeshContainer->dwNumBones];
//	pNewMeshContainer->pRenderingMatrices = new D3DXMATRIX[pNewMeshContainer->dwNumBones];
//
//	for (_uint i = 0; i < pNewMeshContainer->dwNumBones; ++i)
//	{
//		pNewMeshContainer->pOffSetMatrices[i] = *(pNewMeshContainer->pSkinInfo->GetBoneOffsetMatrix(i));
//	}
//
//	*ppNewMeshContainer = pNewMeshContainer;
//
//	/*
//	LPSTR                   Name;
//	LPD3DXEFFECTINSTANCE    pEffects;
//	struct _D3DXMESHCONTAINER *pNextMeshContainer;
//	*/
//
//	return NOERROR;
//}
//
//STDMETHODIMP CHierarchyLoader::DestroyFrame(LPD3DXFRAME pFrameToFree)
//{
//	Safe_Delete_Array(pFrameToFree->Name);
//
//	if (nullptr != pFrameToFree->pMeshContainer)
//		DestroyMeshContainer(pFrameToFree->pMeshContainer);
//
//	if (nullptr != pFrameToFree->pFrameFirstChild)
//		DestroyFrame(pFrameToFree->pFrameFirstChild);
//
//	if (nullptr != pFrameToFree->pFrameSibling)
//		DestroyFrame(pFrameToFree->pFrameSibling);
//
//	Safe_Delete(pFrameToFree);
//
//	return NOERROR;
//}
//
//STDMETHODIMP CHierarchyLoader::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerToFree)
//{
//	D3DXMESHCONTAINER_DERIVED*	pMeshConainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerToFree;
//
//	Safe_Release(pMeshConainer->pOriMesh);
//
//	for (_uint i = 0; i < pMeshConainer->NumMaterials; ++i)
//		Safe_Release(pMeshConainer->ppTexture[i]);
//
//	Safe_Delete_Array(pMeshConainer->ppTexture);
//	Safe_Delete_Array(pMeshConainer->ppCombinedTransformationMatrices);
//	Safe_Delete_Array(pMeshConainer->pOffSetMatrices);
//	Safe_Delete_Array(pMeshConainer->pRenderingMatrices);
//
//	Safe_Delete_Array(pMeshConainer->Name);
//	Safe_Release(pMeshConainer->MeshData.pMesh);
//	Safe_Delete_Array(pMeshConainer->pMaterials);
//	Safe_Delete_Array(pMeshConainer->pAdjacency);
//
//	Safe_Release(pMeshConainer->pSkinInfo);
//
//	Safe_Delete(pMeshConainer);
//
//	return NOERROR;
//}
//
//void CHierarchyLoader::Allocate_Name(char ** pSrcName, const char * pDestName)
//{
//	if (nullptr == pDestName)
//		return;
//
//	_uint iLength = strlen(pDestName);
//
//	*pSrcName = new char[iLength + 1];
//
//	strcpy_s(*pSrcName, iLength + 1, pDestName);
//}
//
//CHierarchyLoader * CHierarchyLoader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pFilePath)
//{
//	CHierarchyLoader*		pInstance = new CHierarchyLoader(pGraphic_Device);
//
//	if (FAILED(pInstance->Ready_HierarchyLoader(pFilePath)))
//	{
//		_MSG_BOX("CMesh_Static Created Failed");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//_ulong CHierarchyLoader::Release(void)
//{
//	Safe_Release(m_pGraphic_Device);
//
//	return 0;
//}
//
