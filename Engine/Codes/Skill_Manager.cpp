#include "Skill_Manager.h"
#include "Skill.h"

_USING(Engine)
_IMPLEMENT_SINGLETON(CSkill_Manager)

CSkill_Manager::CSkill_Manager(void)
{
}

CSkill * CSkill_Manager::Get_Skill(const _tchar * pTag)
{
	auto iter = find_if(m_mapSkill.begin(), m_mapSkill.end(), CTag_Finder(pTag));

	if (iter == m_mapSkill.end())
		return nullptr;

	return iter->second;
}

HRESULT CSkill_Manager::Add_Skill(const _tchar * pTag, CSkill * pSkill)
{
	if (nullptr == pSkill
		|| nullptr == pTag)
		return E_FAIL;

	auto iter = find_if(m_mapSkill.begin(), m_mapSkill.end(), CTag_Finder(pTag));

	if (iter != m_mapSkill.end())
		return E_FAIL;

	m_mapSkill.insert(MAPSKILL::value_type(pTag, pSkill));

	return NOERROR;
}

void CSkill_Manager::Free(void)
{
	for (auto& iter : m_mapSkill)
	{
		Safe_Release(iter.second);
	}
	m_mapSkill.clear();
}
