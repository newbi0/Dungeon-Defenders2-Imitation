#pragma once

typedef struct tagUnitInfo
{
	float			fMaxHp;
	float			fHp;
	float			fAtt;
	float			fDef;
}UNITINFO;

typedef struct tagStairInfo
{
	D3DXVECTOR3		vUpStairPos;
	D3DXVECTOR3		vDownStairPos;
	float			fUnitDistance;
	float			fCrystalDistance;
	float			fHightGap;
	float			fTotalCost;
}STAIRINFO;

typedef struct tagItemInfo
{
	const wchar_t*	pItemName;
	unsigned int	iItemKind;
	float			fItemStats;
}ITEMINFO;