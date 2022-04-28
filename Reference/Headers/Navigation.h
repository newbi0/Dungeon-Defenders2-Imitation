#pragma once
#include "Component.h"

_BEGIN(Engine)

class CCell;
class CPicking;
class CShader;
class _ENGINE_DLL CNavigation : public CComponent
{
public:
	enum EXISTSTATE { EXIST_IN, EXIST_NEIGHBOR, EXIST_OUT, EXIST_NULL};
protected:
	explicit CNavigation(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CNavigation(void) = default;
public:
	CCell* Get_Cell(const _uint& iIndex) const;
	const _uint Get_VecCellSize(void) const {
		return (_uint)m_vecCell.size();	}
	_float Get_Hight(const _vec3& vPos, _uint * pCurrntIdx);
	const vector<_uint> Get_Neighbor_IndexVector(const _uint& iIndex) const;
	_vec3* Get_ZenPoint(void) const;
public:
	_bool Compute_Direction(const _vec3& vPos, const _vec3& vSour, const _vec3& vDest);
	_uint Compute_CurrentIndex(const _vec3 & vPos, _uint * pCurrntIdx);
	_vec3 Compute_MousePosOnCell(HWND hWnd);
	_vec3 Compute_CloseCellPos(const _vec3& vTargetPos);
public:
	void Set_Init(void);
public:
	HRESULT Add_Cell(const _vec3 & vA, const _vec3 & vB, const _vec3 & vC, const _uint& iAttribute);
	EXISTSTATE Check_Exist(const _vec3 & vPos, const _vec3 & vDir, const _float& fSpeed, _uint* iCurrntIdx, _vec3* vOutNormalVec);
	EXISTSTATE Check_Exist(const _vec3 & vPos, const _uint& iIndex);
	_bool Check_Near_NaviPoint(const _vec3 & vDestPt, _vec3* pPoint);
	NODE Check_Best_Point(const _vec3& vParentPos, const _vec3& vTargetPos, const _uint& iIndex);
	_bool Check_Cursor_InCell(HWND hWnd);
public:
	_uint Delete_Selected_Cell(void);	//지운 인덱스를 리턴
	void Save_Cell(const _tchar* pFileName);
	void Load_Cell(const _tchar* pFileName);
	void Render_Navigation(void);
private:
	_float			m_fCurrentHihgt = 0.f;
	_bool			m_isLoad = false;
private:
	vector<CCell*>			m_vecCell;
	typedef vector<CCell*>	VECCELL;
private:
	CPicking*		m_pPickingCom = nullptr;
	CShader*		m_pShaderCom = nullptr;
public:
	void Set_Neighbor(void);
	virtual CComponent* Clone(void);
	static CNavigation* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _uint& iSceneID, const _tchar* pPickingCom, const _tchar* pShaderCom);
protected:
	BOOL IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, FLOAT* t, FLOAT* u, FLOAT* v);
	HRESULT Ready_Navigation(const _uint& iSceneID, const _tchar* pPickingCom, const _tchar* pShaderCom);
	virtual void Free(void);
};

_END