#pragma once

#include "Component.h"

_BEGIN(Engine)

class CTransform;
class _ENGINE_DLL CFrustum_Culling final : public CComponent
{
protected:
	explicit CFrustum_Culling(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CFrustum_Culling(const CFrustum_Culling& rhs);
	virtual ~CFrustum_Culling() = default;
public:
	void Set_StandardTransform(CTransform * pStandard);
public:
	_bool isIn_Frustum(const _vec3* pWorldPosition, const _float& fRadius);
	_bool isIn_Distance(const _vec3* pWorldPosition);
	static CFrustum_Culling* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void);
private:
	CTransform*	m_pStandard = nullptr;
	_vec3		m_vOriginal_Points[8];
private:
	HRESULT Ready_Frustum(void);
	virtual void Free(void);
};

_END