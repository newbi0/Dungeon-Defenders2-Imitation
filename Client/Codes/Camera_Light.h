#pragma once
#include "Camera.h"

namespace Engine
{
	class CObject_Manager;
	class CInput_Device;
	class CTarget_Manager;
}

class CCamera_Light : public CCamera
{
protected:
	explicit CCamera_Light(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Light(const CCamera_Light& rhs);
	virtual ~CCamera_Light(void) = default;
private:
	virtual HRESULT Ready_GameObject_Clone(void) { return NOERROR; }
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld) { return NOERROR; }
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
private:
	CObject_Manager*		m_pObject_Manager= nullptr;
	CTarget_Manager*		m_pTarget_Manager = nullptr;
	CInput_Device*			m_pInput_Device = nullptr;
private:
	_matrix					m_matLightView;
	_matrix					m_matLightProj;
public:
	static CCamera_Light* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const CAMERA_DESC* pCameraDesc, const PROJECTION_DESC* pProjectDesc);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
protected:
	virtual HRESULT Ready_Camera(const CAMERA_DESC* pCameraDesc, const PROJECTION_DESC* pProjectDesc);
	virtual void Free(void);
};

