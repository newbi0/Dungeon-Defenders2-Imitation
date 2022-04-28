#pragma once
#include "Base.h"

_BEGIN(Engine)

class CSkill;
class _ENGINE_DLL CSkill_Manager final :
	public CBase
{
		_DECLARE_SINGLETON(CSkill_Manager);
public:
	explicit CSkill_Manager(void);
	virtual ~CSkill_Manager(void) = default;
public:
	CSkill* Get_Skill(const _tchar* pTag);
public:
	HRESULT Add_Skill(const _tchar* pTag, CSkill* pSkill);
private:
	unordered_map<const _tchar*, CSkill*>			m_mapSkill;
	typedef unordered_map<const _tchar*, CSkill*>	MAPSKILL;
private:
	virtual void Free(void);
};

_END