#include "AStar.h"
#include "Navigation.h"

const _uint LOOP_MAX_NUM = 100;

CAStar::CAStar(LPDIRECT3DDEVICE9 pGraphic_Device, CNavigation* pNaviCom)
	:CComponent(pGraphic_Device)
{
	m_pNavigationCom = pNaviCom;
	m_pNavigationCom->AddRef();
}

CAStar::CAStar(const CAStar & rhs)
	:CComponent(rhs)
{
	m_pNavigationCom = rhs.m_pNavigationCom;
	m_pNavigationCom->AddRef();
}

NODE CAStar::Compute_Cost(const _vec3& vStartPos, const _vec3& vEndPos)
{
	NODE tNode;
	ZeroMemory(&tNode, sizeof(NODE));

	list<NODE> listNode;
	for (auto iter : m_listOpen)
	{
		tNode = m_pNavigationCom->Check_Best_Point(vStartPos, vEndPos, iter);
		listNode.push_back(tNode);
	}

	_vec3	vBestPoint = _vec3(0.f, 0.f, 0.f);
	listNode.sort([](NODE tSour, NODE tDest) -> _bool { return tSour.fTotalCost < tDest.fTotalCost; });

	return *(listNode.begin());
}

list<_vec3> CAStar::Make_Rute(const _uint& iStartIdx, const _vec3& vStartPos, const _vec3& vEndPos)
{
	//Initialize
	m_listOpen.clear();
	m_listClose.clear();
	m_listBest.clear();

	m_listClose.push_back(iStartIdx);

	for (_uint i = 0; i < LOOP_MAX_NUM; ++i)
	{
		//If the last Index added is at the goal position, Do best list return.
		_uint	iLastIdx = *(--m_listClose.end());
		if (CNavigation::EXIST_IN == m_pNavigationCom->Check_Exist(vEndPos, iLastIdx))
		{
			m_listBest.push_back(vEndPos);
			return m_listBest;
		}

		//The neighbor Indexes of the last Index added.
		vector<_uint>	vecNeighborIdx = m_pNavigationCom->Get_Neighbor_IndexVector(iLastIdx);

		for (_uint i = 0; i < vecNeighborIdx.size(); ++i)
		{
			//If the neighbor Index isn't at the close list, Do Add in the open list.
			_bool	isOver = false;
			
			for (auto& iClose : m_listClose)
			{
				if (iClose == vecNeighborIdx[i])
				{
					isOver = true;
					break;
				}
			}
			if (false == isOver)
				m_listOpen.push_back(vecNeighborIdx[i]);
		}

		//If open list is empty, Do return;
		if (true == m_listOpen.empty())
		{
			m_listBest.clear();
			return m_listBest;
		}

		//Find the node had best cost.
		NODE* pNode = new NODE;
		if (false == m_listBest.empty())
		{
			_vec3	vParentPos = *(--m_listBest.end());
			*pNode = Compute_Cost(vParentPos, vEndPos);
		}
		else
			*pNode = Compute_Cost(vStartPos, vEndPos);

		m_listClose.push_back(pNode->iIndex);
		m_listBest.push_back(pNode->vPos);
		m_listOpen.clear();
	}

	return m_listBest;
}

CComponent * CAStar::Clone(void)
{
	return new CAStar(*this);
}

CAStar * CAStar::Create(LPDIRECT3DDEVICE9 pGraphic_Device, CNavigation* pNaviCom)
{
	CAStar*		pInstance = new CAStar(pGraphic_Device, pNaviCom);

	if (FAILED(pInstance->Ready_Astar()))
	{
		_MSG_BOX("CAStar Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

HRESULT CAStar::Ready_Astar(void)
{
	return NOERROR;
}

void CAStar::Free(void)
{
	Safe_Release(m_pNavigationCom);
	CComponent::Free();
}
