#pragma once
#include "VIBuffer.h"

_BEGIN(Engine)

class _ENGINE_DLL CTerrain_Buffer :
	public CVIBuffer
{
public:
	explicit CTerrain_Buffer(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTerrain_Buffer(const CTerrain_Buffer& rhs);
	virtual ~CTerrain_Buffer(void) = default;
private:
	virtual HRESULT Ready_VIBuffer(void);
public:
	virtual void Render_VIBuffer(void);
public:
	static CTerrain_Buffer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void);
private:
	virtual void Free(void);
};

_END
