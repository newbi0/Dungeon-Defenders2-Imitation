#include "stdafx.h"
#include "Loading.h"

#include "Component_Manager.h"
#include "Object_Manager.h"
#include "Sound_Device.h"

#include "Mesh_Static.h"
#include "Mesh_Dynamic.h"
#include "Shader.h"

#include "Camera_Free.h"
#include "Camera_Follow.h"
#include "Static_Object.h"
#include "NoCulling_Object.h"
#include "Small_Object.h"
#include "Light.h"
#include "LightMove.h"
#include "PlaneObject.h"
#include "SpakleRock.h"
#include "Door_Left.h"
#include "Door_Right.h"
#include "MainCrystal.h"
#include "MainCrystal_Support.h"
#include "Player.h"
#include "Sword.h"
#include "Goblin.h"
#include "DemonLord.h"
#include "SpikyTower.h"
#include "FireBall.h"
#include "BossSkillAura.h"
#include "BossBackAura.h"
#include "HitZone.h"
#include "BowlingBall_Fragment.h"
#include "Tornado.h"
#include "Thunderbolt.h"
#include "Blood.h"
#include "HealParticle.h"
#include "PowerUpAura.h"
#include "HpBarBG_Player.h"
#include "HpBar_Player.h"
#include "HpBarBG_Enemy.h"
#include "HpBar_Enemy.h"
#include "HpBar_Boss.h"
#include "HpBarBG_Boss.h"
#include "HpBarFR_Boss.h"
#include "SkillBarBG_Boss.h"
#include "SkillBar_Boss.h"
#include "Font_Damage.h"
#include "ItemIcon.h"
#include "Inventory.h"
#include "Inventory_Blank.h"
#include "Inventory_Button.h"
#include "Sign_Check.h"
#include "Sign_Win.h"
#include "Profile.h"
#include "Wind.h"
#include "Cursor.h"

#include "PlayerInfoBar.h"
#include "SkillButton.h"

#include "Navigation.h"
#include "Frustum_Culling.h"
#include "Texture.h"
#include "Collider.h"
#include "Collider_2D.h"
#include "Turn.h"

#include "Terrain.h"
#include "Terrain_Buffer.h"
#include "Sky.h"
#include "Cube_Texture.h"

CLoading::CLoading(LPDIRECT3DDEVICE9 pGraphic_Device)
	: m_pGraphic_Device(pGraphic_Device)
{
	m_pGraphic_Device->AddRef();
}

_uint APIENTRY Thread_Main(void* pArg)
{
	CLoading*	pLoading = (CLoading*)pArg;
	if (nullptr == pLoading)
		return E_FAIL;

	CRITICAL_SECTION* pSection = pLoading->Get_Critical_Section();
	if (nullptr == pSection)
		return E_FAIL;

	EnterCriticalSection(pSection);

	pLoading->Loading();

	LeaveCriticalSection(pSection);

	return _RUN;
}

HRESULT CLoading::Ready_Loading(SCENEID eID)
{
	m_pComponent_Manager = CComponent_Manager::Get_Instance();
	m_pComponent_Manager->AddRef();

	m_pObject_Manager = CObject_Manager::Get_Instance();
	m_pObject_Manager->AddRef();

	m_eScene = eID;

	InitializeCriticalSection(&m_CS);
	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, Thread_Main, this, 0, nullptr);

	return NOERROR;
}

HRESULT CLoading::Loading(void)
{
	switch (m_eScene)
	{
	case SCENE_STAGE:
	{
		if (FAILED(Ready_Stage_Component())
			|| Ready_Stage_Mesh()
			|| Ready_Stage_Shader()
			|| Ready_Stage_Prototype_Object()
			|| Ready_Stage_Load()
			|| Ready_Stage_Layer()
			|| Ready_Stage_Sound()
			|| Ready_Stage_Turn())
			return E_FAIL;
	}
	default:
		break;
	}

	lstrcpy(m_szText, L"Client");
	m_isFinish = true;

	return NOERROR;
}

HRESULT CLoading::Ready_Stage_Component(void)
{
	CComponent*	pComponent = nullptr;

	pComponent = CCube_Texture::Create(m_pGraphic_Device);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Cube_Texture", pComponent)))
		return E_FAIL;

	pComponent = CTerrain_Buffer::Create(m_pGraphic_Device);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Terrain_Buffer", pComponent)))
		return E_FAIL;

	pComponent = CFrustum_Culling::Create(m_pGraphic_Device);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Frustum_Culling", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Earth_COLOR_Rock.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Rock", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Dirt_COLOR.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Terrain", pComponent)))
		return E_FAIL;

	CShader*	pShader = (CShader*)(m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Shader_Color"));
	pComponent = CCollider::Create(m_pGraphic_Device, pShader);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Collider", pComponent)))
		return E_FAIL;

	pComponent = CCollider_2D::Create(m_pGraphic_Device, pShader);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Collider2D", pComponent)))
		return E_FAIL;

	Safe_Release(pShader);

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_CUBEMAP, L"../Bin/Resource/Texture/Stage/Sky.dds");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Sky", pComponent)))
		return E_FAIL;

	//Effect
	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Effect/BossSkillAura/%d.png", 91);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Effect_BossSkillAura", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Effect/SingleTexture/flare_0.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Effect_BossBackAura", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Effect/FireBall/fire1_%d.png", 75);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Effect_FireBall", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Effect/HitZone/HitZone_Circle.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Effect_HitZone_Circle", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Effect/HitZone/HitZone_Donut.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Effect_HitZone_Donut", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Effect/Thunderbolt/%d.png", 16);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Effect_Thunderbolt", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Effect/Blood/%d.png", 15);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Effect_Blood", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Effect/SingleTexture/x4.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Effect_HealParticle", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Effect/PowerUpAura/PowerUpAura_%d.png", 4);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Effect_PowerUpAura", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/Effect/Wind/Wind%d.png", 30);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Effect_Wind", pComponent)))
		return E_FAIL;

	//UI
	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/targetingCursor_o.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Cursor", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/HUD_Panel_MidGround_PC.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_PlayerInfoBar", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/StatusGauge/verticalBarMeterBG.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_HpBarBG_Player", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/StatusGauge/verticalBarMeterAdditiveLayer.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_HpBar_Player", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/StatusGauge/StatusGauge_%d.png", 4);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_HpBar_Enemy", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/StatusGauge/BossGauge_%d.png", 4);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_HpBar_Boss", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/0_slicerTower_Wheel.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_0_slicerTower", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/1_healSelf_Wheel.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_1_HealSelf", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/2_circularSlice_Wheel.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_2_circularSlice", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/3_bloodRage_Wheel.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_3_bloodRage", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/4_repair_Wheel.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_4_repair", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/5_upgrade_Wheel.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_5_upgrade", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/6_spikyTower_Wheel.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_6_spikyTower", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/7_bouncerTower_Wheel.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_7_bouncerTower", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/8_harpoonTower_Wheel.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_8_harpoonTower", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/9_bowlingTower_Wheel.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_9_bowlingTower", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/Font/Damage_%d.png", 10);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Font_Damage", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/weaponPickupGraphic_o.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_PickupGraphic", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/ItemBoxBG.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Inventory", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/inventoryIconFrame.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Inventory_Blank", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/ItemIcon/leather_chest_Icon.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Icon_Leather_Chest", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/ItemIcon/leather_boots_Icon.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Icon_Leather_Boots", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/ItemIcon/chain_chest_icon.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Icon_Chain_Chest", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/ItemIcon/chain_boots_icon.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Icon_Chain_Boots", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/button_short.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Button_Font_Frame", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/button_short_highlighted.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Button_Font_Frame_Select", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/Font_Putting.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Button_Font_Putting", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/Font_ThrowAway.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Button_Font_ThrowAway", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/arrowButton.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Button_Left", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/arrowButtonFlip.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Button_Right", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/checkbox_on_nobg.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Sign_Check", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/ProfileBG.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_ProfileBG", pComponent)))
		return E_FAIL;

	pComponent = CTexture::Create(m_pGraphic_Device, CTexture::TYPE_NORMAL, L"../Bin/Resource/Texture/Stage/UI/Win.png");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Texture_Win", pComponent)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Ready_Stage_Mesh(void)
{
	lstrcpy(m_szText, L"Loaing Mesh...");

	CComponent*	pComponent = nullptr;

	//Object
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"AncientTree.X", L"Com_Mesh_AncientTree"))) 
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Arch.X", L"Com_Mesh_Arch"))) 
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"BackGroundTerrain.X", L"Com_Mesh_BackGroundTerrain")))
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Barrel.X", L"Com_Mesh_Barrel"))) 
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Box.X", L"Com_Mesh_Box"))) 
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Box_Broken.X", L"Com_Mesh_Box_Broken"))) 
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"BSPMesh.X", L"Com_Mesh_BSPMesh"))) 
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Cart.X", L"Com_Mesh_Cart")))
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"ChurchRoof.X", L"Com_Mesh_ChurchRoof"))) 
		return E_FAIL;
	//if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Crystal.X", L"Com_Mesh_Crystal"))) 
	//	return E_FAIL;
	//if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"CrystalGroup_01.X", L"Com_Mesh_CrystalGroup_01"))) 
	//	return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"DirtSpill.X", L"Com_Mesh_DirtSpill"))) 
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Door_Left.X", L"Com_Mesh_Door_Left"))) 
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Door_Left_Static.X", L"Com_Mesh_Door_Left_Static")))
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Door_Right.X", L"Com_Mesh_Door_Right"))) 
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Door_Right_Static.X", L"Com_Mesh_Door_Right_Static")))
		return E_FAIL;
	//if(FAILED(Create_StaticMashComponent(L"../Bin/Rzesource/Mesh/Static/Stage/Map/Object/", L"Faling_Leaf.X", L"Com_Mesh_Faling_Leaf")))
	//	return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"FireTorch.X", L"Com_Mesh_FireTorch"))) 
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Gargoyle.X", L"Com_Mesh_Gargoyle"))) 
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"GraveStone.X", L"Com_Mesh_GraveStone"))) 
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"GreyStoneStairsx8.X", L"Com_Mesh_GreyStoneStairsx8")))
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"GreyStoneStairsx16.X", L"Com_Mesh_GreyStoneStairsx16")))
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Hammer.X", L"Com_Mesh_Hammer")))
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Hammer_Plaque.X", L"Com_Mesh_Hammer_Plaque"))) 
		return E_FAIL;
	/*if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"HayGroup.X", L"Com_Mesh_HayGroup"))) 
		return E_FAIL;*/
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"ladder_Rungs.X", L"Com_Mesh_ladder_Rungs"))) 
		return E_FAIL;
	/*if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Largecrystalgroup.X", L"Com_Mesh_Largecrystalgroup"))) 
		return E_FAIL;*/
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Log.X", L"Com_Mesh_Log")))
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"LooseTiles.X", L"Com_Mesh_LooseTiles"))) 
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Painting.X", L"Com_Mesh_Painting")))
		return E_FAIL;
	if(FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"RetardCeiling.X", L"Com_Mesh_RetardCeiling"))) 
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Sandpit.X", L"Com_Mesh_Sandpit")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Shovel.X", L"Com_Mesh_Shovel")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"StairCrap.X", L"Com_Mesh_StairCrap")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"StoneWall.X", L"Com_Mesh_StoneWall")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"SkyDonut.X", L"Com_Mesh_SkyDonut")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"TavernDoor.X", L"Com_Mesh_TavernDoor")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Torch.X", L"Com_Mesh_Torch")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Wall_Banner.X", L"Com_Mesh_Wall_Banner")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Water.X", L"Com_Mesh_Water")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"WaterWheel.X", L"Com_Mesh_WaterWheel")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Window.X", L"Com_Mesh_Window")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Windows.X", L"Com_Mesh_Windows")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"WoodDoorFrame.X", L"Com_Mesh_WoodDoorFrame")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"WoodenChest.X", L"Com_Mesh_WoodenChest")))
		return E_FAIL;

	//Roof
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Core/", L"Core%d.X", L"Com_Mesh_Level0_Core%d", 19, 0)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Core_Crystal_%d.X", L"Com_Mesh_Core_Crystal_%d", 3, 0)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"BackgroundRockGroup_0%d.X", L"Com_Mesh_BackgroundRockGroup_0%d", 5)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"BackGroundTreeGroup_0%d.X", L"Com_Mesh_BackGroundTreeGroup_0%d", 3)))
		return E_FAIL;
	//if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"DeskGroup_01.X", L"Com_Mesh_DeskGroup_01")))
	//	return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"FloorTrim_0%d.X", L"Com_Mesh_FloorTrim_0%d", 2)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"LightFixtureGroup_0%d.X", L"Com_Mesh_LightFixtureGroup_0%d", 9)))
		return E_FAIL;
	//if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"LightFixtureGroup_01_Crystal.X", L"Com_Mesh_LightFixtureGroup_01_Crystal")))
	//	return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"MineRailGroup_01.X", L"Com_Mesh_MineRailGroup_01")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"SmallRockGroup_0%d.X", L"Com_Mesh_SmallRockGroup_0%d", 8)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"SwampTreeGroup_0%d.X", L"Com_Mesh_SwampTreeGroup_0%d", 9)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"SwampTreeGroup_1%d.X", L"Com_Mesh_SwampTreeGroup_1%d", 6, 0)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"TrimGroup_0%d.X", L"Com_Mesh_TrimGroup_0%d", 9)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"TrimGroup_1%d.X", L"Com_Mesh_TrimGroup_1%d", 3, 0)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"VineGroup_0%d.X", L"Com_Mesh_VineGroup_0%d", 4)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"WoodTrim_0%d.X", L"Com_Mesh_WoodTrim_0%d", 4)))
		return E_FAIL;

	//------------------------------------------------------------------------------------

	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Dynamic/Stage/Squire/", L"BowlingBall_Full.X", L"Com_Mesh_BowlingBall_Full")))
		return E_FAIL;

	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Dynamic/Stage/Squire/", L"BowlingBall_Fragment_%d.X", L"Com_Mesh_BowlingBall_Fragment_%d", 5, 0)))
		return E_FAIL;

	//Terrain
	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/Map/Terrain/", L"Terrain_Center.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Terrain_Center", pComponent);

	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/Map/Terrain/", L"Terrain_Left.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Terrain_Left", pComponent);

	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/Map/Terrain/", L"Terrain_Right.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Terrain_Right", pComponent);

	//Light
	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"LightBeamGroup_02.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_LightBeamGroup_02", pComponent);

	//LightMove
	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Circle_Light.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Circle_Light", pComponent);

	//Rock
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"RockGroup_0%d.X", L"Com_Mesh_RockGroup_0%d", 9)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"RockGroup_1%d.X", L"Com_Mesh_RockGroup_1%d", 10, 0)))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"RockGroup_2%d.X", L"Com_Mesh_RockGroup_2%d", 6, 0)))
		return E_FAIL;

	//Tree
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Tree_Marsh_Short_Low.X", L"Com_Mesh_Tree_Marsh_Short_Low")))
		return E_FAIL;
	if (FAILED(Create_StaticMashComponent(L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Tree_Marsh_Tall.X", L"Com_Mesh_Tree_Marsh_Tall")))
		return E_FAIL;

	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/Map/Object/", L"Tree_Marsh_Short.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Tree_Marsh_Short", pComponent);

	//Weapon
	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/Weapon/", L"Sword01.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Sword01", pComponent);

	//Tower
	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Dynamic/Stage/Squire/", L"Blockade.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_SpikyTower", pComponent);

	//Dynamic
	pComponent = CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Dynamic/Stage/", L"Player.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Player", pComponent);

	pComponent = CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Dynamic/Stage/", L"Goblin.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Goblin", pComponent);

	pComponent = CMesh_Dynamic::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Dynamic/Stage/", L"DemonLord.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_DemonLord", pComponent);

	//Effect
	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/Effect/", L"Tornado.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Tornado", pComponent);

	//ItemIcon
	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/ItemIcon/", L"Leather_Chest_Pickup.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Leather_Chest_Pickup", pComponent);

	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/ItemIcon/", L"Leather_Boots_Pickup.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Leather_Boots_Pickup", pComponent);

	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/ItemIcon/", L"Chain_Chest_Pickup.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Chain_Chest_Pickup", pComponent);

	pComponent = CMesh_Static::Create(m_pGraphic_Device, L"../Bin/Resource/Mesh/Static/Stage/ItemIcon/", L"Chain_Boots_Pickup.X");
	if (nullptr == pComponent)
		return E_FAIL;
	m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Mesh_Chain_Boots_Pickup", pComponent);

	return NOERROR;
}

HRESULT CLoading::Ready_Stage_Shader(void)
{
	CComponent*	pComponent = nullptr;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_Sky.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Shader_Sky", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_Mesh.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Shader_Mesh", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_Unit.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Shader_Unit", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_Light.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Shader_Light", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_LightMove.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Shader_LightMove", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_PlaneObject.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Shader_PlaneObject", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_SpakleRock.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Shader_SpakleRock", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_Effect_Bless.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Shader_Effect_Bless", pComponent)))
		return E_FAIL;

	pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_HpBar.fx");
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Shader_HpBar", pComponent)))
		return E_FAIL;

	//pComponent = CShader::Create(m_pGraphic_Device, L"../Bin/Shader/Shader_Shadow.fx");
	//if (nullptr == pComponent)
	//	return E_FAIL;
	//if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, L"Com_Shader_Shadow", pComponent)))
	//	return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Ready_Stage_Prototype_Object(void)
{
	lstrcpy(m_szText, L"Loaing Object...");

	CGameObject*	pGameObject = nullptr;

	//Static
	if (FAILED(Create_NoCullingObject(L"Level0_Core0")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core1")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core2")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core3")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core4")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core5")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core6")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core7")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core8")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core9")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core10")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core11")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core12")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core13")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core14")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core15")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core16")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core17")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Level0_Core18")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"LooseTiles")))
		return E_FAIL;
	//if (FAILED(Create_NoCullingObject(L"WoodTrim_03")))
	//	return E_FAIL;

	
	if (FAILED(Create_NoCullingObject(L"Terrain_Center")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Terrain_Left")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"Terrain_Right")))
		return E_FAIL;
	
	//if (FAILED(Create_SmallObject(L"AncientTree")))
	//	return E_FAIL;
	if (FAILED(Create_SmallObject(L"Arch")))
		return E_FAIL;
	//if (FAILED(Create_MashObject(L"BackGroundTerrain")))
	//	return E_FAIL;
	//if (FAILED(Create_MashObject(L"BackgroundRockGroup_0%d", 5)))
	//	return E_FAIL;
	if (FAILED(Create_SmallObject(L"Barrel")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"Box")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"Box_Broken")))
		return E_FAIL;
	if (FAILED(Create_MashObject(L"BSPMesh")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"Cart")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"ChurchRoof")))
		return E_FAIL;
	/*if (FAILED(Create_SmallObject(L"Crystal")))
		return E_FAIL;*/
	//if (FAILED(Create_SmallObject(L"DeskGroup_01")))
	//	return E_FAIL;
	if (FAILED(Create_MashObject(L"DirtSpill")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"Door_Left_Static")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"Door_Right_Static")))
		return E_FAIL;
	/*if (FAILED(Create_SmallObject(L"Faling_Leaf")))
		return E_FAIL;*/
	if (FAILED(Create_SmallObject(L"FireTorch")))
		return E_FAIL;
	if (FAILED(Create_MashObject(L"FloorTrim_0%d", 2)))
		return E_FAIL;
	//if (FAILED(Create_SmallObject(L"Gargoyle")))
	//	return E_FAIL;
	//if (FAILED(Create_SmallObject(L"GraveStone")))
	//	return E_FAIL;
	if (FAILED(Create_MashObject(L"GreyStoneStairsx8")))
		return E_FAIL;
	if (FAILED(Create_MashObject(L"GreyStoneStairsx16")))
		return E_FAIL;
	//if (FAILED(Create_SmallObject(L"Hammer")))
	//	return E_FAIL;
	//if (FAILED(Create_SmallObject(L"Hammer_Plaque")))
	//	return E_FAIL;
	//if (FAILED(Create_MashObject(L"HayGroup")))
	//	return E_FAIL;
	//if (FAILED(Create_SmallObject(L"ladder_Rungs")))
	//	return E_FAIL;
	//if (FAILED(Create_MashObject(L"Largecrystalgroup")))
	//	return E_FAIL;
	/*if (FAILED(Create_SmallObject(L"LightFixtureGroup_0%d", 9)))
		return E_FAIL;*/
	/*if (FAILED(Create_MashObject(L"LightFixtureGroup_01_Crystal")))
		return E_FAIL;*/
	//if (FAILED(Create_SmallObject(L"Log")))
	//	return E_FAIL;
	if (FAILED(Create_SmallObject(L"MineRailGroup_01")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"Painting")))
		return E_FAIL;
	//if (FAILED(Create_MashObject(L"Sandpit")))
	//	return E_FAIL;
	//if (FAILED(Create_SmallObject(L"Shovel")))
	//	return E_FAIL;
	//if (FAILED(Create_SmallObject(L"SmallRockGroup_0%d", 8)))
	//	return E_FAIL;
	if (FAILED(Create_MashObject(L"StairCrap")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"StoneWall")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"Torch")))
		return E_FAIL;
	if (FAILED(Create_NoCullingObject(L"TrimGroup_01")))
		return E_FAIL;
	if (FAILED(Create_MashObject(L"TrimGroup_0%d", 8, 2)))
		return E_FAIL;
	if (FAILED(Create_MashObject(L"TrimGroup_1%d", 3, 0)))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"VineGroup_0%d", 4)))
		return E_FAIL;
	//if (FAILED(Create_SmallObject(L"Wall_Banner")))
	//	return E_FAIL;
	if (FAILED(Create_MashObject(L"Water")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"WaterWheel")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"Window")))
		return E_FAIL;
	//if (FAILED(Create_SmallObject(L"Windows")))
	//	return E_FAIL;
	if (FAILED(Create_SmallObject(L"WoodDoorFrame")))
		return E_FAIL;
	if (FAILED(Create_SmallObject(L"WoodenChest")))
		return E_FAIL;
	//if (FAILED(Create_MashObject(L"WoodTrim_01")))
	//	return E_FAIL;
	//if (FAILED(Create_MashObject(L"WoodTrim_02")))
	//	return E_FAIL;
	//if (FAILED(Create_MashObject(L"WoodTrim_04")))
	//	return E_FAIL;

	pGameObject = CLight::Create(m_pGraphic_Device, L"LightBeamGroup_02");
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Mesh_LightBeamGroup_02", pGameObject)))
		return E_FAIL;

	pGameObject = CLightMove::Create(m_pGraphic_Device, L"Circle_Light");
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Mesh_Circle_Light", pGameObject)))
		return E_FAIL;

	pGameObject = CDoor_Left::Create(m_pGraphic_Device, L"Door_Left");
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Mesh_Door_Left", pGameObject)))
		return E_FAIL;

	pGameObject = CDoor_Right::Create(m_pGraphic_Device, L"Door_Right");
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Mesh_Door_Right", pGameObject)))
		return E_FAIL;

	pGameObject = CMainCrystal::Create(m_pGraphic_Device, L"Core_Crystal_0");
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Mesh_Core_Crystal_0", pGameObject)))
		return E_FAIL;

	pGameObject = CMainCrystal_Support::Create(m_pGraphic_Device, L"Core_Crystal_1", true);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Mesh_Core_Crystal_1", pGameObject)))
		return E_FAIL;

	pGameObject = CMainCrystal_Support::Create(m_pGraphic_Device, L"Core_Crystal_2", false);
	if (nullptr == pGameObject)
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Mesh_Core_Crystal_2", pGameObject)))
		return E_FAIL;

	/*if (FAILED(Create_SpakleRock(L"RockGroup_0%d", 9)))
		return E_FAIL;
	if (FAILED(Create_SpakleRock(L"RockGroup_1%d", 10, 0)))
		return E_FAIL;
	if (FAILED(Create_SpakleRock(L"RockGroup_2%d", 6, 0)))
		return E_FAIL;*/
	//if (FAILED(Create_SpakleRock(L"RetardCeiling", 1)))
	//	return E_FAIL;

	//if (FAILED(Create_PlaneObject(L"Tree_Marsh_Short_Low")))
	//	return E_FAIL;
	//if (FAILED(Create_PlaneObject(L"Tree_Marsh_Tall")))
	//	return E_FAIL;
	//if (FAILED(Create_PlaneObject(L"Tree_Marsh_Short")))
	//	return E_FAIL;
	if (FAILED(Create_PlaneObject(L"SwampTreeGroup_0%d", 9)))
		return E_FAIL;
	if (FAILED(Create_PlaneObject(L"SwampTreeGroup_1%d", 5, 0)))
		return E_FAIL;
	//if (FAILED(Create_PlaneObject(L"BackGroundTreeGroup_0%d", 3)))
	//	return E_FAIL;

	pGameObject = CTerrain::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Terrain", pGameObject)))
		return E_FAIL;

	pGameObject = CSky::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Sky", pGameObject)))
		return E_FAIL;

	//Camera
	CAMERA_DESC		tCameraDesc;
	ZeroMemory(&tCameraDesc, sizeof(CAMERA_DESC));
	tCameraDesc.vEye = _vec3(0.0f, 5.0f, -3.0f);
	tCameraDesc.vAt = _vec3(100.f, 0.0f, 100.f);
	tCameraDesc.vUp = _vec3(0.0f, 1.f, 0.0f);

	PROJECTION_DESC	tProjectionDesc;
	ZeroMemory(&tProjectionDesc, sizeof(PROJECTION_DESC));
	tProjectionDesc.fAspect = _float(g_iBackCX) / g_iBackCY;
	tProjectionDesc.fFar = 1000.f;
	tProjectionDesc.fNear = 0.2f;
	tProjectionDesc.fFovY = D3DXToRadian(60.f);

	pGameObject = CCamera_Free::Create(m_pGraphic_Device, &tCameraDesc, &tProjectionDesc);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_CameraFree", pGameObject)))
	return E_FAIL;

	//Unit
	pGameObject = CPlayer::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Player", pGameObject)))
		return E_FAIL;

	pGameObject = CGoblin::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Goblin", pGameObject)))
		return E_FAIL;

	pGameObject = CDemonLord::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_DemonLord", pGameObject)))
		return E_FAIL;

	//Weapon
	pGameObject = CSword::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Weapon", pGameObject)))
		return E_FAIL;

	//Tower
	pGameObject = CSpikyTower::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_SpikyTower", pGameObject)))
		return E_FAIL;

	pGameObject = CBowlingBall_Fragment::Create(m_pGraphic_Device, L"BowlingBall_Full", CBowlingBall_Fragment::TYPE_FULL, true);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_BowlingBall_Full_Boss", pGameObject)))
		return E_FAIL;

	pGameObject = CBowlingBall_Fragment::Create(m_pGraphic_Device, L"BowlingBall_Full", CBowlingBall_Fragment::TYPE_FULL, false);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_BowlingBall_Full", pGameObject)))
		return E_FAIL;

	pGameObject = CBowlingBall_Fragment::Create(m_pGraphic_Device, L"BowlingBall_Fragment_0", CBowlingBall_Fragment::TYPE_FRAGMENT_0, false);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_BowlingBall_Fragment_0", pGameObject)))
		return E_FAIL;

	pGameObject = CBowlingBall_Fragment::Create(m_pGraphic_Device, L"BowlingBall_Fragment_1", CBowlingBall_Fragment::TYPE_FRAGMENT_1, false);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_BowlingBall_Fragment_1", pGameObject)))
		return E_FAIL;

	pGameObject = CBowlingBall_Fragment::Create(m_pGraphic_Device, L"BowlingBall_Fragment_2", CBowlingBall_Fragment::TYPE_FRAGMENT_2, false);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_BowlingBall_Fragment_2", pGameObject)))
		return E_FAIL;

	pGameObject = CBowlingBall_Fragment::Create(m_pGraphic_Device, L"BowlingBall_Fragment_3", CBowlingBall_Fragment::TYPE_FRAGMENT_3, false);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_BowlingBall_Fragment_3", pGameObject)))
		return E_FAIL;

	pGameObject = CBowlingBall_Fragment::Create(m_pGraphic_Device, L"BowlingBall_Fragment_4", CBowlingBall_Fragment::TYPE_FRAGMENT_4, false);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_BowlingBall_Fragment_4", pGameObject)))
		return E_FAIL;

	//Effect
	pGameObject = CFireBall::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_FireBall", pGameObject)))
		return E_FAIL;

	pGameObject = CBossSkillAura::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_BossSkillAura", pGameObject)))
		return E_FAIL;

	pGameObject = CBossBackAura::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_BossBackAura", pGameObject)))
		return E_FAIL;

	pGameObject = CHitZone::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HitZone", pGameObject)))
		return E_FAIL;

	pGameObject = CTornado::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Tornado", pGameObject)))
		return E_FAIL;

	pGameObject = CThunderbolt::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Thunderbolt", pGameObject)))
		return E_FAIL;

	pGameObject = CBlood::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Blood", pGameObject)))
		return E_FAIL;

	pGameObject = CHealParticle::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HealParticle", pGameObject)))
		return E_FAIL;

	pGameObject = CPowerUpAura::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_PowerUpAura", pGameObject)))
		return E_FAIL;

	pGameObject = CWind::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Wind", pGameObject)))
		return E_FAIL;

	//UI
	pGameObject = CCursor::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Cursor", pGameObject)))
		return E_FAIL;

	pGameObject = CPlayerInfoBar::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_PlayerInfoBar", pGameObject)))
		return E_FAIL;

	pGameObject = CHpBarBG_Player::Create(m_pGraphic_Device, true);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HpBarBG_Player_Left", pGameObject)))
		return E_FAIL;

	pGameObject = CHpBarBG_Player::Create(m_pGraphic_Device, false);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HpBarBG_Player_Right", pGameObject)))
		return E_FAIL;

	pGameObject = CHpBar_Player::Create(m_pGraphic_Device, true);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HpBar_Player_Left", pGameObject)))
		return E_FAIL;

	pGameObject = CHpBar_Player::Create(m_pGraphic_Device, false);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HpBar_Player_Right", pGameObject)))
		return E_FAIL;

	pGameObject = CHpBarBG_Enemy::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HpBarBG_Enemy", pGameObject)))
		return E_FAIL;

	pGameObject = CHpBar_Enemy::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HpBar_Enemy", pGameObject)))
		return E_FAIL;

	pGameObject = CHpBarBG_Boss::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HpBarBG_Boss", pGameObject)))
		return E_FAIL;

	pGameObject = CHpBar_Boss::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HpBar_Boss", pGameObject)))
		return E_FAIL;

	pGameObject = CHpBarFR_Boss::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_HpBarFR_Boss", pGameObject)))
		return E_FAIL;

	pGameObject = CSkillBarBG_Boss::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_SkillBarBG_Boss", pGameObject)))
		return E_FAIL;

	pGameObject = CSkillBar_Boss::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_SkillBar_Boss", pGameObject)))
		return E_FAIL;

	pGameObject = CFont_Damage::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Font_Damage", pGameObject)))
		return E_FAIL;

	pGameObject = CItemIcon::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_ItemIcon", pGameObject)))
		return E_FAIL;

	pGameObject = CInventory::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Inventory", pGameObject)))
		return E_FAIL;

	pGameObject = CInventory_Blank::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Inventory_Blank", pGameObject)))
		return E_FAIL;

	pGameObject = CInventory_Button::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Inventory_Button", pGameObject)))
		return E_FAIL;

	pGameObject = CSign_Check::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Sign_Check", pGameObject)))
		return E_FAIL;

	pGameObject = CSign_Win::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Sign_Win", pGameObject)))
		return E_FAIL;

	pGameObject = CEquipmentBox::Create(m_pGraphic_Device);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Profile", pGameObject)))
		return E_FAIL;

	for (_uint i = 0; i < 10; ++i)
	{
		pGameObject = CSkillButton::Create(m_pGraphic_Device, i);

		switch (i)
		{
		case 0:
			if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Button0", pGameObject)))
				return E_FAIL;
			break;
		case 1:
			if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Button1", pGameObject)))
				return E_FAIL;
			break;
		case 2:
			if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Button2", pGameObject)))
				return E_FAIL;
			break;
		case 3:
			if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Button3", pGameObject)))
				return E_FAIL;
			break;
		case 4:
			if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Button4", pGameObject)))
				return E_FAIL;
			break;
		case 5:
			if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Button5", pGameObject)))
				return E_FAIL;
			break;
		case 6:
			if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Button6", pGameObject)))
				return E_FAIL;
			break;
		case 7:
			if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Button7", pGameObject)))
				return E_FAIL;
			break;
		case 8:
			if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Button8", pGameObject)))
				return E_FAIL;
			break;
		case 9:
			if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Button9", pGameObject)))
				return E_FAIL;
			break;
		default:
			break;
		}
	}


	return NOERROR;
}

HRESULT CLoading::Ready_Stage_Layer(void)
{
	/*if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_CameraFree", L"Object_CameraFree")))
		return E_FAIL;*/

	CAMERA_DESC		tCameraDesc;
	ZeroMemory(&tCameraDesc, sizeof(CAMERA_DESC));
	tCameraDesc.vAt = _vec3(0.0f, 1.f, 0.0f);
	tCameraDesc.vEye = _vec3(0.0f, 1.f, 0.0f);
	tCameraDesc.vUp = _vec3(0.0f, 1.f, 0.0f);

	PROJECTION_DESC	tProjectionDesc;
	ZeroMemory(&tProjectionDesc, sizeof(PROJECTION_DESC));
	tProjectionDesc.fAspect = _float(g_iBackCX) / g_iBackCY;
	tProjectionDesc.fFar = 1000.f;
	tProjectionDesc.fNear = 0.2f;
	tProjectionDesc.fFovY = D3DXToRadian(60.f);

	CGameObject* pGameObject = CCamera_Follow::Create(m_pGraphic_Device, &tCameraDesc, &tProjectionDesc);
	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, L"Object_Camera", pGameObject)))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Camera", L"Object_Camera")))
		return E_FAIL;

	//UI
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Cursor", L"Object_Cursor")))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_HpBarBG_Player_Left", L"Object_HpBarBG_Player_Left")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_HpBarBG_Player_Right", L"Object_HpBarBG_Player_Right")))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_HpBar_Player_Left", L"Object_HpBar_Player_Left")))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Button", L"Object_Button0")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Button", L"Object_Button1")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Button", L"Object_Button2")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Button", L"Object_Button3")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Button", L"Object_Button4")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Button", L"Object_Button5")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Button", L"Object_Button6")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Button", L"Object_Button7")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Button", L"Object_Button8")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Button", L"Object_Button9")))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_PlayerInfoBar", L"Object_PlayerInfoBar")))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Inventory", L"Object_Inventory")))
		return E_FAIL;
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Profile", L"Object_Profile")))
		return E_FAIL;

	//Object
	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Sky", L"Object_Sky")))
		return E_FAIL;

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Player", L"Object_Player")))
		return E_FAIL;
	CUnit*	pPlayer = dynamic_cast<CUnit*>(m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Player"));
	if (nullptr == pPlayer)
		return E_FAIL;
	CGameObject*	pCamera = m_pObject_Manager->Get_GameObject(SCENE_STAGE, L"Layer_Camera");
	if (nullptr == pCamera)
		return E_FAIL;
	((CCamera_Follow*)pCamera)->Init_Camera(pPlayer);

	if (FAILED(m_pObject_Manager->Add_GameObject(SCENE_STAGE, L"Layer_Weapon", L"Object_Weapon")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Ready_Stage_Sound(void)
{
	CSound_Device*	pSound_Divece = CSound_Device::Get_Instance();
	if (nullptr == pSound_Divece)
		return E_FAIL;

	//1
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/02-Dungeons-Build-Phase.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/03-Dungeons-Combat-Phase.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/06-Boss-Approaching.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/07-Demon-Boss.wav")))
		return E_FAIL;

	//5
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_Appear_Wings.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_Attack_1.wav")))	//근거리 공격
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_Attack_2.wav")))	//원거리 공격
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_Clear.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_Die.wav")))
		return E_FAIL;

	//10
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_Shock.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_Skill_01.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_SKill_01_Explosion.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_SKill_02.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_Skill_03_Complete.wav")))
		return E_FAIL;

	//15
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_Skill_03_Thunder.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Boss_Skill_03_Tornado.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/ClickButton.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/GetItem.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Hit.wav")))
		return E_FAIL;

	//20
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Player_Hurt.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Player_Skill_01.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Player_Skill_02.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Player_Skill_03.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Player_SwordThrowing0.wav")))
		return E_FAIL;

	//25
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Player_SwordThrowing1.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Turn_Clear.wav")))
		return E_FAIL;
	if (FAILED(pSound_Divece->LoadWave(L"../Bin/Resource/Sound/Install_Tower.wav")))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Ready_Stage_Load(void)
{
	//Map Static Mesh Load

	m_pObject_Manager->Load_Object(L"../Bin/Data/Finish_StaticMap_NotFlog_dx9.dat", SCENE_STAGE);

	//Navigation Mesh Load
	CComponent* pComponnet = m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Navigation");
	if (nullptr == pComponnet)
		return E_FAIL;
	
	((CNavigation*)pComponnet)->Load_Cell(L"../Bin/Data/Cell_Finish.dat");

	Safe_Release(pComponnet);

	return NOERROR;
}

HRESULT CLoading::Ready_Stage_Turn(void)
{
	CNavigation*	pNavigation = (CNavigation*)m_pComponent_Manager->Clone_Component(SCENE_STATIC, L"Com_Navigation");
	if (nullptr == pNavigation)
		return E_FAIL;

	_vec3	vZenPoint[5];
	memcpy(vZenPoint, pNavigation->Get_ZenPoint(), sizeof(_vec3) * 5);

	CTurn::Get_Instance()->Ready_Turn(vZenPoint, 5);

	Safe_Release(pNavigation);

	return NOERROR;
}

HRESULT CLoading::Create_MashObject(const _tchar * pNameTag)
{
	CGameObject*	pGameObject = CStatic_Object::Create(m_pGraphic_Device, pNameTag);
	if (nullptr == pGameObject)
		return E_FAIL;

	_tchar*	pObjectTag = L"";
	pObjectTag = new _tchar[128];

	_tchar szObjectTag[128] = L"Object_Mesh_";

	wcscat_s(szObjectTag, pNameTag);
	wcscpy_s(pObjectTag, 128, szObjectTag);

	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, pObjectTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Create_MashObject(const _tchar * pNameTag, const _uint & iCnt, const _uint & iStartCnt)
{
	for (_uint i = iStartCnt; i < iCnt + iStartCnt; ++i)
	{
		_tchar	pTag[128] = L"";
		wsprintfW(pTag, pNameTag, i);

		CGameObject*	pGameObject = CStatic_Object::Create(m_pGraphic_Device, pTag);
		if (nullptr == pGameObject)
			return E_FAIL;

		_tchar*	pObjectTag = L"";
		pObjectTag = new _tchar[128];

		_tchar szObjectTag[128] = L"Object_Mesh_";

		wcscat_s(szObjectTag, pTag);
		wcscpy_s(pObjectTag, 128, szObjectTag);

		if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, pObjectTag, pGameObject)))
			return E_FAIL;
	}
	return NOERROR;
}

HRESULT CLoading::Create_NoCullingObject(const _tchar * pNameTag)
{
	CGameObject*	pGameObject = CNoCulling_Object::Create(m_pGraphic_Device, pNameTag);
	if (nullptr == pGameObject)
		return E_FAIL;

	_tchar*	pObjectTag = L"";
	pObjectTag = new _tchar[128];

	_tchar szObjectTag[128] = L"Object_Mesh_";

	wcscat_s(szObjectTag, pNameTag);
	wcscpy_s(pObjectTag, 128, szObjectTag);

	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, pObjectTag, pGameObject)))
		return E_FAIL;
	return NOERROR;
}

HRESULT CLoading::Create_SmallObject(const _tchar * pNameTag)
{
	CGameObject*	pGameObject = CSmall_Object::Create(m_pGraphic_Device, pNameTag);
	if (nullptr == pGameObject)
		return E_FAIL;

	_tchar*	pObjectTag = L"";
	pObjectTag = new _tchar[128];

	_tchar szObjectTag[128] = L"Object_Mesh_";

	wcscat_s(szObjectTag, pNameTag);
	wcscpy_s(pObjectTag, 128, szObjectTag);

	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, pObjectTag, pGameObject)))
		return E_FAIL;
	return NOERROR;
}

HRESULT CLoading::Create_SmallObject(const _tchar * pNameTag, const _uint & iCnt, const _uint & iStartCnt)
{
	for (_uint i = iStartCnt; i < iCnt + iStartCnt; ++i)
	{
		_tchar	pTag[128] = L"";
		wsprintfW(pTag, pNameTag, i);

		CGameObject*	pGameObject = CSmall_Object::Create(m_pGraphic_Device, pTag);
		if (nullptr == pGameObject)
			return E_FAIL;

		_tchar*	pObjectTag = L"";
		pObjectTag = new _tchar[128];

		_tchar szObjectTag[128] = L"Object_Mesh_";

		wcscat_s(szObjectTag, pTag);
		wcscpy_s(pObjectTag, 128, szObjectTag);

		if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, pObjectTag, pGameObject)))
			return E_FAIL;
	}
	return NOERROR;
}

HRESULT CLoading::Create_SpakleRock(const _tchar * pNameTag, const _uint & iCnt, const _uint & iStartCnt)
{
	for (_uint i = iStartCnt; i < iCnt + iStartCnt; ++i)
	{
		_tchar	pTag[128] = L"";
		wsprintfW(pTag, pNameTag, i);

		CGameObject*	pGameObject = CSpakleRock::Create(m_pGraphic_Device, pTag);
		if (nullptr == pGameObject)
			return E_FAIL;

		_tchar*	pObjectTag = L"";
		pObjectTag = new _tchar[128];

		_tchar szObjectTag[128] = L"Object_Mesh_";

		wcscat_s(szObjectTag, pTag);
		wcscpy_s(pObjectTag, 128, szObjectTag);

		if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, pObjectTag, pGameObject)))
			return E_FAIL;
	}
	return NOERROR;
}

HRESULT CLoading::Create_PlaneObject(const _tchar * pNameTag)
{
	CGameObject*	pGameObject = CSpakleRock::Create(m_pGraphic_Device, pNameTag);
	if (nullptr == pGameObject)
		return E_FAIL;

	_tchar*	pObjectTag = L"";
	pObjectTag = new _tchar[128];

	_tchar szObjectTag[128] = L"Object_Mesh_";

	wcscat_s(szObjectTag, pNameTag);
	wcscpy_s(pObjectTag, 128, szObjectTag);

	if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, pObjectTag, pGameObject)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Create_PlaneObject(const _tchar * pNameTag, const _uint & iCnt, const _uint & iStartCnt)
{
	for (_uint i = iStartCnt; i < iCnt + iStartCnt; ++i)
	{
		_tchar	pTag[128] = L"";
		wsprintfW(pTag, pNameTag, i);

		CGameObject*	pGameObject = CPlaneObject::Create(m_pGraphic_Device, pTag);
		if (nullptr == pGameObject)
			return E_FAIL;

		_tchar*	pObjectTag = L"";
		pObjectTag = new _tchar[128];

		_tchar szObjectTag[128] = L"Object_Mesh_";

		wcscat_s(szObjectTag, pTag);
		wcscpy_s(pObjectTag, 128, szObjectTag);

		if (FAILED(m_pObject_Manager->Add_GameObject_Prototype(SCENE_STAGE, pObjectTag, pGameObject)))
			return E_FAIL;
	}
	return NOERROR;
}

HRESULT CLoading::Create_StaticMashComponent(const _tchar * pFilePath, const _tchar * pFileName, const _tchar * pComTag)
{
	CComponent*	pComponent = CMesh_Static::Create(m_pGraphic_Device, pFilePath, pFileName);
	if (nullptr == pComponent)
		return E_FAIL;
	if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, pComTag, pComponent)))
		return E_FAIL;

	return NOERROR;
}

HRESULT CLoading::Create_StaticMashComponent(const _tchar * pFilePath, const _tchar * pFileName, const _tchar * pComTag, const _uint & iCnt, const _uint & iStartCnt)
{
	_uint iTotal = iCnt + iStartCnt;
	for (_uint i = iStartCnt; i < iTotal; ++i)
	{
		CComponent*	pComponent = CMesh_Static::Create(m_pGraphic_Device, pFilePath, pFileName, i);
		if (nullptr == pComponent)
			return E_FAIL;

		_tchar*	szComTag = L"";
		szComTag = new _tchar[128];
		ZeroMemory(szComTag, sizeof(_tchar));

		wsprintfW(szComTag, pComTag, i);

		if (FAILED(m_pComponent_Manager->Add_Component(SCENE_STAGE, szComTag, pComponent)))
			return E_FAIL;
	}

	return NOERROR;
}

CLoading * CLoading::Create(LPDIRECT3DDEVICE9 pGraphic_Device, SCENEID eID)
{
	CLoading* pInstance = new CLoading(pGraphic_Device);

	if (FAILED(pInstance->Ready_Loading(eID)))
	{
		_MSG_BOX("CLoading Created Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoading::Free(void)
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CS);
	CloseHandle(m_hThread);

	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);

	Safe_Release(m_pGraphic_Device);
}



