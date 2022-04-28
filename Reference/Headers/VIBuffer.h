#pragma once
#include "Component.h"

_BEGIN(Engine)

class _ENGINE_DLL CVIBuffer abstract :
	public CComponent
{
public:
	explicit CVIBuffer(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CVIBuffer(const CVIBuffer& rhs);
	virtual ~CVIBuffer(void) = default;
protected:
	virtual HRESULT Ready_VIBuffer(void);
	virtual void Render_VIBuffer(void) _PURE;
protected:
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;
	_uint						m_iNumVertex = 0;
	_uint						m_iSizeOfVertex = 0;
	_ulong						m_dwVertexFVF = 0;
protected:
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;
	_uint						m_iNumPolygon = 0;
	_uint						m_iSizeOfPolygonIdx = 0;
	D3DFORMAT					m_IndexFormat;
public:
	virtual CComponent* Clone(void) _PURE;
protected:
	virtual void Free(void);
};

_END