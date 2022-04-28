#pragma once
#include "Camera.h"

namespace Engine
{
	class CObject_Manager;
	class CInput_Device;
}

class CCamera_Free : public CCamera
{
protected:
	explicit CCamera_Free(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Free(const CCamera_Free& rhs);
	virtual ~CCamera_Free(void) = default;
private:
	virtual HRESULT Ready_GameObject_Clone(void) { return NOERROR; }
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld) { return NOERROR; }
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
private:
	CObject_Manager*		m_pObject_Manager= nullptr;
	CInput_Device*			m_pInput_Device = nullptr;
public:
	static CCamera_Free* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const CAMERA_DESC* pCameraDesc, const PROJECTION_DESC* pProjectDesc);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
protected:
	virtual HRESULT Ready_Camera(const CAMERA_DESC* pCameraDesc, const PROJECTION_DESC* pProjectDesc);
	virtual void Free(void);
};

