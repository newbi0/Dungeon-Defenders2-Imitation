#include "Sound_Device.h"

_USING(Engine)
_IMPLEMENT_SINGLETON(CSound_Device)

CSound_Device::CSound_Device(void)
{
}

CSound_Device::~CSound_Device(void)
{
	for (size_t i = 0; i < m_vecSoundBuff.size(); ++i)
	{
		m_vecSoundBuff[i]->Release();
	}
	m_vecSoundBuff.clear();
	m_pSound->Release();

}

HRESULT CSound_Device::Init(HWND hWnd)
{
	m_ghWnd = hWnd;

	//���� ����̽� ����
	if(FAILED(DirectSoundCreate(NULL, &m_pSound, NULL)))
	{
		MessageBox(m_ghWnd, L"�������̽���������", L"SYSTEM ERROR", MB_OK);
		return E_FAIL;
	}

	//���� ����̽� �������� ����.
	if(FAILED(m_pSound->SetCooperativeLevel(m_ghWnd, DSSCL_PRIORITY)))
	{
		MessageBox(m_ghWnd, L"�������̽� �������� ����", L"SYSTEM ERROR", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

//���������� �б� �����Լ�(���)
HRESULT CSound_Device::LoadWave(TCHAR* pFileName)
{
	HMMIO	hFile;

	//CreateFile
	hFile = mmioOpen(pFileName, NULL, MMIO_READ);//wave������ ����.

	//��ũ����ü.
	MMCKINFO	pParent;
	memset(&pParent, 0, sizeof(pParent));
	pParent.fccType = mmioFOURCC('W','A','V','E');
	mmioDescend(hFile, &pParent, NULL, MMIO_FINDRIFF);

	MMCKINFO	pChild;
	memset(&pChild, 0, sizeof(pChild));
	pChild.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmioDescend(hFile, &pChild, &pParent, MMIO_FINDCHUNK);

	WAVEFORMATEX	wft;
	memset(&wft, 0, sizeof(wft));
	mmioRead(hFile, (char*)&wft, sizeof(wft));

	mmioAscend(hFile, &pChild, 0);
	pChild.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmioDescend(hFile, &pChild, &pParent, MMIO_FINDCHUNK);

	DSBUFFERDESC	BuffInfo;
	memset(&BuffInfo, 0, sizeof(DSBUFFERDESC));
	BuffInfo.dwBufferBytes = pChild.cksize;
	BuffInfo.dwSize = sizeof(DSBUFFERDESC);
	BuffInfo.dwFlags = DSBCAPS_STATIC;
	BuffInfo.lpwfxFormat = &wft;

	LPDIRECTSOUNDBUFFER		SoundBuff;

	if(FAILED(m_pSound->CreateSoundBuffer(&BuffInfo, &SoundBuff, NULL)))
	{
		MessageBox(m_ghWnd, L"������ۻ�������", L"", MB_OK);
		return E_FAIL;
	}

	void *pWrite1 = NULL, *pWrite2 = NULL;
	DWORD dwlength1, dwlength2;

	SoundBuff->Lock(0, pChild.cksize, &pWrite1, &dwlength1
		, &pWrite2, &dwlength2, 0);

	if(pWrite1 > 0)
		mmioRead(hFile, (char*)pWrite1, dwlength1);
	if(pWrite2 > 0)
		mmioRead(hFile, (char*)pWrite2, dwlength2);

	SoundBuff->Unlock(pWrite1, dwlength1, pWrite2, dwlength2);	

	mmioClose(hFile, 0);

	m_vecSoundBuff.push_back(SoundBuff);

	return S_OK;
}

void CSound_Device::SoundPlay(int iIndex, DWORD dwFlag)
{
	if(iIndex < 0 || iIndex > (signed)m_vecSoundBuff.size())
		return;

	m_vecSoundBuff[iIndex]->SetCurrentPosition(0);

	DWORD			dwFre;

	m_vecSoundBuff[iIndex]->GetFrequency(&dwFre);
	m_vecSoundBuff[iIndex]->Play(0, 0, dwFlag);
}

void CSound_Device::SoundStop(int iIndex)
{
	if(iIndex < 0 || iIndex > (signed)m_vecSoundBuff.size())
		return;

	m_vecSoundBuff[iIndex]->Stop();



}

bool CSound_Device::SoundPlaying(int iIndex)
{
	DWORD	dwStatus = 0;
	m_vecSoundBuff[iIndex]->GetStatus(&dwStatus);

	if(dwStatus & DSBSTATUS_PLAYING)
		return true;
	return false;	
}
