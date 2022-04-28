#pragma once
#include "Engine_Defines.h"

_BEGIN(Engine)

class CHierarchyLoader : public ID3DXAllocateHierarchy
{
private:
	explicit CHierarchyLoader(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CHierarchyLoader(void) = default;
public:
	HRESULT Ready_HierarchyLoader(const _tchar * pFilePath);
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
	STDMETHOD(CreateMeshContainer)(THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER *ppNewMeshContainer);
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree);
private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device;
	_tchar					m_szFilePath[MAX_PATH] = L"";
private:
	void Allocate_Name(char ** pSrcName, const char * pDestName);
public:
	static CHierarchyLoader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath);
	_ulong Release(void);
};

_END