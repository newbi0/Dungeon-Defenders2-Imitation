#include "Graphic_Device.h"

_USING(Engine)
_IMPLEMENT_SINGLETON(CGraphic_Device)

CGraphic_Device::CGraphic_Device()
{

}


CGraphic_Device::~CGraphic_Device()
{

}

// ��ġ�� ���� �� ������.
HRESULT CGraphic_Device::Ready_GraphicDev(HWND hWnd, BOOL isWindowed, const _ulong& dwBackBufferCX, const _ulong& dwBackBufferCY, LPDIRECT3DDEVICE9* ppGraphicDev)
{
	// ��ġ�� �����ϱ����� ��ü�� �����Ѵ�.
	m_pSDK = Direct3DCreate9(D3D_SDK_VERSION);

	// ��ġ�� ������ �����Ѵ�.
	D3DCAPS9		DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	if(FAILED(m_pSDK->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps))) // ��ġ�� ���������� ������ �ɴϴ�.
	{ 
		_MSG_BOX("��ġ���������������");
		return E_FAIL;
	}

	// ��ġ�� ������ �������� ��ġ�� ���۹���� �����Ѵ�.
	_ulong		BehaviorFlag = 0;

	//�ϵ������ Vertex ���μ��� ��� ����� �����Ѱ�?
	if (DeviceCaps.Caps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		BehaviorFlag |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
		BehaviorFlag |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	BehaviorFlag |= D3DCREATE_MULTITHREADED;

	// ���� ��ġ�� ��� �ʱ�ȭ����. (����۸� ��� ��������, ������۸��� ��F�� ��������)
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

	// ��ġ�� �����Ѵ�.
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
