#include "Collider.h"
#include "Shader.h"
#include "Input_Device.h"

CCollider::CCollider(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
	m_pInput_Device = CInput_Device::Get_Instance();
	m_pInput_Device->AddRef();
}

HRESULT CCollider::Set_Collider(TYPE eType, const _vec3 * pMin, const _vec3 * pMax, CTransform * pTransform)
{
	if (nullptr == pMin
		|| nullptr == pMax)
		return E_FAIL;

	D3DXMatrixIdentity(&m_matWorld);

	m_eType = eType;

	m_pRelevant_TransformCom = pTransform;
	m_pRelevant_TransformCom->AddRef();

	m_vMin = *pMin;
	m_vMax = *pMax;

	VTXCOL*	pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	//앞면
	pVertices[0].vPosition = _vec3(pMin->x, pMax->y, pMin->z);
	pVertices[1].vPosition = _vec3(pMax->x, pMax->y, pMin->z);
	pVertices[2].vPosition = _vec3(pMax->x, pMin->y, pMin->z);
	pVertices[3].vPosition = _vec3(pMin->x, pMin->y, pMin->z);

	//뒷면
	pVertices[4].vPosition = _vec3(pMin->x, pMax->y, pMax->z);
	pVertices[5].vPosition = _vec3(pMax->x, pMax->y, pMax->z);
	pVertices[6].vPosition = _vec3(pMax->x, pMin->y, pMax->z);
	pVertices[7].vPosition = _vec3(pMin->x, pMin->y, pMax->z);

	m_pVB->Unlock();

	m_vCenter = (pVertices[0].vPosition + pVertices[6].vPosition) * 0.5f;

	m_vProjAxis[0] = (pVertices[1].vPosition + pVertices[6].vPosition) * 0.5f - m_vCenter;
	m_vProjAxis[1] = (pVertices[1].vPosition + pVertices[4].vPosition) * 0.5f - m_vCenter;
	m_vProjAxis[2] = (pVertices[5].vPosition + pVertices[7].vPosition) * 0.5f - m_vCenter;

	m_vAlignAxis[0] = pVertices[2].vPosition - pVertices[3].vPosition;
	m_vAlignAxis[1] = pVertices[0].vPosition - pVertices[3].vPosition;
	m_vAlignAxis[2] = pVertices[7].vPosition - pVertices[3].vPosition;

	for (_uint i = 0; i < 3; ++i)
	{
		D3DXVec3Normalize(&m_vAlignAxis[i], &m_vAlignAxis[i]);
	}


	return NOERROR;
}


void CCollider::Delete_listCollision(const CCollider * pCollider)
{
	list<const CCollider*>::iterator iter = m_listCollision.begin();
	for (; iter != m_listCollision.end(); ++iter)
	{
		if (*iter == pCollider)
		{
			iter = m_listCollision.erase(iter);
			break;
		}
	}
}

HRESULT CCollider::Ready_Collider(CShader* pShader)
{
	if (nullptr == pShader)
		return E_FAIL;

	m_pShaderCom = pShader;
	m_pShaderCom->AddRef();

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(sizeof(VTXCOL) * 8, 0, D3DFVF_XYZ | D3DFVF_TEX0, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	VTXCOL*	pVertices = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertices, 0);

	pVertices[0].vPosition = _vec3(-0.5f, 0.5f, -0.5f);
	pVertices[0].dwColor = 0;

	pVertices[1].vPosition = _vec3(0.5f, 0.5f, -0.5f);
	pVertices[1].dwColor = 0;

	pVertices[2].vPosition = _vec3(0.5f, -0.5f, -0.5f);
	pVertices[2].dwColor = 0;

	pVertices[3].vPosition = _vec3(-0.5f, -0.5f, -0.5f);
	pVertices[3].dwColor = 0;

	pVertices[4].vPosition = _vec3(-0.5f, 0.5f, 0.5f);
	pVertices[4].dwColor = 0;

	pVertices[5].vPosition = _vec3(0.5f, 0.5f, 0.5f);
	pVertices[5].dwColor = 0;

	pVertices[6].vPosition = _vec3(0.5f, -0.5f, 0.5f);
	pVertices[6].dwColor = 0;

	pVertices[7].vPosition = _vec3(-0.5f, -0.5f, 0.5f);
	pVertices[7].dwColor = 0;

	m_pVB->Unlock();

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(sizeof(INDEX16) * 12, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	INDEX16*	pIndices = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndices, 0);

	// +x
	pIndices[0]._0 = 1; pIndices[0]._1 = 5; pIndices[0]._2 = 6;
	pIndices[1]._0 = 1; pIndices[1]._1 = 6; pIndices[1]._2 = 2;
	// -x
	pIndices[2]._0 = 4; pIndices[2]._1 = 0; pIndices[2]._2 = 3;
	pIndices[3]._0 = 4; pIndices[3]._1 = 3; pIndices[3]._2 = 7;
	// +y
	pIndices[4]._0 = 4; pIndices[4]._1 = 5; pIndices[4]._2 = 1;
	pIndices[5]._0 = 4; pIndices[5]._1 = 1; pIndices[5]._2 = 0;
	// -y
	pIndices[6]._0 = 3; pIndices[6]._1 = 2; pIndices[6]._2 = 6;
	pIndices[7]._0 = 3; pIndices[7]._1 = 6; pIndices[7]._2 = 7;
	// +z
	pIndices[8]._0 = 7; pIndices[8]._1 = 6; pIndices[8]._2 = 5;
	pIndices[9]._0 = 7; pIndices[9]._1 = 5; pIndices[9]._2 = 4;
	// +z
	pIndices[10]._0 = 0; pIndices[10]._1 = 1; pIndices[10]._2 = 2;
	pIndices[11]._0 = 0; pIndices[11]._1 = 2; pIndices[11]._2 = 3;

	m_pIB->Unlock();

	return NOERROR;
}

void CCollider::Update_Collider(void)
{
	auto iter = m_listCollision.begin();
	for (; iter != m_listCollision.end();)
	{
		if (true == (*iter)->Get_isTargetDead())
			iter = m_listCollision.erase(iter);
		else if(nullptr == *iter)
			iter = m_listCollision.erase(iter);
		else
			++iter;
	}

	if (0 == m_listCollision.size())
		m_isCollision = false;
	else
		m_isCollision = true;
}

void CCollider::Render_Collider(void)
{
	if (0x80 & m_pInput_Device->Get_KeyState(DIK_X))
	{
		LPD3DXEFFECT	pEffect = m_pShaderCom->Get_EffectHandle();
		if (nullptr == pEffect)
			return;

		_matrix		matWorld, matView, matProj;
		m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
		m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);

		if (m_eType == TYPE_AABB)
			matWorld = *m_pRelevant_TransformCom->Get_WorldMatrix_NotRot();
		else if (m_eType == TYPE_OBB)
			matWorld = *m_pRelevant_TransformCom->Get_WorldMatrix();

		_matrix matIden;
		D3DXMatrixIdentity(&matIden);
		if (matIden != m_matWorld)
			matWorld = m_matWorld;

		pEffect->SetMatrix("g_matWorld", &matWorld);
		pEffect->SetMatrix("g_matView", &matView);
		pEffect->SetMatrix("g_matProj", &matProj);

		pEffect->Begin(nullptr, 0);

		if (false == m_isCollision)
			pEffect->BeginPass(0);
		else
			pEffect->BeginPass(1);

		if (FAILED(m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, sizeof(VTXCOL))))
			return;
		if (FAILED(m_pGraphic_Device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX0)))
			return;
		if (FAILED(m_pGraphic_Device->SetIndices(m_pIB)))
			return;
		if (FAILED(m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, 8, 0, 12)))
			return;

		pEffect->EndPass();
		pEffect->End();
	}
}

void CCollider::RotationX_Collider(const _float & fAngle)
{
	_vec3		vLook, vRight;
	memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));
	memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));

	_matrix		matRot;
	D3DXMatrixRotationX(&matRot, fAngle);

	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[2][0], &vLook, &matRot);
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[0][0], &vRight, &matRot);
}

void CCollider::RotationY_Collider(const _float & fAngle)
{
	_vec3		vLook, vRight;
	memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));
	memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));

	_matrix		matRot;
	D3DXMatrixRotationY(&matRot, fAngle);

	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[2][0], &vLook, &matRot);
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[0][0], &vRight, &matRot);
}

void CCollider::RotationZ_Collider(const _float & fAngle)
{
	_vec3		vLook, vRight;
	memcpy(&vLook, &m_matWorld.m[2][0], sizeof(_vec3));
	memcpy(&vRight, &m_matWorld.m[0][0], sizeof(_vec3));

	_matrix		matRot;
	D3DXMatrixRotationZ(&matRot, fAngle);

	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[2][0], &vLook, &matRot);
	D3DXVec3TransformNormal((_vec3*)&m_matWorld.m[0][0], &vRight, &matRot);
}

_bool CCollider::Collision_AABB(CCollider * pTargetCollider)
{
	_vec3	vMinWorld, vMaxWorld;		//월드 스페이스 상의 좌표
	_matrix	matWorld = *Get_RelevantWorldMatrix_NotRot();
	D3DXVec3TransformCoord(&vMinWorld, &m_vMin, &matWorld);
	D3DXVec3TransformCoord(&vMaxWorld, &m_vMax, &matWorld);

	_vec3	vTargetMin, vTargetMax;
	_matrix	matTargetWorld = *pTargetCollider->Get_RelevantWorldMatrix_NotRot();

	D3DXVec3TransformCoord(&vTargetMin, pTargetCollider->Get_Min(), &matTargetWorld);
	D3DXVec3TransformCoord(&vTargetMax, pTargetCollider->Get_Max(), &matTargetWorld);

	_float	fMin = 0.f, fMax = 0.f;

	fMin = vTargetMin.x > vMinWorld.x ? vTargetMin.x : vMinWorld.x;
	fMax = vTargetMax.x < vMaxWorld.x ? vTargetMax.x : vMaxWorld.x;

	if (fMin > fMax)
	{
		list<const CCollider*>::iterator iter = m_listCollision.begin();
		for(; iter != m_listCollision.end(); ++iter)
		{
			if (*iter == pTargetCollider)
			{
				iter = m_listCollision.erase(iter);
				pTargetCollider->Delete_listCollision(this);
				break;
			}
		}

		return false;
	}

	fMin = vTargetMin.y > vMinWorld.y ? vTargetMin.y : vMinWorld.y;
	fMax = vTargetMax.y < vMaxWorld.y ? vTargetMax.y : vMaxWorld.y;

	if (fMin > fMax)
	{
		list<const CCollider*>::iterator iter = m_listCollision.begin();
		for (; iter != m_listCollision.end(); ++iter)
		{
			if (*iter == pTargetCollider)
			{
				iter = m_listCollision.erase(iter);
				pTargetCollider->Delete_listCollision(this);
				break;
			}
		}

		return false;
	}

	fMin = vTargetMin.z > vMinWorld.z ? vTargetMin.z : vMinWorld.z;
	fMax = vTargetMax.z < vMaxWorld.z ? vTargetMax.z : vMaxWorld.z;

	if (fMin > fMax)
	{
		list<const CCollider*>::iterator iter = m_listCollision.begin();
		for (; iter != m_listCollision.end(); ++iter)
		{
			if (*iter == pTargetCollider)
			{
				iter = m_listCollision.erase(iter);
				pTargetCollider->Delete_listCollision(this);
				break;
			}
		}

		return false;
	}

	//이미 충돌 되어있다고 list에 추가 되어있으면 return 으로 나가버리기, 아니면 추가하기
	list<const CCollider*>::iterator iter = m_listCollision.begin();
	for (; iter != m_listCollision.end(); ++iter)
	{
		if (*iter == pTargetCollider)
			return true;
	}

	m_listCollision.push_back(pTargetCollider);
	pTargetCollider->Add_listCollision(this);

	return true;
}

_bool CCollider::Collision_OBB(CCollider ** ppTargetCollider, _bool isAddTargetList)
{
	if (nullptr == ppTargetCollider
		|| nullptr == *ppTargetCollider)
		return false;

	_matrix		matSourWorld, matDestWorld;

	matSourWorld = *Get_RelevantWorldMatrix();
	matDestWorld = *(*ppTargetCollider)->Get_RelevantWorldMatrix();

	_vec3		vSourCenter, vDestCenter;
	_vec3		vSourProjAxis[3], vDestProjAxis[3];
	_vec3		vSourAlignAxis[3], vDestAlignAxis[3];

	_float		vDistance[3] = { 0.f };		//투영된 값들


	*D3DXVec3TransformCoord(&vSourCenter, &m_vCenter, &matSourWorld);
	vDestCenter = *D3DXVec3TransformCoord(&vDestCenter, &(*ppTargetCollider)->Get_Center(), &matDestWorld);

	//ProjAxis translate local to world
	for (_uint i = 0; i < 3; ++i)
	{
		D3DXVec3TransformNormal(&vSourProjAxis[i], &m_vProjAxis[i], &matSourWorld);
		D3DXVec3TransformNormal(&vDestProjAxis[i], &(*ppTargetCollider)->Get_ProjAxis(i), &matDestWorld);
					
		D3DXVec3TransformNormal(&vSourAlignAxis[i], &m_vAlignAxis[i], &matSourWorld);
		D3DXVec3TransformNormal(&vDestAlignAxis[i], &(*ppTargetCollider)->Get_AlignAxis(i), &matDestWorld);

		D3DXVec3Normalize(&vSourAlignAxis[i], &vSourAlignAxis[i]);
		D3DXVec3Normalize(&vDestAlignAxis[i], &vDestAlignAxis[i]);
	}

	_vec3		vCenterDir = vDestCenter - vSourCenter;

	for (_uint i = 0; i < 3; ++i)
	{
		vDistance[0] = fabs(D3DXVec3Dot(&vCenterDir, &vSourAlignAxis[i]));

		vDistance[1] = fabs(D3DXVec3Dot(&vSourProjAxis[0], &vSourAlignAxis[i]))
			+ fabs(D3DXVec3Dot(&vSourProjAxis[1], &vSourAlignAxis[i]))
			+ fabs(D3DXVec3Dot(&vSourProjAxis[2], &vSourAlignAxis[i]));

		vDistance[2] = fabs(D3DXVec3Dot(&vDestProjAxis[0], &vSourAlignAxis[i]))
			+ fabs(D3DXVec3Dot(&vDestProjAxis[1], &vSourAlignAxis[i]))
			+ fabs(D3DXVec3Dot(&vDestProjAxis[2], &vSourAlignAxis[i]));

		//Fail
		if (vDistance[0] > vDistance[1] + vDistance[2])
		{
			list<const CCollider*>::iterator iter = m_listCollision.begin();
			for (; iter != m_listCollision.end(); ++iter)
			{
				if (*iter == (*ppTargetCollider))
				{
					iter = m_listCollision.erase(iter);
					(*ppTargetCollider)->Delete_listCollision(this);
					break;
				}
			}

			return false;
		}
	}

	for (_uint i = 0; i < 3; ++i)
	{
		vDistance[0] = fabs(D3DXVec3Dot(&vCenterDir, &vDestAlignAxis[i]));

		vDistance[1] = fabs(D3DXVec3Dot(&vSourProjAxis[0], &vDestAlignAxis[i]))
			+ fabs(D3DXVec3Dot(&vSourProjAxis[1], &vDestAlignAxis[i]))
			+ fabs(D3DXVec3Dot(&vSourProjAxis[2], &vDestAlignAxis[i]));

		vDistance[2] = fabs(D3DXVec3Dot(&vDestProjAxis[0], &vDestAlignAxis[i]))
			+ fabs(D3DXVec3Dot(&vDestProjAxis[1], &vDestAlignAxis[i]))
			+ fabs(D3DXVec3Dot(&vDestProjAxis[2], &vDestAlignAxis[i]));

		//Fail
		if (vDistance[0] > vDistance[1] + vDistance[2])
		{
			list<const CCollider*>::iterator iter = m_listCollision.begin();
			for (; iter != m_listCollision.end(); ++iter)
			{
				if (*iter == (*ppTargetCollider))
				{
					iter = m_listCollision.erase(iter);
					(*ppTargetCollider)->Delete_listCollision(this);
					break;
				}
			}

			return false;
		}
	}

	//If List have same Collider, Do return.
	list<const CCollider*>::iterator iter = m_listCollision.begin();
	for (; iter != m_listCollision.end(); ++iter)
	{
		if (*iter == (*ppTargetCollider))
			return true;
	}

	m_listCollision.push_back(*ppTargetCollider);

	if(true == isAddTargetList)
		(*ppTargetCollider)->Add_listCollision(this);

	return true;
}

CCollider * CCollider::Create(LPDIRECT3DDEVICE9 pGraphic_Device, CShader* pShader)
{
	CCollider*		pInstance = new CCollider(pGraphic_Device);

	if (FAILED(pInstance->Ready_Collider(pShader)))
	{
		_MSG_BOX("CCollider Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CCollider::Clone(void)
{
	return Create(m_pGraphic_Device, m_pShaderCom);
}

void CCollider::Free(void)
{
	Safe_Release(m_pVB);
	Safe_Release(m_pIB);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRelevant_TransformCom);
	Safe_Release(m_pInput_Device);

	CComponent::Free();
}
