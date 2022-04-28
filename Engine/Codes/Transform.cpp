#include "Transform.h"

_USING(Engine)

CTransform::CTransform(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_matWorld(rhs.m_matWorld)
{

}

const _matrix* CTransform::Get_WorldMatrix(void) const
{
	if (nullptr == m_pParentMatrix)
	{
		return &m_matWorld;
	}

	else
	{
		_matrix		matWorld;
		matWorld = m_matWorld * *m_pParentMatrix;
		return &matWorld;
	}	
}

const _matrix * CTransform::Get_WorldMatrix_NotRot(void) const
{
	_vec3		vRight, vUp, vLook;
	_matrix		matWorld;
	D3DXMatrixIdentity(&matWorld);

	vRight = _vec3(1.f, 0.f, 0.f);
	vUp = _vec3(0.f, 1.f, 0.f);
	vLook = _vec3(0.f, 0.f, 1.f);

	vRight = vRight * Get_Scale(INFO_RIGHT);
	vUp = vUp * Get_Scale(INFO_UP);
	vLook = vLook * Get_Scale(INFO_LOOK);

	memcpy(&matWorld.m[INFO_RIGHT], &vRight, sizeof(_vec3));
	memcpy(&matWorld.m[INFO_UP], &vUp, sizeof(_vec3));
	memcpy(&matWorld.m[INFO_LOOK], &vLook, sizeof(_vec3));
	memcpy(&matWorld.m[INFO_POSITION], &m_matWorld.m[INFO_POSITION], sizeof(_vec3));

	if (nullptr == m_pParentMatrix)
	{
		return &matWorld;
	}

	else
	{
		matWorld *= *m_pParentMatrix;
		return &matWorld;
	}
}

void CTransform::Set_Infomation(INFORMATION eInfo, const _vec3 * pInfomation)
{
	memcpy(&m_matWorld.m[eInfo][0], pInfomation, sizeof(_vec3));
}

void CTransform::Set_Infomation(INFORMATION eInfo, const _float & fX, const _float & fY, const _float & fZ)
{
	memcpy(&m_matWorld.m[eInfo][0], &_vec3(fX, fY, fZ), sizeof(_vec3));
}

void CTransform::Go_Straight(const _float & fSpeedPerSec)
{
	_vec3			vLook;
	memcpy(&vLook, &m_matWorld.m[INFO_LOOK][0], sizeof(_vec3));

	D3DXVec3Normalize(&vLook, &vLook);

	*((_vec3*)&m_matWorld.m[INFO_POSITION][0]) += vLook * fSpeedPerSec;
}

void CTransform::Go_Direction(const _vec3 & vDir, const _float & fSpeed)
{
	_vec3	vDir_f;
	D3DXVec3Normalize(&vDir_f, &vDir);
	*((_vec3*)&m_matWorld.m[INFO_POSITION][0]) += vDir_f * fSpeed;
}

void CTransform::Go_Sliding(const _vec3 & vDir, const _vec3 & vNormal, const _float & fSpeed)
{
	_vec3	vProj = vNormal * D3DXVec3Dot(&vNormal, &(vDir * -1.f));
	_vec3	vNor;
	D3DXVec3Normalize(&vNor, &(vDir + vProj));
	*((_vec3*)&m_matWorld.m[INFO_POSITION][0]) -= vNor * fSpeed;
}

_bool CTransform::Jump(const _float & fSpeed)
{
	_vec3	vUp = _vec3(0.f, 1.f, 0.f);
	*((_vec3*)&m_matWorld.m[INFO_POSITION][0]) += vUp * fSpeed;

	return true;
}

void CTransform::RotationX(const _float & fAngleX)
{
	_vec3		vLook, vUp;
	memcpy(&vLook, &m_matWorld.m[INFO_LOOK][0], sizeof(_vec3));
	memcpy(&vUp, &m_matWorld.m[INFO_UP][0], sizeof(_vec3));

	_matrix		matRot;
	D3DXMatrixRotationX(&matRot, fAngleX);

	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_LOOK][0], &vLook, &matRot);
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_UP][0], &vUp, &matRot);
}

void CTransform::RotationY(const _float & fAngleY, const _bool& isFix)
{
	if (false == isFix)
	{
		_vec3		vLook, vRight;
		memcpy(&vLook, &m_matWorld.m[INFO_LOOK][0], sizeof(_vec3));
		memcpy(&vRight, &m_matWorld.m[INFO_RIGHT][0], sizeof(_vec3));

		_matrix		matRot;
		D3DXMatrixRotationY(&matRot, fAngleY);

		D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_LOOK][0], &vLook, &matRot);
		D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_RIGHT][0], &vRight, &matRot);
	}
	else
	{
		_matrix	matWorld = *Get_WorldMatrix_NotRot();

		_vec3		vLook, vRight;
		memcpy(&vLook, &matWorld.m[INFO_LOOK][0], sizeof(_vec3));
		memcpy(&vRight, &matWorld.m[INFO_RIGHT][0], sizeof(_vec3));

		_matrix		matRot;
		D3DXMatrixRotationY(&matRot, fAngleY);

		D3DXVec3TransformNormal((_vec3*)&matWorld.m[INFO_LOOK][0], &vLook, &matRot);
		D3DXVec3TransformNormal((_vec3*)&matWorld.m[INFO_RIGHT][0], &vRight, &matRot);

		m_matWorld = matWorld;

	}
}

void CTransform::RotationY(const _float & fAngleY, const _float & fSpeed)
{
	_vec3		vLook, vRight;
	memcpy(&vLook, &m_matWorld.m[INFO_LOOK][0], sizeof(_vec3));
	memcpy(&vRight, &m_matWorld.m[INFO_RIGHT][0], sizeof(_vec3));

	_matrix		matRot;
	D3DXMatrixRotationY(&matRot, fAngleY * fSpeed);

	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_LOOK][0], &vLook, &matRot);
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_RIGHT][0], &vRight, &matRot);
}


void CTransform::RotationZ(const _float & fAngleZ)
{
	_vec3		vUp, vRight;
	memcpy(&vUp, &m_matWorld.m[INFO_UP][0], sizeof(_vec3));
	memcpy(&vRight, &m_matWorld.m[INFO_RIGHT][0], sizeof(_vec3));

	_matrix		matRot;
	D3DXMatrixRotationZ(&matRot, fAngleZ);

	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_UP][0], &vUp, &matRot);
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[INFO_RIGHT][0], &vRight, &matRot);
}

HRESULT CTransform::Ready_Transform(void)
{
	D3DXMatrixIdentity(&m_matWorld);

	return NOERROR;
}

void CTransform::Scaling(_vec3 vScale) // 절대적인 비율로 증가시키낟.
{
	_vec3		vRight, vUp, vLook;

	memcpy(&vRight, &m_matWorld.m[INFO_RIGHT][0], sizeof(_vec3));
	memcpy(&vUp, &m_matWorld.m[INFO_UP][0], sizeof(_vec3));
	memcpy(&vLook, &m_matWorld.m[INFO_LOOK][0], sizeof(_vec3));

	vRight = *D3DXVec3Normalize(&vRight, &vRight) * vScale.x;
	vUp = *D3DXVec3Normalize(&vUp, &vUp) * vScale.y;
	vLook = *D3DXVec3Normalize(&vLook, &vLook) * vScale.z;

	memcpy(&m_matWorld.m[INFO_RIGHT][0], &vRight, sizeof(_vec3));
	memcpy(&m_matWorld.m[INFO_UP][0], &vUp, sizeof(_vec3));
	memcpy(&m_matWorld.m[INFO_LOOK][0], &vLook, sizeof(_vec3));
}

void CTransform::Scaling(const _float & fX, const _float & fY, const _float & fZ)
{
	_vec3		vRight, vUp, vLook;

	memcpy(&vRight, &m_matWorld.m[INFO_RIGHT][0], sizeof(_vec3));
	memcpy(&vUp, &m_matWorld.m[INFO_UP][0], sizeof(_vec3));
	memcpy(&vLook, &m_matWorld.m[INFO_LOOK][0], sizeof(_vec3));

	vRight = *D3DXVec3Normalize(&vRight, &vRight) * fX;
	vUp = *D3DXVec3Normalize(&vUp, &vUp) * fY;
	vLook = *D3DXVec3Normalize(&vLook, &vLook) * fZ;

	memcpy(&m_matWorld.m[INFO_RIGHT][0], &vRight, sizeof(_vec3));
	memcpy(&m_matWorld.m[INFO_UP][0], &vUp, sizeof(_vec3));
	memcpy(&m_matWorld.m[INFO_LOOK][0], &vLook, sizeof(_vec3));
}

void CTransform::Symmetry_Y(void)
{
	_vec3	vRight;
	memcpy(&vRight, &m_matWorld.m[INFO_RIGHT][0], sizeof(_vec3));
	vRight *= -1.f;
	memcpy(&m_matWorld.m[INFO_RIGHT][0], &vRight, sizeof(_vec3));
}


HRESULT CTransform::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName)
{
	if (nullptr == pEffect)
		return E_FAIL;

	if (nullptr == m_pParentMatrix)
	{
		if (FAILED(pEffect->SetMatrix(pConstantName, &m_matWorld)))
			return E_FAIL;
	}

	else
	{
		_matrix		matWorld;
		matWorld = m_matWorld * *m_pParentMatrix;
		if (FAILED(pEffect->SetMatrix(pConstantName, &matWorld)))
			return E_FAIL;
	}

	return NOERROR;
}

CTransform * CTransform::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CTransform*		pInstance = new CTransform(pGraphic_Device);
	
	if (FAILED(pInstance->Ready_Transform()))
	{
		_MSG_BOX("CTransform Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void)
{
	return new CTransform(*this);
}

void CTransform::Free(void)
{
	Engine::CComponent::Free();
}
