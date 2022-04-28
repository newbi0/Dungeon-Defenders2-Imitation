#pragma once
#include "VIBuffer.h"

_BEGIN(Engine)

class _ENGINE_DLL CRect_Texture :
	public CVIBuffer
{
public:
	explicit CRect_Texture(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CRect_Texture(const CRect_Texture& rhs);
	virtual ~CRect_Texture(void) = default;
private:
	virtual HRESULT Ready_VIBuffer(void);
public:
	virtual void Render_VIBuffer(void);
	void Resize_Buffer(const _vec2& vMin, const _vec2& vMax);
	void Resize_Buffer(const _vec3& vMin, const _vec3& vMax);
public:
	static CRect_Texture* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void);
private:
	virtual void Free(void);
};

_END
