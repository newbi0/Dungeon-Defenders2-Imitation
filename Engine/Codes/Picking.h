#pragma once
#include "Component.h"

_BEGIN(Engine)

class _ENGINE_DLL CPicking final :
	public CComponent
{
protected:
	explicit CPicking(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CPicking(const CPicking& rhs);
	virtual ~CPicking(void) = default;
public:
	_vec3 Compute_Picking_Terrain(HWND hWnd, _vec3* pVtxPos, const _uint & iVtxCntX, const _uint & iVtxCntZ, const _matrix* pWorld_Inverse);
	//_vec3 Compute_Picking_Mesh(HWND hWnd, const VTXMESH* pVerteices, const INDEX32* pIndeices, const _uint& iNumPolygon, const _matrix* pWorld_Inverse, _bool* pResult = nullptr);
	_bool Compute_Picking_Cell(HWND hWnd, const _vec3& vPointA, const _vec3& vPointB, const _vec3& vPointC, const _matrix* pWorld_Inverse, _vec3* pPos = nullptr);
	BOOL IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, FLOAT* t, FLOAT* u, FLOAT* v);
	void Compute_Ray(HWND hWnd, const _matrix* pWorld_Inverse, _vec3* vOutRayPos, _vec3* vOutRayDir);
public:
	static CPicking* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void);
protected:
	HRESULT Ready_Picking(void);
	virtual void Free(void);
};

_END