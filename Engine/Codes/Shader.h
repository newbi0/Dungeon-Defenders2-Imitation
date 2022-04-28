#pragma once

#include "Component.h"

_BEGIN(Engine)

class _ENGINE_DLL CShader : public CComponent
{
private:
	explicit CShader(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CShader(const CShader& rhs);
	virtual ~CShader() = default;
public:
	LPD3DXEFFECT Get_EffectHandle(void) const {
		return m_pEffect; }
public:
	HRESULT Ready_Shader(const _tchar* pShaderPath);
private:
	LPD3DXEFFECT			m_pEffect = nullptr; // 셰이더파일을 대표하는 객체.
	LPD3DXBUFFER			m_pBuffer = nullptr;
public:
	static CShader* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const _tchar* pShaderPath);
	virtual CComponent* Clone(void);
	virtual void Free(void);
};

_END