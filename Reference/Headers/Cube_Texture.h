#pragma once
#include "VIBuffer.h"

_BEGIN(Engine)

class _ENGINE_DLL CCube_Texture :
	public CVIBuffer
{
public:
	explicit CCube_Texture(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCube_Texture(const CCube_Texture& rhs);
	virtual ~CCube_Texture(void) = default;
private:
	virtual HRESULT Ready_VIBuffer(void);
public:
	virtual void Render_VIBuffer(void);
	void Resize_Buffer(const _vec2& vMin, const _vec2& vMax);
public:
	static CCube_Texture* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void);
private:
	virtual void Free(void);
};

_END
