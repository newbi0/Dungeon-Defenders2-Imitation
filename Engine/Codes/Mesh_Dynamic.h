#pragma once
#include "Component.h"

_BEGIN(Engine)

class CHierarchyLoader;
class CAnimationCtrl;
class _ENGINE_DLL CMesh_Dynamic final :
	public CComponent
{
private:
	explicit CMesh_Dynamic(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CMesh_Dynamic(const CMesh_Dynamic& rhs);
	virtual ~CMesh_Dynamic(void) = default;
public:
	const _vec3* Get_Min(void) const {
		return &m_vMin;	}
	const _vec3* Get_Max(void) const {
		return &m_vMax;	}
	const _uint Get_CurrentAniIdx(void) const;
	_matrix* Get_FrameCombinedMatrix(const char* pFrameName) const;
public:
	void Set_AdjustPeriod(const _double& dPeriod);
public:
	HRESULT Ready_Mesh(const _tchar* pFilePath, const _tchar* pFileName);
	void Render_Mesh(LPD3DXEFFECT pEffect, const _uint& iPass);
	void SetUp_AnimationSet(const _uint& iIndex, const _float& fWeight);
	void Play_Animation(const _float& fTimeDelta);
	//void Compute_MinMax(const D3DXFRAME_DERIVED* pFrame);
	void Compute_MinMax(const _tchar * pFilePath);
	_bool is_Finished(void);
private:
	CHierarchyLoader*	m_pLoader;
	CAnimationCtrl*		m_pAniCtrl;
	D3DXFRAME_DERIVED*	m_pRootFrame;
	_matrix				m_matBase;
private:
	_vec3				m_vMin, m_vMax;
private:
	list<D3DXMESHCONTAINER_DERIVED*>	m_listMeshContainer;
private:
	void Update_CombinedTransformationMatrix(D3DXFRAME_DERIVED* pFrame, const _matrix* pParent_Matrix);
	void SetUp_CombinedTransformationMatrixPointer(D3DXFRAME_DERIVED* pFrame);
public:
	static CMesh_Dynamic* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pFilePath, const _tchar* pFileName);
	virtual CComponent* Clone(void);
private:
	virtual void Free(void);
};

_END