
matrix				g_matWorld, g_matView, g_matProj;

cbuffer LightInfo
{
	vector			g_vLightDir;
	vector			g_vLightDiffuse;
	vector			g_vLightAmvient;
	vector			g_vLightSpecular;
};

cbuffer MaterialInfo
{
	vector			g_vMaterialDiffuse;
	vector			g_vMaterialAmvient;
	vector			g_vMaterialSpecular;
	float			g_fSpecularPower;
};

cbuffer	CameraInfo
{
	vector			g_vCameraPosition;
};

texture				g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
};

struct VS_IN
{
	float3			vPosition : POSITION;
	float3			vNormal : NORMAL;
	float2			vTexUV : TEXCOORD;
};

struct VS_OUT
{
	vector			vPosition : POSITION0;
	vector			vPosition_World : POSITION1;
	float2			vTexUV : TEXCOORD;
	vector			vLightResult : COLOR0;
};


VS_OUT VS_MAIN(VS_IN In) // 정점을 받아오고 변환하여 리턴한다.
{
	VS_OUT			Out;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition_World = mul(vector(In.vPosition, 1.f), g_matWorld);
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	//Shader Compute
	vector	vNormal_World = mul(vector(In.vNormal, 0.f), g_matWorld);
	vector	vShade = max(dot(normalize(vNormal_World), normalize(g_vLightDir) * -1.f), 0.f);
	Out.vLightResult = vShade + (g_vLightAmvient * g_vMaterialAmvient);

	return Out;
}

struct PS_IN
{
	vector			vPosition : POSITION0;
	vector			vPosition_World : POSITION1;
	float2			vTexUV : TEXCOORD;
	vector			vLightResult : COLOR0;
};

struct PS_OUT
{
	vector			vColor : COLOR;
};


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = (tex2D(BaseSampler, In.vTexUV)
		* (g_vLightDiffuse * g_vMaterialDiffuse))
		* In.vLightResult;

	return Out;
}

PS_OUT PS_HURT(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0.f;

	//Out.vColor = tex2D(BaseSampler, In.vTexUV) * In.vLightResult;
	Out.vColor = tex2D(BaseSampler, In.vTexUV) + vector(1.f, 0.f, 0.f, 0.5f);

	return Out;
}

technique Default_Device
{
	pass Default_Rendering
	{
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Render_Hurt
	{
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_HURT();
	}

	//pass AlphTestRendering
	//{
	//	AlphaTestEnable = true;
	//	AlphaRef = 0x1f;
	//	AlphaFunc = greater;

	//	CullMode = none;

	//	VertexShader = compile vs_3_0 VS_MAIN();
	//	PixelShader = compile ps_3_0 PS_MAIN();
	//}
}