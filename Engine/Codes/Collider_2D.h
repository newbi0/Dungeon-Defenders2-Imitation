#pragma once
#include "Component.h"

_BEGIN(Engine)

class CShader;
class _ENGINE_DLL CCollider_2D final : public CComponent
{
protected:
	explicit CCollider_2D(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCollider_2D(const CCollider_2D& rhs);
	virtual ~CCollider_2D() = default;
public:
	HRESULT Ready_Collider(CShader* pShader);
public:
	void Render_Collider(void);
private:
	CShader*					m_pShaderCom = nullptr;
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;
	_matrix						m_matWorld;
public:
	static CCollider_2D* Create(LPDIRECT3DDEVICE9 pGraphic_Device, CShader* pShader);
	virtual CCollider_2D* Clone(void);
private:
	virtual void Free(void);
};

_END
