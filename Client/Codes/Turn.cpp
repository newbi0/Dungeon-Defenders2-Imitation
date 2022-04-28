#include "stdafx.h"
#include "Defines.h"
#include "Turn.h"
#include "Object_Manager.h"
#include "Camera_Follow.h"
#include "Player.h"
#include "Sound_Device.h"
#include "Unit.h"

_IMPLEMENT_SINGLETON(CTurn)

CTurn::CTurn()
{
}

HRESULT CTurn::Ready_Turn(const _vec3 * pZenPointArray, const _uint & iNumArray)
{
	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pSound_Device = CSound_Device::Get_Instance();

	m_iZenArrayNum = iNumArray;

	m_pZenPoint = new _vec3[iNumArray];

	memcpy(m_pZenPoint, pZenPointArray, sizeof(_vec3) * m_iZenArrayNum);

	return NOERROR;
}

_int CTurn::Update_Turn(_float fTimeDelta)
{
	if (2 == m_iNextTurn)
	{
		if (0 == m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_Goblin"))
		{
			if (TURN_BUILD == m_eCurrentTurn)
				return _RUN;
			m_eCurrentTurn = TURN_BUILD;

			CPlayer*	pPlayer = (CPlayer*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
			if (nullptr == pPlayer)
				return E_FAIL;
			pPlayer->Set_Animation(CPlayer::Ani_WaveWave);
			//Ani_WaveWave -> WinWave¿ÀÅ¸

			//m_pSound_Device->SoundStop(2);
			//m_pSound_Device->SoundPlay(1, 0);
		}
	}

	if (3 == m_iNextTurn)
	{
		if (0 == m_pObject_Manager->Get_GameObjectNum(SCENE_STAGE, L"Layer_DemonLord"))
		{
			//½Â¸®
			if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Sign_Win", L"Object_Sign_Win", &_vec3(0.f, 0.f, 0.f))))
				return E_FAIL;

			CPlayer*	pPlayer = (CPlayer*)(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
			if (nullptr == pPlayer)
				return E_FAIL;
			pPlayer->Set_Animation(CPlayer::Ani_Win);

			m_eCurrentTurn = TURN_WIN;
			m_pSound_Device->SoundStop(4);
			m_pSound_Device->SoundPlay(26, 0);
			m_iNextTurn = 0;
		}
	}

	return _RUN;
}

HRESULT CTurn::Start_Wave(void)
{
	m_eCurrentTurn = TURN_WAVE;
	if (1 == m_iNextTurn)
	{
		if (FAILED(Start_Wave_1()))
			return E_FAIL;
	}
	else if (2 == m_iNextTurn)
	{
		if (FAILED(Start_Wave_Boss()))
			return E_FAIL;
	}

	return NOERROR;
}

HRESULT CTurn::Start_Wave_1(void)
{
	for (_uint i = 0; i < m_iZenArrayNum; ++i)
	{
		m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Goblin", L"Object_Goblin", &m_pZenPoint[i]);
	}

	m_pSound_Device->SoundStop(1);
	m_pSound_Device->SoundPlay(2, 0);

	m_iNextTurn = 2;

	return NOERROR;
}

HRESULT CTurn::Start_Wave_Boss(void)
{
	//m_pSound_Device->SoundStop(1);
	//³¯°³Áþ¼Ò¸®
	m_pSound_Device->SoundPlay(5, 0);

	m_iNextTurn = 3;

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_DemonLord", L"Object_DemonLord")))
		return E_FAIL;

	CCamera_Follow*	pCamera = (CCamera_Follow*)m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera");
	if (nullptr == pCamera)
		return E_FAIL;

	const CUnit*	pBoss = dynamic_cast<CUnit*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_DemonLord"));
	if (nullptr == pBoss)
		return E_FAIL;
	pCamera->Set_MouseRot(false);
	pCamera->Set_LookDirction(true);
	pCamera->Set_Target(pBoss, _vec3(0.f, 20.f, 12.f), 5.5 * 3.f);

	return NOERROR;
}

void CTurn::Free(void)
{
	Safe_Delete_Array(m_pZenPoint);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pSound_Device);
}

