#include "Navigation.h"
#include "Cell.h"
#include "Picking.h"
#include "Component_Manager.h"
#include "Shader.h"

CNavigation::CNavigation(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CCell * CNavigation::Get_Cell(const _uint & iIndex) const
{
	if (iIndex >= m_vecCell.size())
		return nullptr;

	auto iter = m_vecCell.begin();

	return iter[iIndex];
}

_vec3 * CNavigation::Get_ZenPoint(void) const
{
	_uint	iIdx = 0;
	_vec3*	pZenPoint = &_vec3();
	for (auto& iter : m_vecCell)
	{
		if (CCell::ATTRIBUTE_MONSTER == iter->Get_Attribute())
		{
			_vec3	vA = iter->Get_Point(CCell::POINT_A);
			_vec3	vB = iter->Get_Point(CCell::POINT_B);
			_vec3	vC = iter->Get_Point(CCell::POINT_C);

			pZenPoint[iIdx] = (vA + vB + vC) / 3;
			++iIdx;
		}
	}

	return pZenPoint;
}

_bool CNavigation::Compute_Direction(const _vec3& vPos, const _vec3 & vSour, const _vec3 & vDest)
{
	_vec3	vSourN = vPos - vSour;
	_vec3	vDestN = vDest - vSour;
	vDestN = _vec3(vDestN.y * -1.f, vDestN.x, vDestN.z);
	D3DXVec3Normalize(&vSourN, &vSourN);
	D3DXVec3Normalize(&vDestN, &vDestN);
	_float	fCos = D3DXVec3Dot(&vSourN, &vDestN);
	if (0.f < fCos)
		return true;
	return false;
}

_uint CNavigation::Compute_CurrentIndex(const _vec3 & vPos, _uint * pCurrntIdx)
{
	if (nullptr == pCurrntIdx)
		return 0;
	_uint i = 0;
	_vec3	vNormal;
	for (auto& pCell : m_vecCell)
	{
		if (i == 70)
			int a = 0;
		if (EXIST_IN == pCell->Check_Exist_Cell(vPos, pCurrntIdx, &vNormal))
			return *pCurrntIdx;
		++i;
	}
	return *pCurrntIdx;
}

_vec3 CNavigation::Compute_MousePosOnCell(HWND hWnd)
{
	_vec3 vPos = _vec3(0.f, 0.f, 0.f);
	_matrix matIden;
	D3DXMatrixIdentity(&matIden);
	for (auto iter : m_vecCell)
	{
		if (true == m_pPickingCom->Compute_Picking_Cell(hWnd, iter->Get_Point(CCell::POINT_A), iter->Get_Point(CCell::POINT_B), iter->Get_Point(CCell::POINT_C), &matIden, &vPos))
			return vPos;
	}

	return vPos;
}

_vec3 CNavigation::Compute_CloseCellPos(const _vec3 & vTargetPos)
{
	_float	fDistance = 0.f, fResultDistance = 0.f;
	_vec3	vResult = _vec3(0.f, 0.f, 0.f);

	for (auto& pCell : m_vecCell)
	{
		if(pCell == (*m_vecCell.begin()))
			vResult = pCell->Get_Close_Point(vTargetPos, &fResultDistance);
		else
		{
			_vec3	vSour = pCell->Get_Close_Point(vTargetPos, &fDistance);
			if (fDistance < fResultDistance)
				vResult = vSour;
		}
	}

	return vResult;
}

_float CNavigation::Get_Hight(const _vec3& vPos, _uint * pCurrntIdx)
{
	if (*pCurrntIdx >= m_vecCell.size())
		return 0.f;

	_float t, u, v;
	_vec3	vNew = _vec3(vPos.x, vPos.y + 1.f, vPos.z);

	_vec3 vA = _vec3(m_vecCell[*pCurrntIdx]->Get_Point(CCell::POINT_A));
	_vec3 vB = _vec3(m_vecCell[*pCurrntIdx]->Get_Point(CCell::POINT_B));
	_vec3 vC = _vec3(m_vecCell[*pCurrntIdx]->Get_Point(CCell::POINT_C));

	if (TRUE == IntersectTriangle(vNew, _vec3(0.f, -1.f, 0.f), vB, vC, vA, &t, &u, &v))
	{
		m_fCurrentHihgt = (vNew + _vec3(0.f, -1.f, 0.f) * t).y;
		return m_fCurrentHihgt;
	}
	else
		int a = 0;
	//else
	//{
	//	Compute_CurrentIndex(vPos, pCurrntIdx);
	//	if (TRUE == IntersectTriangle(vNew, _vec3(0.f, -1.f, 0.f), vB, vC, vA, &t, &u, &v))
	//	{
	//		m_fCurrentHihgt = (vNew + _vec3(0.f, -1.f, 0.f) * t).y;
	//		return m_fCurrentHihgt;
	//	}
	//}


	return m_fCurrentHihgt;
}

const vector<_uint> CNavigation::Get_Neighbor_IndexVector(const _uint & iIndex) const
{
	return m_vecCell[iIndex]->Get_Neighbor_IndexVector();
}

BOOL CNavigation::IntersectTriangle(const D3DXVECTOR3& orig, const D3DXVECTOR3& dir, D3DXVECTOR3& v0, D3DXVECTOR3& v1, D3DXVECTOR3& v2, FLOAT* t, FLOAT* u, FLOAT* v)
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

void CNavigation::Set_Init(void)
{
	for (auto iter : m_vecCell)
	{
		iter->Set_Select(false);
	}
}

HRESULT CNavigation::Add_Cell(const _vec3 & vA, const _vec3 & vB, const _vec3 & vC, const _uint& iAttribute)
{
	_uint iIndx = m_vecCell.size();

	CCell*	pCell = CCell::Create(m_pGraphic_Device, vA, vB, vC, iIndx, m_pShaderCom, iAttribute);
	if (nullptr == pCell)
		return E_FAIL;

	m_vecCell.push_back(pCell);

	if(false == m_isLoad)
		Set_Neighbor();

	return NOERROR;
}

CNavigation::EXISTSTATE CNavigation::Check_Exist(const _vec3 & vPos, const _vec3 & vDir, const _float& fSpeed, _uint* iCurrntIdx, _vec3* vOutNormalVec)
{
	if (0 == m_vecCell.size()
		|| *iCurrntIdx > m_vecCell.size())
		return EXIST_NULL;
	
	_vec3 vDir_New;
	D3DXVec3Normalize(&vDir_New, &vDir);
	_vec3	vNextPos = vPos + vDir_New * fSpeed;
	EXISTSTATE	eResult = m_vecCell[*iCurrntIdx]->Check_Exist_Cell(vNextPos, iCurrntIdx, vOutNormalVec);

	if (*iCurrntIdx > m_vecCell.size())
		return EXIST_NULL;

	return eResult;
}

CNavigation::EXISTSTATE CNavigation::Check_Exist(const _vec3 & vPos, const _uint & iIndex)
{
	if (0 == m_vecCell.size())
		return EXIST_NULL;

	_vec3	vTmp;
	_uint	iTmp;
	return m_vecCell[iIndex]->Check_Exist_Cell(vPos, &iTmp, &vTmp);
}

_bool CNavigation::Check_Near_NaviPoint(const _vec3 & vDestPt, _vec3 * pPoint)
{
	for (auto iter : m_vecCell)
	{
		if (true == iter->Check_Near_NaviPoint(vDestPt, pPoint))
			return true;
	}

	return false;
}

NODE CNavigation::Check_Best_Point(const _vec3& vParentPos, const _vec3& vTargetPos, const _uint& iIndex)
{
	NODE tNode = m_vecCell[iIndex]->Check_Best_Point(vParentPos, vTargetPos);
	tNode.iIndex = iIndex;

	return tNode;
}

_bool CNavigation::Check_Cursor_InCell(HWND hWnd)
{
	_matrix matIden;
	D3DXMatrixIdentity(&matIden);
	for (auto iter : m_vecCell)
	{
		if (true == m_pPickingCom->Compute_Picking_Cell(hWnd, iter->Get_Point(CCell::POINT_A), iter->Get_Point(CCell::POINT_B), iter->Get_Point(CCell::POINT_C), &matIden))
		{
			iter->Set_Select(!(iter->Get_isSelected()));
			return true;
		}
	}

	return false;
}

_uint CNavigation::Delete_Selected_Cell(void)
{
	_uint iIndex = 0;
	vector<CCell*>::iterator	iter = m_vecCell.begin();
	for (; ;)
	{
		if (m_vecCell.end() == m_vecCell.begin() + iIndex)
			break;
		if (true == m_vecCell[iIndex]->Get_isSelected())
		{
			Safe_Release(m_vecCell[iIndex]);
			m_vecCell.begin() + iIndex = m_vecCell.erase(m_vecCell.begin() + iIndex);
			return iIndex;
		}
		else
			++iIndex;
	}

	return iIndex;
}

void CNavigation::Save_Cell(const _tchar* pFileName)
{
	HANDLE		hFile;

	hFile = CreateFile(pFileName, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return;

	_ulong	dwByte = 0;

	_vec3	vPoint[3];

	for (auto iter : m_vecCell)
	{
		vPoint[0] = iter->Get_Point(CCell::POINT_A);
		vPoint[1] = iter->Get_Point(CCell::POINT_B);
		vPoint[2] = iter->Get_Point(CCell::POINT_C);

		WriteFile(hFile, vPoint, sizeof(_vec3) * 3, &dwByte, 0);
	}

	CloseHandle(hFile);
}

void CNavigation::Load_Cell(const _tchar* pFileName)
{
	m_isLoad = true;
	for (auto iter : m_vecCell)
		Safe_Release(iter);
	m_vecCell.clear();

	HANDLE		hFile;

	hFile = CreateFile(pFileName, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return;
	
	_ulong	dwByte = 0;

	for (;;)
	{
		NAVIMASHINFO	tInfo;

		ReadFile(hFile, &tInfo, sizeof(NAVIMASHINFO), &dwByte, 0);
		if (0 == dwByte)
			break;

		if (FAILED(Add_Cell(tInfo.vA, tInfo.vB, tInfo.vC, tInfo.iAttribute)))
			return;
	}

	Set_Neighbor();

	CloseHandle(hFile);
	m_isLoad = false;
}

void CNavigation::Render_Navigation(void)
{
	LPD3DXEFFECT pEffect = m_pShaderCom->Get_EffectHandle();
	if (nullptr == pEffect)
		return;

	_matrix		matWorld, matView, matProj;
	D3DXMatrixIdentity(&matWorld);
	if (FAILED(m_pGraphic_Device->GetTransform(D3DTS_VIEW, &matView)))
		return;
	if (FAILED(m_pGraphic_Device->GetTransform(D3DTS_PROJECTION, &matProj)))
		return;

	if (FAILED(pEffect->SetMatrix("g_matWorld", &matWorld)))
		return;
	if (FAILED(pEffect->SetMatrix("g_matView", &matView)))
		return;
	if (FAILED(pEffect->SetMatrix("g_matProj", &matProj)))
		return;

	for (auto iter : m_vecCell)
	{
		iter->Render_Cell(pEffect);
	}
}

void CNavigation::Set_Neighbor(void)
{
	_uint iSize = (_uint)m_vecCell.size();
	for (_uint i = 0; i < iSize; ++i)
	{
		for (_uint j = 0; j < iSize; ++j)
		{
			if (i == j)
				continue;

			if (true == m_vecCell[j]->Check_Equal_Point(m_vecCell[i]->Get_Point(CCell::POINT_A))
				&& true == m_vecCell[j]->Check_Equal_Point(m_vecCell[i]->Get_Point(CCell::POINT_B)))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_AB, m_vecCell[j], j - 1);
				continue;
			}
			
			if (true == m_vecCell[j]->Check_Equal_Point(m_vecCell[i]->Get_Point(CCell::POINT_B))
				&& true == m_vecCell[j]->Check_Equal_Point(m_vecCell[i]->Get_Point(CCell::POINT_C)))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_BC, m_vecCell[j], j - 1);
				continue;
			}

			if (true == m_vecCell[j]->Check_Equal_Point(m_vecCell[i]->Get_Point(CCell::POINT_C))
				&& true == m_vecCell[j]->Check_Equal_Point(m_vecCell[i]->Get_Point(CCell::POINT_A)))
			{
				m_vecCell[i]->Set_Neighbor(CCell::NEIGHBOR_CA, m_vecCell[j], j - 1);
				continue;
			}
		}
	}
}

CComponent * CNavigation::Clone(void)
{
	AddRef();
	return this;
}

CNavigation * CNavigation::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _uint& iSceneID, const _tchar* pPickingCom, const _tchar* pShaderCom)
{
	CNavigation*	pInstance = new CNavigation(pGraphic_Device);
	if (FAILED(pInstance->Ready_Navigation(iSceneID, pPickingCom, pShaderCom)))
	{
		_MSG_BOX("CNavigation Creating Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CNavigation::Ready_Navigation(const _uint& iSceneID, const _tchar* pPickingCom, const _tchar* pShaderCom)
{
	m_pPickingCom = (CPicking*)CComponent_Manager::Get_Instance()->Clone_Component(iSceneID, pPickingCom);
	if (nullptr == m_pPickingCom)
		return E_FAIL;

	m_pShaderCom = (CShader*)CComponent_Manager::Get_Instance()->Clone_Component(iSceneID, pShaderCom);
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	return NOERROR;
}

void CNavigation::Free(void)
{
	for (auto iter : m_vecCell)
		Safe_Release(iter);
	m_vecCell.clear();

	Safe_Release(m_pPickingCom);
	Safe_Release(m_pShaderCom);

	CComponent::Free();
}