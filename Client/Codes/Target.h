#pragma once

#include "Base.h"

_BEGIN(Engine)

class _ENGINE_DLL CTarget : public CBase
{
private:
	xplicit CTarget(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual ~CTarget(void) = default;
public:
	HREUSLT Ready_Target(const _ulong& dwSizeX, const _ulong& dwSizeY, D3DFORMAT Format, D3DXCOLOR Color);
	HREUSLT Ready_DebugTarget(const _float&fX, const _float& fY, const _float& fSizeX, const _float& fSizeY);
	HRESULT SetUp_OnGraphicDevice(const _ulong& dwIndex);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName);
	HRESULT Release_OnGraphicDevice(const _ulong& dwIndex);
	HRESULT Clear_Target();
	void Render_DebugBuffer(void);
private:
	LPDIRECT3DDEVICE9			m_pGraphic_Device = nullptr;
	LPDIRECT3DTEXTURE9			m_pTarget_Texture = nullptr;
	LPDIRECT3DSURFACE9			m_pTarget_Surface = nullptr;
	LPDIRECT3DSURFACE9			m_pOld_Surface = nullptr;
private:
	D3DXCOLOR					m_ClearColor;
private:
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;
public:
	static CTarget* Create(LPDIRECT3DDEVICE9 pGraphic_Deivce, const _ulong& dwSizeX, const _ulong& dwSizeY, D3DFORMAT Format, D3DXCOLOR Color);
	virtual void Free(void);
};

_END