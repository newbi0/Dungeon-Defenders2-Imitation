#include "Texture.h"

_USING(Engine)

CTexture::CTexture(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CComponent(pGraphic_Device)
{
}

CTexture::CTexture(const CTexture & rhs)
	: CComponent(rhs)
{
	m_vecTextures = rhs.m_vecTextures;

	for (auto& pTexture : m_vecTextures)
		pTexture->AddRef();
	
}

HRESULT CTexture::Ready_Texture(TEXTURETYPE eTextureType, const _tchar * pTextureFilePath, const _uint & iNumTexture)
{
	IDirect3DBaseTexture9*				pTexture = nullptr;

	m_vecTextures.reserve(iNumTexture);

	for (size_t i = 0; i < iNumTexture; i++)
	{
		_tchar		szFullPath[MAX_PATH] = L"";

		wsprintf(szFullPath, pTextureFilePath, i);

		HRESULT				hr = 0;

		switch (eTextureType)
		{
		case TYPE_NORMAL:
			hr = D3DXCreateTextureFromFile(m_pGraphic_Device, szFullPath, (LPDIRECT3DTEXTURE9*)&pTexture);
			break;
		case TYPE_CUBEMAP:
			hr = D3DXCreateCubeTextureFromFile(m_pGraphic_Device, szFullPath, (LPDIRECT3DCUBETEXTURE9*)&pTexture);
			break;
		}
		if (FAILED(hr))
			return E_FAIL;

		m_vecTextures.push_back(pTexture);
	}
	
	return NOERROR;
}

HRESULT CTexture::SetUp_OnShader(LPD3DXEFFECT pEffect, const char * pConstantName, const _uint & iCnt)
{
	if (nullptr == pEffect)
		return E_FAIL;

	if (iCnt >= m_vecTextures.size())
		return E_FAIL;

	if (FAILED(pEffect->SetTexture(pConstantName, m_vecTextures[iCnt])))
		return E_FAIL;

	return NOERROR;
}

CTexture * CTexture::Create(LPDIRECT3DDEVICE9 pGraphic_Device, TEXTURETYPE eTextureType, const _tchar * pTextureFilePath, const _uint & iNumTexture)
{
	CTexture*		pInstance = new CTexture(pGraphic_Device);

	if (FAILED(pInstance->Ready_Texture(eTextureType, pTextureFilePath, iNumTexture)))
	{
		_MSG_BOX("CTexture Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTexture::Clone(void)
{
	return new CTexture(*this);
}

void CTexture::Free(void)
{
	for ( auto& pTexture : m_vecTextures)
	{
		Safe_Release(pTexture);
	}
	m_vecTextures.clear();

	Engine::CComponent::Free();
}
