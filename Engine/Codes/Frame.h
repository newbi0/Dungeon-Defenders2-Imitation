#pragma once

#include "Engine_Defines.h"
#include "Base.h"

_BEGIN(Engine)

// ���� ������ �ð����� true�� �����ϴ� �Լ��� ���� Ŭ������.
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
	_float			m_fRenderCallPerSec = 0; // Ʈ�縦 �����ؾ��� �ð�
	_float			m_fTimeAcc; // �ð��� �������ѳ��� ����
public:
	static CFrame* Create(const _float& fRenderCall);
private:
	virtual void Free(void);
};

_END