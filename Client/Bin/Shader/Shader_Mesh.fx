
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

cbuffer BrushInfo
{
	vector			g_vHitZonePos;
	float			g_fHitZoneSize;
};

texture				g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
};

texture				g_HitZoneTexture;

sampler HitZoneSampler = sampler_state
{
	texture = g_HitZoneTexture;
};

texture				g_HitZoneRoundTexture;

sampler HitZoneRoundSampler = sampler_state
{
	texture = g_HitZoneRoundTexture;
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
	vector			vShade : COLOR0;
	vector			vSpecular : COLOR1;
};


VS_OUT VS_MAIN(VS_IN In) // 정점을 받아오고 변환하여 리턴한다.
{
	VS_OUT			Out = (VS_OUT)0.f;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition_World = mul(vector(In.vPosition, 1.f), g_matWorld);
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	//Shader Compute
	vector	vNormal_World = mul(vector(In.vNormal, 0.f), g_matWorld);
	vector	vShade = max(dot(normalize(vNormal_World), normalize(g_vLightDir) * -1.f), 0.f);

	Out.vShade = vShade + (g_vLightAmvient * g_vMaterialAmvient);

	//Specular Compute
	vector	vReflect = reflect(normalize(g_vLightDir), normalize(vNormal_World));
	vector	vLook = Out.vPosition_World - g_vCameraPosition;
	vector	vSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), g_fSpecularPower);

	Out.vSpecular = vSpecular * (g_vLightSpecular * g_vMaterialSpecular);

	return Out;
}

struct PS_IN
{
	vector			vPosition : POSITION0;
	vector			vPosition_World : POSITION1;
	float2			vTexUV : TEXCOORD;
	vector			vShade : COLOR0;
	vector			vSpecular : COLOR1;
};

struct PS_OUT
{
	vector			vColor : COLOR;
};


PS_OUT PS_HITZONE(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0.f;

	vector	vColor = (vector)0.f;

	float	fHitZoneHalfSize = g_fHitZoneSize * 0.5f;

		//Check whether the x position in the hit zone
	if (g_vHitZonePos.x - fHitZoneHalfSize < In.vPosition_World.x
		&& In.vPosition_World.x < g_vHitZonePos.x + fHitZoneHalfSize
		//Check whether the z position in the hit zone
		&& g_vHitZonePos.z - fHitZoneHalfSize < In.vPosition_World.z
		&& In.vPosition_World.z < g_vHitZonePos.z + fHitZoneHalfSize)
	{
		float2			vNewUV;

		vNewUV.x = (In.vPosition_World.x - (g_vHitZonePos.x - fHitZoneHalfSize))
			/ g_fHitZoneSize;
		vNewUV.y = (In.vPosition_World.z - (g_vHitZonePos.z - fHitZoneHalfSize))
			/ g_fHitZoneSize;

		vColor = tex2D(HitZoneSampler, vNewUV);
	}

	vector	vRoundColor = (vector)0.f;

	if (g_vHitZonePos.x - 50 * 0.5f < In.vPosition_World.x && In.vPosition_World.x < g_vHitZonePos.x + 50 * 0.5f
		&& g_vHitZonePos.z - 50 * 0.5f < In.vPosition_World.z && In.vPosition_World.z < g_vHitZonePos.z + 50 * 0.5f)
	{
		float2			vNewUV;

		vNewUV.x = (In.vPosition_World.x - (g_vHitZonePos.x - 50 * 0.5f)) / 50;
		vNewUV.y = (In.vPosition_World.z - (g_vHitZonePos.z - 50 * 0.5f)) / 50;
		vRoundColor = tex2D(HitZoneRoundSampler, vNewUV);
	}

	if (vRoundColor.r > 0.7 && vRoundColor.g > 0.7 && vRoundColor.b > 0.7)
		Out.vColor = (((tex2D(BaseSampler, In.vTexUV) + vColor) + vRoundColor) * (g_vLightDiffuse * g_vMaterialDiffuse)) * In.vShade;
	else
		Out.vColor = ((tex2D(BaseSampler, In.vTexUV) + vColor) * (g_vLightDiffuse * g_vMaterialDiffuse)) * In.vShade;

	return Out;
}


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0.f;

	//Out.vColor = (tex2D(BaseSampler, In.vTexUV)
	//	* (g_vLightDiffuse * g_vMaterialDiffuse))
	//	* In.vShade;
	//	+ In.vSpecular;

	Out.vColor = (tex2D(BaseSampler, In.vTexUV)
		* (g_vLightDiffuse * g_vMaterialDiffuse))
		* In.vShade;

	return Out;
}

technique Default_Device
{
	pass Render_Default
	{
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	pass Render_HitZone
	{
		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_HITZONE();
	}
}