#pragma once

#include "Engine_Defines.h"
#include "Base.h"
#include "Layer.h"

_BEGIN(Engine)

class CCollider;
class CComponent;
class _ENGINE_DLL CObject_Manager : public CBase
{
	_DECLARE_SINGLETON(CObject_Manager)
private:
	explicit CObject_Manager();
	virtual ~CObject_Manager() = default;
public:
	CGameObject* Get_GameObject(const _uint& iSceneID, const _tchar* pLayerTag, const _uint& iIndex = 0);
	CGameObject* Get_GameObject_ApplyToPosition(const _uint& iSceneID, const _tchar* pLayerTag, const _vec3* pPos);
	CGameObject* Get_LastGameObject(const _uint& iSceneID, const _tchar* pLayerTag);
	const CComponent* Get_Component(const _uint& iSceneID, const _tchar* pLayerTag, const _tchar* pComponentTag, const _uint& iIndex = 0);
	const _uint Get_GameObjectNum(const _uint& iSceneID, const _tchar* pLayerTag);
	const _bool Get_isHitZone(void);
public:
	void Set_HitZone(const CGameObject::HITZONETYPE & eType, const _uint& iSCENEID);
public:
	HRESULT Reserve_Container_Size(const _uint& iMaxSize);
	HRESULT Add_GameObject_Prototype(const _uint& iSceneIdx, const _tchar* pObjectTag, CGameObject* pGameObject_Prototype);
	HRESULT Add_GameObject(const _uint& iSceneIdx, const _tchar* pLayerTag, const _tchar* pObjectTag, const _vec3* pPos = nullptr);
	HRESULT Add_GameObject(const _uint& iSceneIdx, const _tchar* pLayerTag, const _tchar* pObjectTag, _matrix matWorld);
public:
	_int Update_GameObject(const _float& fTimeDelta);
	_int LastUpdate_GameObject(const _float& fTimeDelta);
public:
	_bool Check_Collision_AABB(const _uint & iSceneIdx, CCollider* pCollider, const _tchar* pLayerTag, CGameObject** ppTarget = nullptr);
	_bool Check_Collision_OBB(const _uint & iSceneIdx, CCollider* pCollider, const _tchar* pLayerTag, CGameObject** ppTarget = nullptr, const _tchar* pColliderName = L"Com_Collider");
public:
	void Load_Object(const _tchar* pFilePath, const _uint& SceneID);
public:
	void Clear_DeadObject(void);
	HRESULT Clear_GameObject(const _uint& iSceneIdx);
	HRESULT Clear_GameObject(const _uint& iSceneIdx, const _tchar* pLayerTag);
private:
	unordered_map<const _tchar*, CGameObject*>*			m_pMapObject_Prototype = nullptr;
	typedef unordered_map<const _tchar*, CGameObject*>	MAPOBJECT_PROTOTYPE;
private:
	unordered_map<const _tchar*, CLayer*>*				m_pMapLayer= nullptr;
	typedef unordered_map<const _tchar*, CLayer*>		MAPLAYER;
private:
	vector<const _tchar*>*								m_pVecObjectTag;
	typedef vector<const _tchar*>						VECOBJECTTAG;
//For.Load
private:
	vector<LOAD_INFO*>*									m_pVecLoad;
	typedef vector<LOAD_INFO*>							VECLOAD;
private:
	_uint												m_iMaxSize = 0;
public:
	virtual void Free(void);
};

_END