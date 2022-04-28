#include "Frustum_Culling.h"
#include "Transform.h"

_USING(Engine)

CFrustum_Culling::CFrustum_Culling(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CFrustum_Culling::CFrustum_Culling(const CFrustum_Culling & rhs)
	: CComponent(rhs)
{
}

void CFrustum_Culling::Set_StandardTransform(CTransform * pStandard)
{
	if (nullptr == pStandard)
		return;
	m_pStandard = pStandard;
	m_pStandard->AddRef();
}

_bool CFrustum_Culling::isIn_Frustum(const _vec3 * pWorldPosition, const _float & fRadius)
{
 	_vec3 vPoint[8];
	memcpy(&vPoint, &m_vOriginal_Points, sizeof(_vec3) * 8);

	_matrix matView, matProj;

	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);

	D3DXMatrixInverse(&matView, nullptr, &matView);
	D3DXMatrixInverse(&matProj, nullptr, &matProj);

	for (_uint i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matProj);

	for (_uint i = 0; i < 8; ++i)
		D3DXVec3TransformCoord(&vPoint[i], &vPoint[i], &matView);

	D3DXPLANE			Plane[6];
	ZeroMemory(Plane, sizeof(D3DXPLANE) * 6);

	// +x
	D3DXPlaneFromPoints(&Plane[0], &vPoint[1], &vPoint[5], &vPoint[6]);
	// -x
	D3DXPlaneFromPoints(&Plane[1], &vPoint[4], &vPoint[0], &vPoint[3]);
	// +y
	D3DXPlaneFromPoints(&Plane[2], &vPoint[4], &vPoint[5], &vPoint[1]);
	// -y
	D3DXPlaneFromPoints(&Plane[3], &vPoint[3], &vPoint[2], &vPoint[6]);
	// +z
	D3DXPlaneFromPoints(&Plane[4], &vPoint[7], &vPoint[6], &vPoint[5]);
	// -z
	D3DXPlaneFromPoints(&Plane[5], &vPoint[0], &vPoint[1], &vPoint[2]);

	for (_uint i = 0; i < 6; ++i)
	{
		if (fRadius < D3DXPlaneDotCoord(&Plane[i], pWorldPosition))
			return false;
	}

	return true;
}

_bool CFrustum_Culling::isIn_Distance(const _vec3 * pWorldPosition)
{

	if (nullptr == m_pStandard)
		return false;

	_vec3	vDir = *m_pStandard->Get_Infomation(CTransform::INFO_POSITION) - *pWorldPosition;
	_float	fDistance = D3DXVec3Length(&vDir);

	if (fDistance > 60.f)
		return false;



	return true;
}

CFrustum_Culling * CFrustum_Culling::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CFrustum_Culling*		pInstance = new CFrustum_Culling(pGraphic_Device);

	if (FAILED(pInstance->Ready_Frustum()))
	{
		_MSG_BOX("CFrustum_Culling Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CFrustum_Culling::Clone(void)
{
	AddRef();
	return this;
}

HRESULT CFrustum_Culling::Ready_Frustum(void)
{
	m_vOriginal_Points[0] = _vec3(-1.f, 1.f, 0.f);
	m_vOriginal_Points[1] = _vec3(1.f, 1.f, 0.f);
	m_vOriginal_Points[2] = _vec3(1.f, -1.f, 0.f);
	m_vOriginal_Points[3] = _vec3(-1.f, -1.f, 0.f);

	m_vOriginal_Points[4] = _vec3(-1.f, 1.f, 1.f);
	m_vOriginal_Points[5] = _vec3(1.f, 1.f, 1.f);
	m_vOriginal_Points[6] = _vec3(1.f, -1.f, 1.f);
	m_vOriginal_Points[7] = _vec3(-1.f, -1.f, 1.f);

	return NOERROR;
}

void CFrustum_Culling::Free(void)
{
	Safe_Release(m_pStandard);

	CComponent::Free();
}
