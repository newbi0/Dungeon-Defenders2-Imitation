#include "AnimationCtrl.h"

_USING(Engine)

CAnimationCtrl::CAnimationCtrl(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
	, m_pAniCtrl(nullptr)
	, m_iCurrentAniSetIndex(0)
	, m_dPeriod(0.0)
{
	m_pGraphic_Device->AddRef();
}

CAnimationCtrl::CAnimationCtrl(const CAnimationCtrl & rhs)
	: m_pGraphic_Device(rhs.m_pGraphic_Device)
	, m_iCurrentAniSetIndex(0)
	, m_dPeriod(rhs.m_dPeriod)
	, m_dAdjustedPeriod(rhs.m_dPeriod)
{
	rhs.m_pAniCtrl->CloneAnimationController(rhs.m_pAniCtrl->GetMaxNumAnimationOutputs(), rhs.m_pAniCtrl->GetMaxNumAnimationSets(), rhs.m_pAniCtrl->GetMaxNumTracks(), rhs.m_pAniCtrl->GetMaxNumEvents(), &m_pAniCtrl);

	m_pGraphic_Device->AddRef();

}

HRESULT CAnimationCtrl::Ready_AnimationCtrl(void)
{

	return NOERROR;
}

void CAnimationCtrl::SetUp_AnimationSet(const _uint & iIndex, const _float& fWeight)
{
	if (m_iCurrentAniSetIndex == iIndex)
		return;

 	m_iMaxNumAniSet = m_pAniCtrl->GetMaxNumAnimationSets();

	if (iIndex >= m_iMaxNumAniSet)
		return;

	//m_iCurrentTrack가 0 이 참이면 1 : 거짓이면 0
	m_iNewTrack = m_iCurrentTrack == 0 ? 1 : 0;

	LPD3DXANIMATIONSET		pAniSet = nullptr;

	m_pAniCtrl->GetAnimationSet(iIndex, &pAniSet);
	//m_pAniCtrl->GetAnimationSetByName();

	m_pAniCtrl->SetTrackAnimationSet(m_iNewTrack, pAniSet);

	m_dPeriod = pAniSet->GetPeriod();

	Safe_Release(pAniSet);

	m_pAniCtrl->UnkeyAllTrackEvents(m_iCurrentTrack);
	m_pAniCtrl->UnkeyAllTrackEvents(m_iNewTrack);

	m_pAniCtrl->SetTrackPosition(m_iNewTrack, 0.0);

	m_pAniCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fTimeAcc + 0.1);
	m_pAniCtrl->KeyTrackWeight(m_iCurrentTrack, 0.1f, m_fTimeAcc, 0.1, D3DXTRANSITION_LINEAR);
	m_pAniCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.0f, m_fTimeAcc, 0.1, D3DXTRANSITION_LINEAR);

	m_pAniCtrl->SetTrackEnable(m_iNewTrack, TRUE);
	m_pAniCtrl->KeyTrackWeight(m_iNewTrack, 0.9f, m_fTimeAcc, 0.1, D3DXTRANSITION_LINEAR);
	m_pAniCtrl->KeyTrackSpeed(m_iNewTrack, 1.0f, m_fTimeAcc, 0.1, D3DXTRANSITION_LINEAR);


	////0.25초 동안 전환 된 후 이전 프레임 비활성화됌.
	//m_pAniCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fTimeAcc + 0.25);
	//m_pAniCtrl->KeyTrackWeight(m_iCurrentTrack, fWeight, m_fTimeAcc, 0.25, D3DXTRANSITION_LINEAR);
	//m_pAniCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.0f, m_fTimeAcc, 0.25, D3DXTRANSITION_LINEAR);

	//m_pAniCtrl->SetTrackEnable(m_iNewTrack, TRUE);
	//m_pAniCtrl->KeyTrackWeight(m_iNewTrack, 1.f - fWeight, m_fTimeAcc, 0.25, D3DXTRANSITION_LINEAR);
	//m_pAniCtrl->KeyTrackSpeed(m_iNewTrack, 1.0f, m_fTimeAcc, 0.25, D3DXTRANSITION_LINEAR);

	m_iCurrentAniSetIndex = iIndex;

	m_pAniCtrl->ResetTime();

	m_fTimeAcc = 0.f;

	m_iCurrentTrack = m_iNewTrack;
}

void CAnimationCtrl::Play_Animation(const _float & fTimeDelta)
{
	m_pAniCtrl->AdvanceTime(fTimeDelta, nullptr);

	m_fTimeAcc += fTimeDelta;

}

_bool CAnimationCtrl::is_Finished(void)
{
	D3DXTRACK_DESC			Track_Desc;
	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &Track_Desc);

	if (Track_Desc.Position >= m_dPeriod - m_dAdjustedPeriod)
	{
		return true;
	}

	return false;
}

CAnimationCtrl * CAnimationCtrl::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
{
	CAnimationCtrl*		pInstance = new CAnimationCtrl(pGraphic_Device);

	if (FAILED(pInstance->Ready_AnimationCtrl()))
	{
		_MSG_BOX("CAnimationCtrl Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;

	return nullptr;
}

CAnimationCtrl * CAnimationCtrl::Clone(const CAnimationCtrl & rhs)
{
	return new CAnimationCtrl(rhs);
}

void CAnimationCtrl::Free(void)
{
	Safe_Release(m_pAniCtrl);
	Safe_Release(m_pGraphic_Device);
}

//#include "AnimationCtrl.h"
//
//_USING(Engine)
//
//CAnimationCtrl::CAnimationCtrl(LPDIRECT3DDEVICE9 pGraphic_Device)
//	: m_pGraphic_Device(pGraphic_Device)
//{
//	m_pGraphic_Device->AddRef();
//}
//
//CAnimationCtrl::CAnimationCtrl(const CAnimationCtrl & rhs)
//	: m_pGraphic_Device(rhs.m_pGraphic_Device)
//	, m_iCurrentAniSetIndex(0)
//	, m_dPeriod(rhs.m_dPeriod)
//{
//	rhs.m_pAniCtrl->CloneAnimationController(rhs.m_pAniCtrl->GetMaxNumAnimationOutputs(), rhs.m_pAniCtrl->GetMaxNumAnimationSets()
//	, rhs.m_pAniCtrl->GetMaxNumTracks(), rhs.m_pAniCtrl->GetMaxNumEvents(), &m_pAniCtrl);
//
//	m_pGraphic_Device->AddRef();
//}
//
//HRESULT CAnimationCtrl::Ready_AnimationCtrl(void)
//{
//	return NOERROR;
//}
//
//void CAnimationCtrl::SetUp_AnimationSet(const _uint & iIndex)
//{
//	if (nullptr == m_pAniCtrl)
//		return;
//
//	if (m_iCurrentAniSetIndex == iIndex)
//		return;
//
//	m_iMaxNumAniSet = m_pAniCtrl->GetMaxNumAnimationSets();
//	if (iIndex >= m_iMaxNumAniSet)
//		return;
//
//	if (0 == m_iCurrentTrack)
//		m_iNewTrack = 1;
//	else
//		m_iNewTrack = 0;
//
//	LPD3DXANIMATIONSET	pAniSet = nullptr;
//
//	m_pAniCtrl->GetAnimationSet(iIndex, &pAniSet);
//	m_pAniCtrl->SetTrackAnimationSet(m_iNewTrack, pAniSet);
//
//	m_dPeriod = pAniSet->GetPeriod();
//
//	Safe_Release(pAniSet);
//
//	//이벤트 초기화
// 	m_pAniCtrl->UnkeyAllTrackEvents(m_iCurrentTrack);
//	m_pAniCtrl->UnkeyAllTrackEvents(m_iNewTrack);
//
//	//보간
//	//0.25초 동안 전환 된 후 이전 프레임 비활성화됌.
//	m_pAniCtrl->KeyTrackEnable(m_iCurrentTrack, FALSE, m_fTimeAcc + 0.25);
//	m_pAniCtrl->KeyTrackWeight(m_iCurrentTrack, 0.1, m_fTimeAcc, 0.25, D3DXTRANSITION_LINEAR);
//	m_pAniCtrl->KeyTrackSpeed(m_iCurrentTrack, 1.0, m_fTimeAcc, 0.25, D3DXTRANSITION_LINEAR);
//
//	m_pAniCtrl->SetTrackEnable(m_iNewTrack, TRUE);
//	m_pAniCtrl->KeyTrackWeight(m_iNewTrack, 0.9, m_fTimeAcc, 0.25, D3DXTRANSITION_LINEAR);
//	m_pAniCtrl->KeyTrackSpeed(m_iNewTrack, 1.0, m_fTimeAcc, 0.25, D3DXTRANSITION_LINEAR);
//
//	//트랙시간초기화
//	m_pAniCtrl->SetTrackPosition(m_iNewTrack, 0.0);
//	//컨트롤러 내부 카운트 초기화
//	m_pAniCtrl->ResetTime();
//
//	m_iCurrentAniSetIndex = iIndex;
//	m_iCurrentTrack = m_iNewTrack;
//	m_fTimeAcc = 0.f;
//}
//
//void CAnimationCtrl::Play_Animation(const _float & fTimeDelta)
//{
//	if (nullptr == m_pAniCtrl)
//		return;
//
//	m_fTimeAcc += fTimeDelta;
//
//	if (FAILED(m_pAniCtrl->AdvanceTime(fTimeDelta, nullptr)))
//		return;
//}
//
//_bool CAnimationCtrl::is_Finished(void)
//{
//	D3DXTRACK_DESC	tTrack_Desc;
//	m_pAniCtrl->GetTrackDesc(m_iCurrentTrack, &tTrack_Desc);
//
//	//더 자연스럽게 0.01 정도 갭을 주자
//	if (tTrack_Desc.Position >= m_dPeriod - 0.01f)
//		return true;
//
//	return false;
//}
// 
//CAnimationCtrl * CAnimationCtrl::Create(LPDIRECT3DDEVICE9 pGraphic_Device)
//{
//	CAnimationCtrl*		pInstance = new CAnimationCtrl(pGraphic_Device);
//
//	if (FAILED(pInstance->Ready_AnimationCtrl()))
//	{
//		_MSG_BOX("CAnimationCtrl Created Failed");
//		Safe_Release(pInstance);
//	}
//	return pInstance;
//}
//
//CAnimationCtrl * CAnimationCtrl::Clone(const CAnimationCtrl & rhs)
//{
//	return new CAnimationCtrl(rhs);
//}
//
//void CAnimationCtrl::Free(void)
//{
//	Safe_Release(m_pAniCtrl);
//	Safe_Release(m_pGraphic_Device);
//}
