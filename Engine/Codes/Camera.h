#pragma once

#include "GameObject.h"

_BEGIN(Engine)

class _ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	enum MATRIXTYPE { MATRIX_VIEW, MATRIX_PROJ, MATRIX_WORLD, MATRIX_END };
protected:
	explicit CCamera(LPDIRECT3DDEVICE9 pGraphic_Device);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;
public:
	const CAMERA_DESC* Get_Camera_Desc(void) const {
		return &m_Camera_Desc;	}
	_matrix Get_Matrix(MATRIXTYPE eType) const {
		return m_matrix[eType];}
public:
	virtual HRESULT Ready_GameObject_Prototype(const CAMERA_DESC* pCameraDesc, const PROJECTION_DESC* pProjectDesc);
	virtual HRESULT Ready_GameObject(void);
	virtual _int Update_GameObject(const _float& fTimeDelta);
	virtual _int LastUpdate_GameObject(const _float& fTimeDelta);
protected:
	CAMERA_DESC				m_Camera_Desc;		
protected:
	PROJECTION_DESC			m_Project_Desc;
protected:
	_matrix					m_matrix[MATRIX_END];
public:
	virtual CGameObject* Clone(const _vec3* pPos = nullptr) _PURE;
	virtual CGameObject* Clone(const _matrix& matWorld) _PURE;
	virtual void Free(void);
};

_END