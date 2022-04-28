#pragma once
#include "Component.h"

_BEGIN(Engine)

class _ENGINE_DLL CMesh_Static final : public CComponent
{
private:
	explicit CMesh_Static(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMesh_Static(const CMesh_Static& rhs);
	virtual ~CMesh_Static(void) = default;
public:
	const _vec3* Get_Min(void) const {
		return &m_vMin;	}
	const _vec3* Get_Max(void) const {
		return &m_vMax;	}
private:
	HRESULT Ready_Mesh(const _tchar* pFilePath, const _tchar* pFileName, const _uint iFileNum);
	HRESULT Compute_MinMax(void);
public:
	void Render_Mesh(LPD3DXEFFECT pEffect, const _uint& iNumPass);
private:
	LPD3DXMESH		m_pMesh = nullptr;
	LPD3DXBUFFER	m_pAdjacency = nullptr;
	LPD3DXBUFFER	m_pMaterialBuffers = nullptr;
	_ulong			m_dwNumMaterial = 0;
private:
	D3DXMATERIAL*		m_pMaterials = nullptr;
	LPDIRECT3DTEXTURE9*	m_ppTextrues = nullptr;
private:
	_vec3				m_vMin, m_vMax;
public:
	static CMesh_Static* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath, const _tchar* pFileName, const _uint iFileNum = 0);
	virtual CComponent* Clone(void);
private:
	virtual void Free(void);
};

_END