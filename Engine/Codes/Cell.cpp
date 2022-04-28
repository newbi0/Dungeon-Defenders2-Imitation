#include "Cell.h"
#include "Shader.h"
#include "Component_Manager.h"
#include "VIBuffer.h"

CCell::CCell(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	m_pGraphic_Device = pGraphic_Device;
	m_pGraphic_Device->AddRef();
}

const vector<_uint> CCell::Get_Neighbor_IndexVector(void)
{
	vector<_uint>	vecNeighborIdx;

	if (nullptr != m_pNeighbor[NEIGHBOR_AB])
		vecNeighborIdx.push_back(m_pNeighbor[NEIGHBOR_AB]->Get_Index());
	if (nullptr != m_pNeighbor[NEIGHBOR_BC])
		vecNeighborIdx.push_back(m_pNeighbor[NEIGHBOR_BC]->Get_Index());
	if (nullptr != m_pNeighbor[NEIGHBOR_CA])
		vecNeighborIdx.push_back(m_pNeighbor[NEIGHBOR_CA]->Get_Index());

	return vecNeighborIdx;
}

_vec3 CCell::Get_Close_Point(const _vec3 & vTargetPos, _float* pDistance)
{
	_uint	iResult = 0;
	for (_uint i = 0; i < POINT_END; ++i)
	{
		if (0 == i)
		{
			*pDistance = D3DXVec3Length(&(vTargetPos - m_vPoint[i]));
			iResult = i;
		}
		else
		{
			if (*pDistance > D3DXVec3Length(&(vTargetPos - m_vPoint[i])))
			{
				*pDistance = D3DXVec3Length(&(vTargetPos - m_vPoint[i]));
				iResult = i;
			}
		}
	}
	return m_vPoint[iResult];
}

void CCell::Set_Neighbor(NEIGHBOR eNeighbor, CCell * pCell, const _uint & iNeighborIdx)
{
	if (eNeighbor >= NEIGHBOR_END
		|| nullptr == pCell)
		return;
	m_pNeighbor[eNeighbor] = pCell;
	m_iNeighborIdx[eNeighbor] = iNeighborIdx;
}

CNavigation::EXISTSTATE CCell::Check_Exist_Cell(const _vec3 & vPos, _uint* iCurrentIdx, _vec3* vOutNormalVec)
{
	if (false == Check_Exist_Side(vPos, m_vPoint[POINT_A], m_vNormal[NORMAL_AB]))
	{
		if (nullptr == m_pNeighbor[NEIGHBOR_AB])
		{
			*vOutNormalVec = m_vNormal[NORMAL_AB];
			*iCurrentIdx = m_iIndex;
			return CNavigation::EXIST_OUT;
		}

		*iCurrentIdx = m_pNeighbor[NEIGHBOR_AB]->Get_Index();
		return CNavigation::EXIST_NEIGHBOR;
	}
	if (false == Check_Exist_Side(vPos, m_vPoint[POINT_B], m_vNormal[NORMAL_BC]))
	{
		if (nullptr == m_pNeighbor[NEIGHBOR_BC])
		{
			*vOutNormalVec = m_vNormal[NORMAL_BC];
			*iCurrentIdx = m_iIndex;
			return CNavigation::EXIST_OUT;
		}

		*iCurrentIdx = m_pNeighbor[NEIGHBOR_BC]->Get_Index();
		return CNavigation::EXIST_NEIGHBOR;
	}
	if (false == Check_Exist_Side(vPos, m_vPoint[POINT_C], m_vNormal[NORMAL_CA]))
	{
		if (nullptr == m_pNeighbor[NEIGHBOR_CA])
		{
			*vOutNormalVec = m_vNormal[NORMAL_CA];
			*iCurrentIdx = m_iIndex;
			return CNavigation::EXIST_OUT;
		}

		*iCurrentIdx = m_pNeighbor[NEIGHBOR_CA]->Get_Index();
		return CNavigation::EXIST_NEIGHBOR;
	}
	*iCurrentIdx = m_iIndex;
	return CNavigation::EXIST_IN;
}

_bool CCell::Check_Exist_Side(const _vec3 & vPos, const _vec3 & vStartPoint, const _vec3 & vNormal)
{
	_vec3	vTarget = vPos - vStartPoint;

	D3DXVec3Normalize(&vTarget, &vTarget);
	_float	fCos = D3DXVec3Dot(&vNormal, &vTarget);

	if (fCos < 0)
		return true;

	return false;
}

_bool CCell::Check_Equal_Point(const _vec3 & vPoint)
{
	for (_uint i = 0; i < POINT_END; ++i)
	{
		if (vPoint == m_vPoint[i])
			return true;
	}

	return false;
}

_bool CCell::Check_Near_NaviPoint(const _vec3 & vDestPt, _vec3* pPoint)
{
	for (_uint i = 0; i < POINT_END; ++i)
	{
		if (0.5f > D3DXVec3Length(&(m_vPoint[i] - vDestPt)))
		{
			*pPoint = m_vPoint[i];
			return true;
		}
	}
	return false;
}

NODE CCell::Check_Best_Point(const _vec3 & vParentPos, const _vec3 & vTargetPos)
{
	NODE	tNode;
	ZeroMemory(&tNode, sizeof(NODE));

	tNode.fPCost = D3DXVec3Length(&(vParentPos - m_vCenter));
	tNode.fTCost = D3DXVec3Length(&(vTargetPos - m_vCenter));
	tNode.fTotalCost = tNode.fPCost + tNode.fTCost;
	tNode.vPos = m_vCenter;

	return tNode;
}

void CCell::Render_Cell(LPD3DXEFFECT pEffect)
{
	if (nullptr == pEffect)
		return;

	_matrix		matWorld, matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView);
	m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj);

	pEffect->SetMatrix("g_matWorld", &matWorld);
	pEffect->SetMatrix("g_matView", &matView);
	pEffect->SetMatrix("g_matProj", &matProj);

	pEffect->Begin(nullptr, 0);

	if(ATTRIBUTE_PUBLIC == m_eAttribute)
		pEffect->BeginPass(0);
	else if(ATTRIBUTE_MONSTER == m_eAttribute)
		pEffect->BeginPass(1);

	if (FAILED(m_pGraphic_Device->SetStreamSource(0, m_pVB, 0, m_iSizeOfVertex)))
		return;
	if (FAILED(m_pGraphic_Device->SetFVF(m_dwVertexFVF)))
		return;
	if (FAILED(m_pGraphic_Device->SetIndices(m_pIB)))
		return;
	if (FAILED(m_pGraphic_Device->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_iNumVertex, 0, m_iNumPolygon)))
		return;

	pEffect->EndPass();
	pEffect->End();
}

HRESULT CCell::Ready_VBInfomation(const _vec3& vA, const _vec3& vB, const _vec3& vC)
{
	m_iNumVertex = 3;
	m_iSizeOfVertex = sizeof(VTXCOL);
	m_dwVertexFVF = D3DFVF_XYZ | D3DFVF_TEX0;

	m_iNumPolygon = 1;
	m_iSizeOfPolygonIdx = sizeof(INDEX16);
	m_IndexFormat = D3DFMT_INDEX16;

	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->CreateVertexBuffer(m_iSizeOfVertex * m_iNumVertex, 0, m_dwVertexFVF, D3DPOOL_MANAGED, &m_pVB, nullptr)))
		return E_FAIL;

	if (FAILED(m_pGraphic_Device->CreateIndexBuffer(m_iSizeOfPolygonIdx * m_iNumPolygon, 0, m_IndexFormat, D3DPOOL_MANAGED, &m_pIB, nullptr)))
		return E_FAIL;

	VTXCOL*		pVertex = nullptr;

	m_pVB->Lock(0, 0, (void**)&pVertex, 0);

	pVertex[0].vPosition = (_vec3)vA;
	pVertex[0].dwColor = 0;

	pVertex[1].vPosition = (_vec3)vB;
	pVertex[1].dwColor = 0;

	pVertex[2].vPosition = (_vec3)vC;
	pVertex[2].dwColor = 0;

	m_pVB->Unlock();

	INDEX16*	pIndex = nullptr;

	m_pIB->Lock(0, 0, (void**)&pIndex, 0);

	pIndex[0]._0 = 0;
	pIndex[0]._1 = 1;
	pIndex[0]._2 = 2;

	m_pIB->Unlock();

	return NOERROR;
}

CCell * CCell::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _vec3& vA, const _vec3& vB, const _vec3& vC, const _uint& iIndex, CShader* pShaderCom, const _uint& iAttribute)
{
	CCell*	pInstance = new CCell(pGraphic_Device);
	if (FAILED(pInstance->Ready_Cell(vA, vB, vC, iIndex, pShaderCom, iAttribute)))
	{
		_MSG_BOX("CCell Creating Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CCell::Ready_Cell(const _vec3& vA, const _vec3& vB, const _vec3& vC, const _uint& iIndex, CShader* pShaderCom, const _uint& iAttribute)
{
	m_iIndex = iIndex;
	m_eAttribute = (ATTRIBUTE)iAttribute;

	m_vPoint[POINT_A] = vA;
	m_vPoint[POINT_B] = vB;
	m_vPoint[POINT_C] = vC;

	_vec3	vPoint = vB - vA;
	m_vNormal[NORMAL_AB];
	D3DXVec3Normalize(&m_vNormal[NORMAL_AB], &_vec3(vPoint.z * -1.f, 0.f, vPoint.x));

	vPoint = vC - vB;
	m_vNormal[NORMAL_BC];
	D3DXVec3Normalize(&m_vNormal[NORMAL_BC], &_vec3(vPoint.z * -1.f, 0.f, vPoint.x));

	vPoint = vA - vC;
	m_vNormal[NORMAL_CA];
	D3DXVec3Normalize(&m_vNormal[NORMAL_CA], &_vec3(vPoint.z * -1.f, 0.f, vPoint.x));

	m_vCenter = (vA + vB + vC) / 3;
	
	for (_uint i = 0; i < NEIGHBOR_END; ++i)
		m_pNeighbor[i] = nullptr;

	if (FAILED(Ready_VBInfomation(vA, vB, vC)))
		return E_FAIL;

	m_pShaderCom = pShaderCom;
	m_pShaderCom->AddRef();

	return NOERROR;
}

void CCell::Free(void)
{
	Safe_Release(m_pShaderCom);

	Safe_Release(m_pGraphic_Device);
}