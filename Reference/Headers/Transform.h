#pragma once

#include "Component.h"

_BEGIN(Engine)

class _ENGINE_DLL CTransform : public CComponent
{
public:
	enum INFORMATION {INFO_RIGHT, INFO_UP, INFO_LOOK, INFO_POSITION};
private:
	explicit CTransform(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;
public:
	const _matrix* Get_WorldMatrix(void) const;
	const _matrix* Get_WorldMatrix_NotRot(void) const;
	const _vec3* Get_Infomation(INFORMATION eInfo) const {
		return (_vec3*)&m_matWorld.m[eInfo][0]; }
	const _float Get_Scale(INFORMATION eInfo) const {
		return D3DXVec3Length(&(_vec3)m_matWorld.m[eInfo]);	}
public:
	HRESULT Ready_Transform(void);
	HRESULT SetUp_OnShader(LPD3DXEFFECT pEffect, const char* pConstantName);
public:
	void Set_ParentMatrixPointer(_matrix* pParentMatrixPointer) {
		m_pParentMatrix = pParentMatrixPointer; }
	void Set_Infomation(const _matrix& matWorld) {
		m_matWorld = matWorld;	}
	void Set_Infomation(INFORMATION eInfo, const _vec3* pInfomation);
	void Set_Infomation(INFORMATION eInfo, const _float& fX, const _float& fY, const _float& fZ);
public:
	void Go_Straight(const _float& fSpeed);
	void Go_Direction(const _vec3 & vDir, const _float& fSpeed);
	void Go_Sliding(const _vec3 & vDir, const _vec3& vNormal, const _float & fSpeed);
	_bool Jump(const _float & fSpeed);
public:
	void RotationX(const _float& fAngleX);
	void RotationY(const _float& fAngleY, const _bool& isFix = false);
	void RotationY(const _float& fAngleY, const _float& fSpeed);
	void RotationZ(const _float& fAngleZ);
	void Scaling(_vec3 vScale);
	void Scaling(const _float& fX, const _float& fY, const _float& fZ);
	void Symmetry_Y(void);
private:
	_matrix						m_matWorld;
	_matrix*					m_pParentMatrix = nullptr;
private:
	_bool						m_isJumpUp = true;
public:
	static CTransform* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void);
	virtual void Free(void);
};

_END