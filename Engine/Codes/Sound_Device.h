#pragma once
#include "Base.h"

_BEGIN(Engine)

class _ENGINE_DLL CSound_Device : public CBase
{
	_DECLARE_SINGLETON(CSound_Device)
private:
	LPDIRECTSOUND				m_pSound;
	vector<LPDIRECTSOUNDBUFFER> m_vecSoundBuff;
public:
	HRESULT Init(HWND hWnd);
	HRESULT LoadWave(TCHAR* pFileName);
	void SoundPlay(int iIndex, DWORD dwFlag);
	void SoundStop(int iIndex);
	bool SoundPlaying(int iIndex);
private:
	HWND	m_ghWnd;
private:
	CSound_Device(void);
	~CSound_Device(void);
};

_END