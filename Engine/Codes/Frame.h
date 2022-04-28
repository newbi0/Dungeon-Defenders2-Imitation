#pragma once

#include "Engine_Defines.h"
#include "Base.h"

_BEGIN(Engine)

// 내가 설정한 시간마다 true를 리턴하는 함수를 가진 클래스다.
class CFrame :
	public CBase
{
private:
	explicit CFrame();
	virtual ~CFrame();
public:
	HRESULT Ready_Frame(const _float& fRenderCall);
	_bool Permit_Call(const _float& fTimeDelta);
private:	
	_float			m_fRenderCallPerSec = 0; // 트루를 리턴해야할 시간
	_float			m_fTimeAcc; // 시간을 누적시켜놓은 변수
public:
	static CFrame* Create(const _float& fRenderCall);
private:
	virtual void Free(void);
};

_END