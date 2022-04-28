
#include "Frame.h"

_USING(Engine)

CFrame::CFrame()
	: m_fRenderCallPerSec(0.f)
	, m_fTimeAcc(0.f)
{
}


CFrame::~CFrame()
{
}

HRESULT CFrame::Ready_Frame(const _float & fRenderCall)
{
	m_fRenderCallPerSec = 1.f / fRenderCall;
	
	return NOERROR;
}

// ���� ������ �ð����� true�� �����ϴ� �Լ��� ���� Ŭ������.
_bool CFrame::Permit_Call(const _float& fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (m_fRenderCallPerSec <= m_fTimeAcc)
	{
		m_fTimeAcc = 0.f;
		return true;
	}

	return false;
}

CFrame * CFrame::Create(const _float & fRenderCall)
{
	CFrame*		pInstance = new CFrame;
	
	if (FAILED(pInstance->Ready_Frame(fRenderCall)))
	{
		_MSG_BOX("CFrame Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CFrame::Free(void)
{
}
