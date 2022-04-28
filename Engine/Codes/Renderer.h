#pragma once

#include "Component.h"

_BEGIN(Engine)

class CTarget_Manager;
class CShader;
class CGameObject;
class _ENGINE_DLL CRenderer : public CComponent
{
public:
	enum RENDERGROUP { RENDER_SKY, RENDER_PRIORITY, RENDER_UNIT, RENDER_NONALPHA, RENDER_ALPHA, RENDER_EFFECT, RENDER_UI_PRIORITY, RENDER_UI, RENDER_END};
private:
	explicit CRenderer(LPDIRECT3DDEVICE9 pGraphic_Device);	
	virtual ~CRenderer() = default;
public:
	HRESULT Ready_Renderer(void);
	HRESULT Add_RenderGroup(RENDERGROUP eGroup, CGameObject* pGameObject);
	void Render_Renderer(void);
private:
	list<CGameObject*>				m_RenderList[RENDER_END];
	typedef list<CGameObject*>		RENDERLIST;
private:
	void Render_Sky(void);
	void Render_Priority(void);
	void Render_Unit(void);
	void Render_NonAlpha(void);
	void Render_Alpha(void);
	void Render_Effect(void);
	void Render_UI_Priority(void);
	void Render_UI(void);
	void Render_Blend(void);
	void Clear_Renderer(void);
private:
	CTarget_Manager*				m_pTarget_Manager = nullptr;
private:
	CShader*						m_pShader_Blend = nullptr;
private:
	LPDIRECT3DVERTEXBUFFER9		m_pVB = nullptr;
	LPDIRECT3DINDEXBUFFER9		m_pIB = nullptr;
public:
	static CRenderer* Create(LPDIRECT3DDEVICE9 pGraphic_Device);
	virtual CComponent* Clone(void);
	virtual void Free(void);

};

_END