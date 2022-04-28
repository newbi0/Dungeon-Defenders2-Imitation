#include "stdafx.h"
#include "Camera_Free.h"
#include "Input_Device.h"
#include "Object_Manager.h"
#include "Input_Device.h"

CCamera_Free::CCamera_Free(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
{
	m_pObject_Manager->AddRef();
	m_pInput_Device->AddRef();
}

CCamera_Free::CCamera_Free(const CCamera_Free & rhs)
	: CCamera(rhs)
	, m_pObject_Manager(rhs.m_pObject_Manager)
	, m_pInput_Device(rhs.m_pInput_Device)
{
}

_int CCamera_Free::Update_GameObject(const _float & fTimeDelta)
{
 	CInput_Device*	pInput_Device = CInput_Device::Get_Instance();

	_vec3			vLook;
	memcpy(&vLook, &m_matrix[MATRIX_WORLD].m[2][0], sizeof(_vec3));

	if (pInput_Device->Get_MouseState(CInput_Device::DIM_RBUTTON) & 0x80)
	{
		if (_long lMouseMoveX = pInput_Device->Get_MouseMove(CInput_Device::DIMM_X))
		{
			_matrix			matRot;

			D3DXMatrixRotationY(&matRot, D3DXToRadian(lMouseMoveX * fTimeDelta * 5.f));

			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

			m_Camera_Desc.vAt = m_Camera_Desc.vEye + vLook;
		}

		if (_long lMouseMoveY = pInput_Device->Get_MouseMove(CInput_Device::DIMM_Y))
		{
			_vec3			vRight;
			memcpy(&vRight, &m_matrix[MATRIX_WORLD].m[0][0], sizeof(_vec3));

			_matrix			matRot;
			D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(lMouseMoveY * fTimeDelta * 5.f));

			D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

			m_Camera_Desc.vAt = m_Camera_Desc.vEye + vLook;
		}
	}

	//if (_long lWheel = pInput_Device->Get_MouseMove(CInput_Device::DIMM_Z))
	//{
	//	_vec4	vEye = m_Camera_Desc.vEye;
	//	_vec4	vLook = m_Camera_Desc.vAt - m_Camera_Desc.vEye;
	//	vEye += XMVector3Normalize(vLook * (lWheel * 0.1f));
	//	_float iDistance = XMVectorGetX(XMVector3Length(m_Camera_Desc.vAt - vEye));
	//	if (8.f > iDistance)
	//		return _RUN;
	//	m_Camera_Desc.vEye = vEye;
	//}

	if (pInput_Device->Get_KeyState(DIK_PGDN) & 0x80)
	{
		m_Camera_Desc.vAt.y -= 0.5f;
		m_Camera_Desc.vEye.y -= 0.5f;
	}

	if (pInput_Device->Get_KeyState(DIK_PGUP) & 0x80)
	{
		m_Camera_Desc.vAt.y += 0.5f;
		m_Camera_Desc.vEye.y += 0.5f;
	}

	if (pInput_Device->Get_KeyState(DIK_A) & 0x80)
	{
		_vec3			vRight;
		memcpy(&vRight, &m_matrix[MATRIX_WORLD].m[0][0], sizeof(_vec3));

		m_Camera_Desc.vEye -= *D3DXVec3Normalize(&vRight, &vRight) * 1.f;
		m_Camera_Desc.vAt -= *D3DXVec3Normalize(&vRight, &vRight) * 1.f;
	}

	if (pInput_Device->Get_KeyState(DIK_D) & 0x80)
	{
		_vec3			vRight;
		memcpy(&vRight, &m_matrix[MATRIX_WORLD].m[0][0], sizeof(_vec3));

		m_Camera_Desc.vEye += *D3DXVec3Normalize(&vRight, &vRight) * 1.f;
		m_Camera_Desc.vAt += *D3DXVec3Normalize(&vRight, &vRight) * 1.f;
	}

	if (pInput_Device->Get_KeyState(DIK_W) & 0x80)
	{
		_vec3			vLook;
		memcpy(&vLook, &m_matrix[MATRIX_WORLD].m[2][0], sizeof(_vec3));

		m_Camera_Desc.vEye += *D3DXVec3Normalize(&vLook, &vLook) * 1.f;
		m_Camera_Desc.vAt += *D3DXVec3Normalize(&vLook, &vLook) * 1.f;
	}

	if (pInput_Device->Get_KeyState(DIK_S) & 0x80)
	{
		_vec3			vLook;
		memcpy(&vLook, &m_matrix[MATRIX_WORLD].m[2][0], sizeof(_vec3));

		m_Camera_Desc.vEye -= *D3DXVec3Normalize(&vLook, &vLook) * 1.f;
		m_Camera_Desc.vAt -= *D3DXVec3Normalize(&vLook, &vLook) * 1.f;
	}

	return CCamera::Update_GameObject(fTimeDelta);
}

_int CCamera_Free::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return CCamera::LastUpdate_GameObject(fTimeDelta);
}

CCamera_Free * CCamera_Free::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const CAMERA_DESC * pCameraDesc, const PROJECTION_DESC * pProjectDesc)
{
	CCamera_Free*	pInstance = new CCamera_Free(pGraphic_Device);
	if (FAILED(pInstance->Ready_Camera(pCameraDesc, pProjectDesc)))
	{
		_MSG_BOX("CCamera_Free Creating Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Free::Clone(const _vec3 * pPos)
{
	return new CCamera_Free(*this);
}

CGameObject * CCamera_Free::Clone(const _matrix & matWorld)
{
	return new CCamera_Free(*this);
}

HRESULT CCamera_Free::Ready_Camera(const CAMERA_DESC * pCameraDesc, const PROJECTION_DESC * pProjectDesc)
{
	CCamera::Ready_GameObject_Prototype(pCameraDesc, pProjectDesc);

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	return NOERROR;
}

void CCamera_Free::Free(void)
{
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pInput_Device);

	CCamera::Free();
}
