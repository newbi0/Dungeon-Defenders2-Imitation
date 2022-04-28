#pragma once

namespace Engine
{
	typedef struct tagNavigationMash_Info
	{
		D3DXVECTOR3		vA;
		D3DXVECTOR3		vB;
		D3DXVECTOR3		vC;
		unsigned int	iAttribute;
	}NAVIMASHINFO;

	typedef struct tagFrame_Derived : public D3DXFRAME
	{
		D3DXMATRIX			matCombinedTransformationMatrix;
	}D3DXFRAME_DERIVED;

	typedef struct tafMeshContainer_Derived : public D3DXMESHCONTAINER
	{
		LPD3DXMESH				pOriMesh;
		LPDIRECT3DTEXTURE9*		ppTexture;
		D3DXMATRIX**			ppCombinedTransformationMatrices; //피부에 영향주는 행렬
		D3DXMATRIX*				pOffSetMatrices; //피부를 뼈대에 붙이기 위한 행렬
		D3DXMATRIX*				pRenderingMatrices; // 그리기위한 최종적 행렬
		unsigned long			dwNumBones;
	}D3DXMESHCONTAINER_DERIVED;

	typedef struct tagLoadObject_Info
	{
		wchar_t			szLayerTag[128];
		wchar_t			szObjectTag[128];
		D3DXMATRIX		matWorld;
	}LOAD_INFO;

	typedef struct tagCamera_Desc
	{
		D3DXVECTOR3			vEye; // 카메라의 위치
		D3DXVECTOR3			vAt; // 카메라가 바라보는 점.
		D3DXVECTOR3			vUp; // 내 좌표계에서의 y축 벡터
	}CAMERA_DESC;

	typedef struct tagProjection_Desc
	{
		_float				fFovY; // 시야각도 y
		_float				fAspect; // 종횡비
		_float				fNear, fFar;
	}PROJECTION_DESC;

	typedef struct tagVertex_Color
	{
		D3DXVECTOR3			vPosition; // D3DFVF_XYZ
		DWORD				dwColor; // D3DFVF_DIFFUSE
	}VTXCOL;

	typedef struct tagVertex_Normal_Texture
	{
		D3DXVECTOR3			vPosition; // D3DFVF_XYZ // 로컬스페이스 상의 좌표를 의미한다.
		D3DXVECTOR3			vNormal;
		D3DXVECTOR2			vTexUV; // D3DFVF_TEX1
	}VTXNORTEX;

	typedef struct tagVertex_Texture
	{
		D3DXVECTOR3			vPosition; // D3DFVF_XYZ // 로컬스페이스 상의 좌표를 의미한다.
		D3DXVECTOR2			vTexUV; // D3DFVF_TEX1
	}VTXTEX;

	typedef struct tagVertex_ViewPort
	{
		D3DXVECTOR4			vPosition;
		D3DXVECTOR2			vTexUV;
	}VTXVIEWPORT;

	typedef struct tagVertex_Cube_Texture
	{
		D3DXVECTOR3			vPosition; // D3DFVF_XYZ // 로컬스페이스 상의 좌표를 의미한다.
		D3DXVECTOR3			vTexUV; // D3DFVF_TEX1
	}VTXCUBETEX;

	typedef struct tagIndex16
	{
		unsigned short			_0, _1, _2;
	}INDEX16;

	typedef struct tagIndex32
	{
		unsigned long			_0, _1, _2;
	}INDEX32;

	typedef struct tagNode
	{
		_vec3		vPos;
		float		fPCost;
		float		fTCost;
		float		fTotalCost;
		_uint		iIndex;
	} NODE;

	typedef struct tagAniInfo
	{
		unsigned int	iState;
		double			dPeriode;
		bool			isRepetable;
		float			fSpeed;
		float			fWeight;
	}ANIINFO;
}