#pragma once
#include "Base.h"
#include "Graphic_Device.h"
#include "Navigation.h"

_BEGIN(Engine)

class CShader;
class _ENGINE_DLL CCell final :
	public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_CA, NEIGHBOR_BC, NEIGHBOR_END };
	enum NORMAL { NORMAL_AB, NORMAL_BC, NORMAL_CA, NORMAL_END };
	enum ATTRIBUTE { ATTRIBUTE_PUBLIC, ATTRIBUTE_MONSTER, ATTRIBUTE_END };
private:
	explicit CCell(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CCell(void) = default;
public:
	const _uint Get_Index(void) const {
		return m_iIndex;	}
	const _vec3 Get_Point(POINT ePoint) const {
		return m_vPoint[ePoint];	}
	const _vec3 Get_Normal(NORMAL eNormal) const {
		return m_vNormal[eNormal];	}
	const _vec3 Get_Center(void) const {
		return m_vCenter;	}
	const _bool Get_isSelected(void) const {
		return m_isSelected;	}
	const vector<_uint> Get_Neighbor_IndexVector(void);
	const ATTRIBUTE Get_Attribute(void) {
		return m_eAttribute;	}
	_vec3 Get_Close_Point(const _vec3& vTargetPos, _float* pDistance);
public:
	void Set_Neighbor(NEIGHBOR eNeighbor, CCell* pCell, const _uint& iNeighborIdx);
	void Set_Index(const _uint& iIndex) {
		m_iIndex = iIndex;	}
	void Set_Select(_bool isSelected) {
		m_isSelected = isSelected;	}
public:
	CNavigation::EXISTSTATE Check_Exist_Cell(const _vec3& vPos, _uint* iCurrentIdx, _vec3* vOutNormalVec);
	_bool Check_Exist_Side(const _vec3& vPos, const _vec3& vSour, const _vec3& vDest);
	_bool Check_Equal_Point(const _vec3& vPoint);
	_bool Check_Near_NaviPoint(const _vec3 & vDestPt, _vec3* pPoint);
	NODE Check_Best_Point(const _vec3 & vParentPos, const _vec3 & vTargetPos);
	void Render_Cell(LPD3DXEFFECT pEffect);
private:
	LPDIRECT3DDEVICE9 		m_pGraphic_Device = nullptr;
private:
	_uint					m_iIndex = 0;
private:
	_vec3					m_vNormal[NORMAL_END];
	_vec3					m_vPoint[POINT_END];
	CCell*					m_pNeighbor[NEIGHBOR_END];
	_uint					m_iNeighborIdx[NEIGHBOR_END];
	_vec3					m_vCenter;
protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;
	_uint						m_iNumVertex = 0;
	_uint						m_iSizeOfVertex = 0;
	_ulong						m_dwVertexFVF = 0;
protected:
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;
	_uint						m_iNumPolygon = 0;
	_uint						m_iSizeOfPolygonIdx = 0;
	D3DFORMAT					m_IndexFormat;
private:
	VTXCOL*					m_pVertex = nullptr;
	INDEX16*				m_pIndecies = nullptr;
private:
	_bool					m_isSelected = false;
	ATTRIBUTE				m_eAttribute = ATTRIBUTE_PUBLIC;
private:
	CShader*				m_pShaderCom;
private:
	HRESULT Ready_VBInfomation(const _vec3& vA, const _vec3& vB, const _vec3& vC);
public:
	static CCell* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _vec3& vA, const _vec3& vB, const _vec3& vC, const _uint& iIndex, CShader* pShaderCom, const _uint& iAttribute);
	HRESULT Ready_Cell(const _vec3& vA, const _vec3& vB, const _vec3& vC, const _uint& iIndex, CShader* pShaderCom, const _uint& iAttribute);
private:
	virtual void Free(void);
};

_END