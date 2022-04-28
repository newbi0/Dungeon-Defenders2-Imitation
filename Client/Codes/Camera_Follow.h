#pragma once
#include "Camera.h"

const float BASIC_SPEED = 8.f;

namespace Engine
{
	class CObject_Manager;
	class CInput_Device;
}

class CUnit;
class CCamera_Follow : public CCamera
{
protected:
	enum INFORMATION { INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POSITION };
protected:
	explicit CCamera_Follow(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera_Follow(const CCamera_Follow& rhs);
	virtual ~CCamera_Follow(void) = default;
public:
	void Set_Target(const CUnit* pTarget, const _vec3& vDistance, const _float& fSpeed = BASIC_SPEED, const _bool& isLookFromFront = false);
	void Set_MouseRot(const _bool& isRot) {
		m_isMouseRot = isRot;}
	void Set_MouseMove(const _bool& isMove) {
		m_isMouseMove = isMove;	}
	void Set_LookDirction(const _bool& isFront) {
		m_isLookFromFront = isFront;	}
	void Set_isShake(const _bool& isShake, const _float& fShakeAmount, const _uint& iShakeNum, const _float& fSpeed = 5.f);
public:
	HRESULT Init_Camera(CUnit* pTarget);
public:
	void Shake_Camera(const _float& fTimeDelta);
private:
	void Camera_Move(const _float& fTimeDelta, const _matrix& matTarget);
	const _vec3 Compute_Back_Direction(const _matrix& matTarget);
public:
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
private:
	CObject_Manager*		m_pObject_Manager = nullptr;
	CInput_Device*			m_pInput_Device = nullptr;
	const CUnit*			m_pTarget = nullptr;
private:
	_float					m_fLimitY = 0.f;
	_bool					m_isCameraMove = true;
	_bool					m_isMouseRot = true;
	_bool					m_isMouseMove = true;
	_bool					m_isLookFromFront = false;		//카메라가 객체 앞을 보게 할건지
	_vec3					m_vDistance;
	_float					m_fSpeed = 8.f;
//For. Change Target
private:
	_bool					m_isChangeTarget = false;
	_vec3					m_vChangedEye;
	_vec3					m_vChangedAt;
//For. Shake
private:
	_bool					m_isShake = false;
	_bool					m_isShakeInit = false;
	_bool					m_isShakeDown = false;
	_float					m_fShakeSpeed = 0.f;
	_float					m_fShakeAmount = 0.f;
	_uint					m_iShakeNum = 0;
	_uint					m_iShakeMaxNum = 0;
	_vec3					m_vOriginalEye;
	_vec3					m_vOriginalAt;
public:
	static CCamera_Follow* Create(LPDIRECT3DDEVICE9 pGraphic_Device, const CAMERA_DESC* pCameraDesc, const PROJECTION_DESC* pProjectDesc);
	virtual CGameObject* Clone(const _vec3* pPos = nullptr);
	virtual CGameObject* Clone(const _matrix& matWorld);
private:
	virtual HRESULT Ready_GameObject_Clone(void) { return NOERROR; }
	virtual HRESULT Ready_GameObject_Clone(const _matrix& matWorld) { return NOERROR; }
protected:
	virtual HRESULT Ready_Camera(const CAMERA_DESC* pCameraDesc, const PROJECTION_DESC* pProjectDesc);
	virtual void Free(void);
};
