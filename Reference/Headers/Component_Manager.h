#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Renderer.h"
#include "Rect_Texture.h"


_BEGIN(Engine)

class _ENGINE_DLL CComponent_Manager : public CBase
{
	_DECLARE_SINGLETON(CComponent_Manager)
private:
	explicit CComponent_Manager();
	virtual ~CComponent_Manager() = default;
public:
	HRESULT Reserve_Container_Size(const _uint& iMaxSize);
	HRESULT Add_Component(const _uint& iSceneIdx, const _tchar* pComponentTag, CComponent* pComponent);
	CComponent* Clone_Component(const _uint& iSceneIdx, const _tchar* pComponentTag);
	HRESULT Clear_Component(const _uint& iSceneIdx);
private:
	unordered_map<const _tchar*, CComponent*>*			m_pMapComponent = nullptr;
	typedef unordered_map<const _tchar*, CComponent*>	MAPCOMPONENT;
private:
	vector<const _tchar*>*								m_pVecTag;
	typedef vector<const _tchar*>						VECTAG;
private:
	_uint												m_iMaxSize = 0;
public:
	virtual void Free(void);
};

_END