#include "Picking.h"
#include "Camera.h"
#include "Component_Manager.h"

_USING(Engine)

CPicking::CPicking(LPDIRECT3DDEVICE9 pGraphic_Device)
	:CComponent(pGraphic_Device)
{
}

CPicking::CPicking(const CPicking & rhs)
	:CComponent(rhs)
{
}

_vec3 CPicking::Compute_Picking_Terrain(HWND hWnd, _vec3* pVtxPos, const _uint & iVtxCntX, const _uint & iVtxCntZ, const _matrix* pWorld_Inverse)
{
	_vec3	vRayPos, vRayDir;
	Compute_Ray(hWnd, pWorld_Inverse, &vRayPos, &vRayDir);

	for (_uint i = 0; i < iVtxCntZ; ++i)
	{
		for (_uint j = 0; j < iVtxCntX; ++j)
		{
			_uint	iCurrentIndex = i * iVtxCntX + j;

			if(iCurrentIndex + iVtxCntX + 1 > iVtxCntZ * iVtxCntX)
				return _vec3(0.f, 0.f, 0.f);

			_uint	iIndex[] =
			{
				iCurrentIndex + iVtxCntX,
				iCurrentIndex + iVtxCntX + 1,
				iCurrentIndex + 1,
				iCurrentIndex
			};

			_float t, u, v;

			//오른쪽위
			if(TRUE == IntersectTriangle(vRayPos, vRayDir, pVtxPos[iIndex[1]], pVtxPos[iIndex[0]], pVtxPos[iIndex[2]], &t, &u, &v))
			{
				iCurrentIndex;
				return _vec3((pVtxPos[iIndex[1]].x) + ((pVtxPos[iIndex[0]].x) - (pVtxPos[iIndex[1]].x)) * u, 0.f
					, (pVtxPos[iIndex[1]].z) + ((pVtxPos[iIndex[2]].z) - (pVtxPos[iIndex[1]].z)) * v);
			}
			//왼쪽아래
			if (TRUE == IntersectTriangle(vRayPos, vRayDir, pVtxPos[iIndex[3]], pVtxPos[iIndex[2]], pVtxPos[iIndex[0]], &t, &u, &v))
			{
				iCurrentIndex;
				return _vec3((pVtxPos[iIndex[3]].x) + ((pVtxPos[iIndex[2]].x) - (pVtxPos[iIndex[3]].x)) * u, 0.f
					, (pVtxPos[iIndex[3]].z) + ((pVtxPos[iIndex[0]].z) - (pVtxPos[iIndex[3]].z)) * v);
			}
		}
	}

	return _vec3(0.f, 0.f, 0.f);
}
//
//_vec3 CPicking::Compute_Picking_Mesh(HWND hWnd, const VTXMESH* pVerteices, const INDEX32* pIndeices, const _uint& iNumPolygon, const _matrix* pWorld_Inverse, _bool* pResult)
//{
//	_vec3	vRayPos, vRayDir;
//	Compute_Ray(hWnd, pWorld_Inverse, &vRayPos, &vRayDir);
//
//	for (_uint i = 0; i < iNumPolygon; ++i)
//	{
//		_float t, u, v;
//
//		_vec3	vPos[3];
//
//		D3DXVec3TransformCoord(&vPos[0], pVerteices[pIndeices[i]._0].vPosition, pWorld_Inverse);
//		D3DXVec3TransformCoord(&vPos[1], pVerteices[pIndeices[i]._1].vPosition, pWorld_Inverse);
//		D3DXVec3TransformCoord(&vPos[2], pVerteices[pIndeices[i]._2].vPosition, pWorld_Inverse);
//
//		if (true == IntersectTriangle(vRayPos, vRayDir
//			, vPos[0]
//			, vPos[1]
//			, vPos[2]
//			, &t, &u, &v))
//		{
//			if (nullptr != pResult)
//				*pResult = true;
//			return vRayPos + vRayDir * t;
//		}
//	}
//	if (nullptr != pResult)
//		*pResult = false;
//	return _vec3(0.f, 0.f, 0.f, 1.f);
//}

_bool CPicking::Compute_Picking_Cell(HWND hWnd, const _vec3 & vPointA, const _vec3 & vPointB, const _vec3 & vPointC, const _matrix * pWorld_Inverse, _vec3* pPos)
{
	_vec3	vRayPos, vRayDir;
	Compute_Ray(hWnd, pWorld_Inverse, &vRayPos, &vRayDir);

	_float t, u, v;

	if (TRUE == IntersectTriangle(vRayPos, vRayDir, (_vec3)vPointA, (_vec3)vPointB, (_vec3)vPointC, &t, &u, &v))
	{
		if (nullptr != pPos)
			*pPos = vRayPos + vRayDir * t;
		return true;
	}

	*pPos = _vec3(0.f, 0.f, 0.f);
	return false;
}

BOOL CPicking::IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, FLOAT* t, FLOAT* u, FLOAT* v)
{
	// Find vectors for two edges sharing vert0
	D3DXVECTOR3 edge1 = v1 - v0;
	D3DXVECTOR3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	D3DXVECTOR3 pvec;
	D3DXVec3Cross(&pvec, &dir, &edge2);

	// If determinant is near zero, ray lies in plane of triangle
	FLOAT det = D3DXVec3Dot(&edge1, &pvec);

	D3DXVECTOR3 tvec;
	if (det > 0)
	{
		tvec = orig - v0;
	}
	else
	{
		tvec = v0 - orig;
		det = -det;
	}

	if (det < 0.0001f)
		return FALSE;

	// Calculate U parameter and test bounds
	*u = D3DXVec3Dot(&tvec, &pvec);
	if (*u < 0.0f || *u > det)
		return FALSE;

	// Prepare to test V parameter
	D3DXVECTOR3 qvec;
	D3DXVec3Cross(&qvec, &tvec, &edge1);

	// Calculate V parameter and test bounds
	*v = D3DXVec3Dot(&dir, &qvec);
	if (*v < 0.0f || *u + *v > det)
		return FALSE;

	// Calculate t, scale parameters, ray intersects triangle
	*t = D3DXVec3Dot(&edge2, &qvec);
	FLOAT fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return TRUE;
}

void CPicking::Compute_Ray(HWND hWnd, const _matrix * pWorld_Inverse, _vec3 * pOutRayPos, _vec3 * pOutRayDir)
{
	POINT	ptMouse;
	GetCursorPos(&ptMouse);
	ScreenToClient(hWnd, &ptMouse);

	D3DVIEWPORT9	ViewPort;
	ZeroMemory(&ViewPort, sizeof(D3DVIEWPORT9));
	m_pGraphic_Device->GetViewport(&ViewPort);

	_vec3	vMouseProj = _vec3(ptMouse.x / (ViewPort.Width * 0.5f) - 1.f,
		ptMouse.y / (ViewPort.Height * -0.5f) + 1.f, 0.f);

	_matrix	matProj;
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);
	_matrix	matProj_Inverse;
	D3DXMatrixInverse(&matProj_Inverse, nullptr, &matProj);

	_vec3	vMouseView;
	D3DXVec3TransformCoord(&vMouseView, &vMouseProj, &matProj_Inverse);

	_vec3	vRayPosView = _vec3(0.f, 0.f, 0.f);
	_vec3	vRayDirView = vMouseView - vRayPosView;

	_matrix	matView;
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	_matrix	matView_Inverse;
	D3DXMatrixInverse(&matView_Inverse, nullptr, &matView);

	//월드좌표
	D3DXVec3TransformCoord(pOutRayPos, &vRayPosView, &matView_Inverse);
	D3DXVec3TransformNormal(pOutRayDir, &vRayDirView, &matView_Inverse);
}

CPicking * CPicking::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CPicking*	pInstance = new CPicking(pGraphic_Device);
	if (FAILED(pInstance->Ready_Picking()))
	{
		_MSG_BOX("CPicking Creating Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CPicking::Clone(void)
{
	return new CPicking(*this);
}

HRESULT CPicking::Ready_Picking(void)
{
	return NOERROR;
}

void CPicking::Free(void)
{
	CComponent::Free();
}
