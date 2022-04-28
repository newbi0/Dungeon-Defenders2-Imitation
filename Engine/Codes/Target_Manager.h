#pragma once

#include "Base.h"

_BEGIN(Engine)

class CTarget;
class _ENGINE_DLL CTarget_Manager : public CBase
{
	_DECLARE_SINGLETON(CTarget_Manager)
private:
	explicit CTarget_Manager();
	virtual ~CTarget_Manager() = default;
public:
	HRESULT Add_Target(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pTargetTag, const _ulong& dwSizeX, const _ulong& dwSizeY, D3DFORMAT Format, D3DXCOLOR Color);
	HRESULT Ready_DebugBuffer(const _tchar* pTargetTag, const _float& fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
	HRESULT Add_MRT(const _tchar* pMRTTag, const _tchar* pTargetTag);
	HRESULT Begin_MRT(const _tchar* pMRTTag);
	HRESULT End_MRT(const _tchar* pMRTTag);
	void Render_DebugBuffer(const _tchar* pMRTTag);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const _tchar* pTargetTag, const char* pConstantName);
	HRESULT SetUp_OnShader_Matrix(LPD3DXEFFECT pEffect, const _tchar* pTargetTag, const char* pConstantName, const _matrix& matValue);
private:
	map<const _tchar*, CTarget*>						m_mapTarget;
	typedef map<const _tchar*, CTarget*>				MAPTARGET;
private:
	map<const _tchar*, list<CTarget*>>					m_mapMRT;
	typedef map<const _tchar*, list<CTarget*>>			MAPMRT;

private:
	CTarget*	Find_Target(const _tchar* pTargetTag);
	list<CTarget*>*	Find_MRT(const _tchar* pMRTTag);
public:
	virtual void Free(void);

};

_END