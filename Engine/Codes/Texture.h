#pragma once

#include "Component.h"

_BEGIN(Engine)

class _ENGINE_DLL CTexture : public CComponent
{
public:
	enum TEXTURETYPE { TYPE_NORMAL, TYPE_CUBEMAP, TYPE_END };
private:
	explicit CTexture(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;
public:
	virtual HRESULT Ready_Texture(TEXTURETYPE eTextureType, const _tchar* pTextureFilePath, const _uint& iNumTexture);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName, const _uint& iCnt = 0);
private:	
	vector<IDirect3DBaseTexture9*>				m_vecTextures;
	typedef vector<IDirect3DBaseTexture9*>		VECTEXTURE;
public:
	static CTexture* Create(LPDIRECT3DDEVICE9 pGraphic_Device, TEXTURETYPE eTextureType, const _tchar* pTextureFilePath, const _uint& iNumTexture = 1);
	virtual CComponent* Clone(void);
	virtual void Free(void);
};


_END