#include "stdafx.h"
#include "UI.h"

CUI::CUI(LPDIRECT3DDEVICE9 pGraphic_Device)
	: CGameObject(pGraphic_Device)
{
}

CUI::CUI(const CUI & rhs)
	: CGameObject(rhs)
{
}

void CUI::Set_SeeAble(_bool isSeeAble)
{
}

void CUI::Free(void)
{
	CGameObject::Free();
}
