#include "stdafx.h"
#include "Camera_Light.h"
#include "Input_Device.h"
#include "Object_Manager.h"
#include "Target_Manager.h"
#include "Input_Device.h"
#include "Defines.h"

CCamera_Light::CCamera_Light(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
{
	m_pObject_Manager->AddRef();
	m_pTarget_Manager->AddRef();
	m_pInput_Device->AddRef();
}

CCamera_Light::CCamera_Light(const CCamera_Light & rhs)
	: CCamera(rhs)
	, m_pObject_Manager(rhs.m_pObject_Manager)
	, m_pTarget_Manager(CTarget_Manager::Get_Instance())
	, m_pInput_Device(rhs.m_pInput_Device)
{
	m_pTarget_Manager->AddRef();
}

_int CCamera_Light::Update_GameObject(const _float & fTimeDelta)
{
	_vec3	vPos, vAt, vDir;

	vDir = _vec3(1.f, 1.f, 1.f);

	vAt = (_vec3)((CCamera*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera"))->Get_Matrix(CCamera::MATRIX_WORLD);
	vPos = vAt;
	vPos.x += 500.f * vDir.x;
	vPos.z += 500.f * vDir.z;
	vPos.y += 500.f;

	D3DXMatrixOrthoLH(&m_matLightProj, 500.f, 500.f, 1.f, 10000.f);
	D3DXMatrixLookAtLH(&m_matLightView, &vPos, &vAt, &_vec3(0.f, 1.f, 0.f));

	return _RUN;
}

_int CCamera_Light::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return CCamera::LastUpdate_GameObject(fTimeDelta);
}

CCamera_Light * CCamera_Light::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const CAMERA_DESC * pCameraDesc, const PROJECTION_DESC * pProjectDesc)
{
	CCamera_Light*	pInstance = new CCamera_Light(pGraphic_Device);
	if (FAILED(pInstance->Ready_Camera(pCameraDesc, pProjectDesc)))
	{
		_MSG_BOX("CCamera_Light Creating Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Light::Clone(const _vec3 * pPos)
{
	return new CCamera_Light(*this);
}

CGameObject * CCamera_Light::Clone(const _matrix & matWorld)
{
	return new CCamera_Light(*this);
}

HRESULT CCamera_Light::Ready_Camera(const CAMERA_DESC * pCameraDesc, const PROJECTION_DESC * pProjectDesc)
{
	CCamera::Ready_GameObject_Prototype(pCameraDesc, pProjectDesc);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	return NOERROR;
}

void CCamera_Light::Free(void)
{
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pInput_Device);

	CCamera::Free();
}
