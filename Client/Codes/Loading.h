#pragma once
#include "Base.h"
#include "Defines.h"

namespace Engine
{
	class CComponent_Manager;
	class CObject_Manager;
}

class CLoading :
	public CBase
{
private:
	explicit CLoading(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CLoading(void) = default;
public:
	_bool Get_isLoadingFinish(void) {
		return m_isFinish;	}
	const _tchar* Get_String(void) {
		return m_szText;	}
	CRITICAL_SECTION* Get_Critical_Section(void) {
		return &m_CS;	}
public:
	HRESULT Ready_Loading(SCENEID eID);
	HRESULT Loading(void);
private:
	LPDIRECT3DDEVICE9		m_pGraphic_Device = nullptr;
private:
	HANDLE					m_hThread = 0;
	CRITICAL_SECTION		m_CS;
private:
	CComponent_Manager*		m_pComponent_Manager = nullptr;
	CObject_Manager*		m_pObject_Manager = nullptr;
private:
	SCENEID					m_eScene = SCENE_STATIC;
	_bool					m_isFinish = false;
	_tchar					m_szText[128] = L"";
private:
	HRESULT Ready_Stage_Component(void);
	HRESULT Ready_Stage_Mesh(void);
	HRESULT Ready_Stage_Shader(void);
	HRESULT Ready_Stage_Prototype_Object(void);
	HRESULT Ready_Stage_Layer(void);
	HRESULT Ready_Stage_Sound(void);
	HRESULT Ready_Stage_Load(void);
	HRESULT Ready_Stage_Turn(void);
private:
	HRESULT Create_MashObject(const _tchar* pNameTag);
	HRESULT Create_MashObject(const _tchar* pNameTag, const _uint& iCnt, const _uint& iStartCnt = 1);
	HRESULT Create_NoCullingObject(const _tchar* pNameTag);
	HRESULT Create_SmallObject(const _tchar* pNameTag);
	HRESULT Create_SmallObject(const _tchar* pNameTag, const _uint& iCnt, const _uint& iStartCnt = 1);
	HRESULT Create_SpakleRock(const _tchar* pNameTag, const _uint& iCnt, const _uint& iStartCnt = 1);
	HRESULT Create_PlaneObject(const _tchar* pNameTag);
	HRESULT Create_PlaneObject(const _tchar* pNameTag, const _uint& iCnt, const _uint& iStartCnt = 1);
	HRESULT Create_StaticMashComponent(const _tchar* pFilePath, const _tchar* pFileName, const _tchar* pComTag);
	HRESULT Create_StaticMashComponent(const _tchar* pFilePath, const _tchar* pFileName, const _tchar* pComTag, const _uint& iCnt, const _uint& iStartCnt = 1);
public:
	static CLoading* Create(LPDIRECT3DDEVICE9 pGraphic_Device, SCENEID eID);
private:
	virtual void Free(void);
};

