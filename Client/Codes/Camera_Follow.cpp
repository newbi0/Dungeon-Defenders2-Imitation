#include "stdafx.h"
#include "Camera_Follow.h"
#include "Defines.h"
#include "Object_Manager.h"
#include "Player.h"

const float AT_HIGHT = 3.f;
const float SHAKE_END_POINT = 0.05f;
const float ROT_SPEED = 0.5f;
const float ZOOM_SPEED = 0.2f;
const float ZOOM_MAX = 3.f;
const float EYE_HIGHT_MAX = 5.f;
const float EYE_HIGHT_MIN = 0.3f;
const float ARRIVE_DISTANCE = 1.f;

CCamera_Follow::CCamera_Follow(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CCamera(pGraphic_Device)
	, m_pObject_Manager(CObject_Manager::Get_Instance())
	, m_pInput_Device(CInput_Device::Get_Instance())
{
	m_pObject_Manager->AddRef();
}

CCamera_Follow::CCamera_Follow(const CCamera_Follow & rhs)
	: CCamera(rhs)
	, m_pObject_Manager(rhs.m_pObject_Manager)
	, m_pInput_Device(rhs.m_pInput_Device)
	, m_pTarget(rhs.m_pTarget)
	, m_fLimitY(rhs.m_fLimitY)
	, m_isMouseMove(rhs.m_isMouseMove)
	, m_isLookFromFront(rhs.m_isLookFromFront)
	, m_vDistance(rhs.m_vDistance)
	, m_fSpeed(rhs.m_fSpeed)
	, m_isChangeTarget(rhs.m_isChangeTarget)
	, m_vChangedEye(rhs.m_vChangedEye)
	, m_vChangedAt(rhs.m_vChangedAt)
	, m_iShakeNum(rhs.m_iShakeNum)
{
	m_pObject_Manager->AddRef();
	m_pInput_Device->AddRef();
}

void CCamera_Follow::Set_Target(const CUnit * pTarget, const _vec3& vDistance
	, const _float& fSpeed, const _bool& isLookFromFront)
{
	if (nullptr != pTarget)
	{
		//If the target to change is different to the current target.
		if (m_pTarget != pTarget)
		{
			//Set Changed Information
			m_isChangeTarget = true;
			m_isLookFromFront = isLookFromFront;
			m_vDistance = vDistance;
			m_fSpeed = fSpeed;

			_matrix	matTarget = *(pTarget->Get_WorldMatrix());

			_vec3	vBackDir = Compute_Back_Direction(matTarget);

			D3DXVec3Normalize(&vBackDir, &vBackDir);

			//Set Changed Eye, At, Target
			_vec3	vTargetPos = static_cast<_vec3>(matTarget.m[INFO_POSITION]);

			m_vChangedEye = vTargetPos - (vBackDir * m_vDistance.z);
			m_vChangedEye.y += m_fLimitY;

			m_vChangedAt = vTargetPos;
			m_vChangedAt.y += AT_HIGHT;

			m_pTarget = pTarget;
		}
		//If the target to change is same the current target.
		else
		{
			m_fSpeed = fSpeed;
			m_vDistance = vDistance;
		}
	}
}

void CCamera_Follow::Set_isShake(const _bool & isShake, const _float & fShakeAmount, const _uint & iShakeNum, const _float & fSpeed)
{
	m_isCameraMove = false;
	m_isShake = isShake;
	m_isShakeInit = false;
	m_fShakeAmount = fShakeAmount;
	m_iShakeMaxNum = iShakeNum;
	m_fShakeSpeed = fSpeed;
}

HRESULT CCamera_Follow::Init_Camera(CUnit * pTarget)
{
	if (nullptr == pTarget)
		return E_FAIL;

	m_pTarget = pTarget;

	m_vDistance = _vec3(0.f, 7.f, 3.f);
	m_fLimitY = 7.f;

	_matrix matTarget = *m_pTarget->Get_WorldMatrix();

	m_Camera_Desc.vEye = _vec3(matTarget.m[3][0] + m_vDistance.x, matTarget.m[3][1] + m_vDistance.y, matTarget.m[3][2] + m_vDistance.z);
	m_Camera_Desc.vAt = _vec3(matTarget.m[3]);

	return NOERROR;
}

_int CCamera_Follow::Update_GameObject(const _float & fTimeDelta)
{
	if (nullptr == m_pTarget)
		return _RUN;

	//Shake
	if (false == m_isCameraMove
		&& true == m_isShake)
	{
		Shake_Camera(fTimeDelta);
		return _RUN;
	}

	_matrix matTarget = *(m_pTarget->Get_WorldMatrix());

	if (true == m_isMouseMove)
	{
		//Rotation
		if (true == m_isMouseRot)
		{
			_vec3	m_vOriEye = _vec3(matTarget.m[3][0], matTarget.m[3][1] + m_vDistance.y, matTarget.m[3][2]);

			if (_long lMouseMovex = m_pInput_Device->Get_MouseMove(CInput_Device::DIMM_X))
			{
				_matrix		matRot = *(m_pTarget->Get_matRotY());
				memcpy(&matRot.m[3], &m_Camera_Desc.vAt, sizeof(_vec3));

				_vec3	vDist = m_Camera_Desc.vEye - m_Camera_Desc.vAt;

				D3DXVec3TransformCoord(&m_Camera_Desc.vEye, &vDist, &matRot);
			}

			if (0x80 & m_pInput_Device->Get_MouseState(CInput_Device::DIM_RBUTTON))
			{
				if (_long lMouseMoveY = m_pInput_Device->Get_MouseMove(CInput_Device::DIMM_Y))
				{
					_matrix		matRot;
					D3DXMatrixRotationAxis(&matRot, &(_vec3)m_matrix[MATRIX_WORLD].m[0], lMouseMoveY * fTimeDelta * ROT_SPEED);
					memcpy(&matRot.m[3], &m_Camera_Desc.vAt, sizeof(_vec3));

					_vec3	vDist = m_Camera_Desc.vEye - m_Camera_Desc.vAt;
					D3DXVec3TransformCoord(&vDist, &vDist, &matRot);

					if (vDist.y > m_vOriEye.y - EYE_HIGHT_MAX
						|| vDist.y < m_vOriEye.y + EYE_HIGHT_MIN)
					{
						m_Camera_Desc.vEye = vDist;
						m_fLimitY = vDist.y - matTarget.m[3][1];
					}
				}
			}

			//Zoom
			if (_long lMouseWheel = m_pInput_Device->Get_MouseMove(CInput_Device::DIM_MBUTTON))
			{
				m_vDistance.z += lMouseWheel * fTimeDelta * ZOOM_SPEED * -1.f;

				if (m_vDistance.z < ZOOM_MAX)
					m_vDistance.z = ZOOM_MAX;
			}
		}

		//Fix At
		m_Camera_Desc.vAt = matTarget.m[3];
		m_Camera_Desc.vAt.y += AT_HIGHT;

		Camera_Move(fTimeDelta, matTarget);
	}

	return CCamera::Update_GameObject(fTimeDelta);
}

_int CCamera_Follow::LastUpdate_GameObject(const _float & fTimeDelta)
{
	return CCamera::LastUpdate_GameObject(fTimeDelta);
}

void CCamera_Follow::Shake_Camera(const _float& fTimeDelta)
{
	if (false == m_isShakeInit)
	{
		m_vOriginalEye = m_Camera_Desc.vEye;
		m_vOriginalAt = m_Camera_Desc.vAt;
		m_isShakeInit = true;
	}

	//Up
	if (false == m_isShakeDown)
	{
		m_Camera_Desc.vEye.y += m_fShakeSpeed * fTimeDelta;
		m_Camera_Desc.vAt.y += m_fShakeSpeed * fTimeDelta;

		// Up -> Down
		if (m_Camera_Desc.vAt.y - m_vOriginalAt.y > m_fShakeAmount)
			m_isShakeDown = true;
	}

	//Down
	else
	{
		m_Camera_Desc.vEye.y -= m_fShakeSpeed * fTimeDelta;
		m_Camera_Desc.vAt.y -= m_fShakeSpeed * fTimeDelta;

		// Down -> Up
		if (m_Camera_Desc.vAt.y - m_vOriginalAt.y < m_fShakeAmount * -1.f)
			m_isShakeDown = false;

		// Down End
		if (m_Camera_Desc.vAt.y - m_vOriginalAt.y < SHAKE_END_POINT)
		{
			++m_iShakeNum;

			//Shake End
			if (m_iShakeNum > m_iShakeMaxNum)
			{
				m_iShakeNum = 0;
				m_Camera_Desc.vEye = m_vOriginalEye;
				m_Camera_Desc.vAt = m_vOriginalAt;

				m_isShakeDown = false;
				m_isShake = false;
				m_isCameraMove = true;
			}
		}
	}
}

void CCamera_Follow::Camera_Move(const _float & fTimeDelta, const _matrix& matTarget)
{
	_vec3	vDir;
	ZeroMemory(vDir, sizeof(_vec3));

	//If target isn't changing.
	if (false == m_isChangeTarget)
	{
		_vec3	vBackDir = Compute_Back_Direction(matTarget);

		D3DXVec3Normalize(&vBackDir, &vBackDir);

		_vec3	vEyeFinal = static_cast<_vec3>(matTarget.m[INFO_POSITION]) - (vBackDir * m_vDistance.z);
		vEyeFinal.y += m_fLimitY;

		vDir = vEyeFinal - m_Camera_Desc.vEye;
	}

	//If target is changing.
	else
	{
		vDir = m_vChangedEye - m_Camera_Desc.vEye;
		if (ARRIVE_DISTANCE > D3DXVec3Length(&vDir))
		{
			m_fSpeed = BASIC_SPEED;
			m_isChangeTarget = false;
			return;
		}
	}

	if (ARRIVE_DISTANCE > D3DXVec3Length(&vDir))
		return;

	D3DXVec3Normalize(&vDir, &vDir);
	m_Camera_Desc.vEye += vDir * fTimeDelta * m_fSpeed;
}

const _vec3 CCamera_Follow::Compute_Back_Direction(const _matrix & matTarget)
{
	_vec3	vBackDir;
	ZeroMemory(vBackDir, sizeof(_vec3));

	//For camera look at the target from back.
	if (false == m_isLookFromFront)
		vBackDir = static_cast<_vec3>(matTarget.m[INFO_LOOK]) * -1.f;
	//For camera look at the target from front.
	else
		vBackDir = static_cast<_vec3>(matTarget.m[INFO_LOOK]);

	return vBackDir;
}

CCamera_Follow * CCamera_Follow::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const CAMERA_DESC * pCameraDesc, const PROJECTION_DESC * pProjectDesc)
{
	CCamera_Follow*	pInstance = new CCamera_Follow(pGraphic_Device);

	if (FAILED(pInstance->Ready_Camera(pCameraDesc, pProjectDesc)))
	{
		_MSG_BOX("CCamera_Follow Clone Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CCamera_Follow::Clone(const _vec3 * pPos)
{
	return new CCamera_Follow(*this);
}

CGameObject * CCamera_Follow::Clone(const _matrix & matWorld)
{
	return new CCamera_Follow(*this);
}

HRESULT CCamera_Follow::Ready_Camera(const CAMERA_DESC * pCameraDesc, const PROJECTION_DESC * pProjectDesc)
{
	CCamera::Ready_GameObject_Prototype(pCameraDesc, pProjectDesc);

	return NOERROR;
}

void CCamera_Follow::Free(void)
{
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pInput_Device);
	CCamera::Free();
}
