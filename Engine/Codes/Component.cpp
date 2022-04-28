#include "Component.h"

_USING(Engine)

CComponent::CComponent(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
	, m_isClone(false)
{
	m_pGraphic_Device->AddRef();
}

CComponent::CComponent(const CComponent & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
	, m_isClone(true)
{
	m_pGraphic_Device->AddRef();
}


void CComponent::Free(void)
{
	Engine::Safe_Release(m_pGraphic_Device);
}
