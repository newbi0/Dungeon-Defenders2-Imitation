#pragma once

#include "Engine_Defines.h"
#include "Base.h"

// ��ġ�� �����Ѵ�.
// ��ġ�� ��ǥ�ϴ� ��ü�� �����ϳ�.
// ��ġ�� ��ǥ�ϴ� ��ü : �̹����� �׸���. ���� �����ϳ�, ���� ���ؼ� �ﰢ���� �׸���.

_BEGIN(Engine)

class _ENGINE_DLL CGraphic_Device : public CBase
{	
	_DECLARE_SINGLETON(CGraphic_Device)
public:
	enum WINMODE { MODE_FULL, MODE_WIN };
private:
	explicit CGraphic_Device();
	virtual ~CGraphic_Device();
public:
	HRESULT Ready_GraphicDev(HWND hWnd, BOOL isWindowed
		, const _ulong& dwBackBufferCX, const _ulong& dwBackBufferCY
		, LPDIRECT3DDEVICE9* ppGraphicDev);
private:
	LPDIRECT3D9				m_pSDK = nullptr;
	LPDIRECT3DDEVICE9		m_pGraphicDev = nullptr;
private:
	virtual void Free(void);
};

_END


