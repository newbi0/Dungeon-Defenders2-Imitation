#include "Graphic_Device.h"

_USING(Engine)
_IMPLEMENT_SINGLETON(CGraphic_Device)

CGraphic_Device::CGraphic_Device()
{

}


CGraphic_Device::~CGraphic_Device()
{

}

// 장치를 조사 및 ㅈ생성.
HRESULT CGraphic_Device::Ready_GraphicDev(HWND hWnd, BOOL isWindowed, const _ulong& dwBackBufferCX, const _ulong& dwBackBufferCY, LPDIRECT3DDEVICE9* ppGraphicDev)
{
	// 장치를 조사하기위한 객체를 생성한다.
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);

	// 장치의 성능을 조사한다.
	D3DCAPS9		DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	if(FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps))) // 장치의 성능정보를 가지고 옵니다.
	{ 
		_MSG_BOX("장치정보가져오기실패");
		return E_FAIL;
	}

	// 장치의 정보를 바탕으로 장치의 동작방식을 설정한다.
	_ulong		BehaviorFlag = 0;

	//하드웨어의 Vertex 프로세싱 모드 사용이 가능한가?
	if (DeviceCaps.Caps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		BehaviorFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
		BehaviorFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	BehaviorFlag |= D3DCREATE_MULTITHREADED;

	// 내가 장치를 어떻게 초기화할지. (백버퍼를 어떻게 생성할지, 더블버퍼링을 어덯게 수행할지)
	D3DPRESENT_PARAMETERS			d3dpp;
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	d3dpp.BackBufferWidth = dwBackBufferCX;
	d3dpp.BackBufferHeight = dwBackBufferCY;
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount = 1;

	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hWnd;

	d3dpp.Windowed = isWindowed;

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;

	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// 장치를 생성한다.
	if (FAILED(m_pSDK->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, BehaviorFlag, &d3dpp, &m_pGraphicDev)))
	{
		_MSG_BOX("GraphicDevice Created Failed");
		return E_FAIL;
	}

	*ppGraphicDev = m_pGraphicDev;

	m_pGraphicDev->AddRef();

	return NOERROR;
}

void CGraphic_Device::Free(void)
{
	_ulong dwRefCnt = 0;

	if (dwRefCnt = Safe_Release(m_pGraphicDev))
		_MSG_BOX("m_pGraphicDev Release Failed");	
	
	if (Safe_Release(m_pSDK))	
		_MSG_BOX("m_pSDK Release Failed");	
}
