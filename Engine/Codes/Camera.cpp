#include "Camera.h"

_USING(Engine)

CCamera::CCamera(LPDIRECT3DDEVICE9  pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_Camera_Desc(rhs.m_Camera_Desc)
	, m_Project_Desc(rhs.m_Project_Desc)	
{
	for (size_t i = 0; i < MATRIX_END; ++i)
	{
		m_matrix[i] = rhs.m_matrix[i];
	}

}

HRESULT CCamera::Ready_GameObject_Prototype(const CAMERA_DESC* pCameraDesc, const PROJECTION_DESC* pProjectDesc)
{
	m_Camera_Desc = *pCameraDesc;

	D3DXMatrixLookAtLH(&m_matrix[MATRIX_VIEW], &m_Camera_Desc.vEye, &m_Camera_Desc.vAt, &m_Camera_Desc.vUp);
	D3DXMatrixInverse(&m_matrix[MATRIX_WORLD], nullptr, &m_matrix[MATRIX_VIEW]);

	m_Project_Desc = *pProjectDesc;
	D3DXMatrixPerspectiveFovLH(&m_matrix[MATRIX_PROJ], m_Project_Desc.fFovY, m_Project_Desc.fAspect, m_Project_Desc.fNear, m_Project_Desc.fFar);

	return NOERROR;
}

HRESULT CCamera::Ready_GameObject(void)
{
	return NOERROR;
}

_int CCamera::Update_GameObject(const _float & fTimeDelta)
{

	return _RUN;
}

_int CCamera::LastUpdate_GameObject(const _float & fTimeDelta)
{
	D3DXMatrixLookAtLH(&m_matrix[MATRIX_VIEW], &m_Camera_Desc.vEye, &m_Camera_Desc.vAt, &m_Camera_Desc.vUp);
	D3DXMatrixInverse(&m_matrix[MATRIX_WORLD], nullptr, &m_matrix[MATRIX_VIEW]);

	D3DXMatrixPerspectiveFovLH(&m_matrix[MATRIX_PROJ], m_Project_Desc.fFovY, m_Project_Desc.fAspect, m_Project_Desc.fNear, m_Project_Desc.fFar);

	if (nullptr == m_pGraphic_Device)
		return _EXIT;

	m_pGraphic_Device->SetTransform(D3DTS_VIEW, &m_matrix[MATRIX_VIEW]);
	m_pGraphic_Device->SetTransform(D3DTS_PROJECTION, &m_matrix[MATRIX_PROJ]);


	return _RUN;
}

void CCamera::Free(void)
{
	CGameObject::Free();
}
