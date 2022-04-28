#include "Shader.h"

_USING(Engine)

CShader::CShader(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
	, m_pEffect(nullptr)
{

}

CShader::CShader(const CShader & rhs)
	: CComponent(rhs)
	, m_pEffect(rhs.m_pEffect)
{
	m_pEffect->AddRef();
}

HRESULT CShader::Ready_Shader(const _tchar * pShaderPath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pGraphic_Device, pShaderPath, nullptr, nullptr, D3DXSHADER_DEBUG, nullptr, &m_pEffect, &m_pBuffer)))
		return E_FAIL;	

	return NOERROR;
}

CShader * CShader::Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar * pShaderPath)
{
	CShader*		pInstance = new CShader(pGraphic_Device);

	if (FAILED(pInstance->Ready_Shader(pShaderPath)))
	{
		_MSG_BOX("CShader Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CShader::Clone(void)
{
	return new CShader(*this);
}

void CShader::Free(void)
{
	Engine::Safe_Release(m_pEffect);

	Engine::CComponent::Free();
}
