
matrix				g_matWorld, g_matView, g_matProj;

float				g_fX, g_fY, g_fZ;

texture				g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
};

struct VS_IN
{
	float3			vPosition : POSITION;
	float2			vTexUV : TEXCOORD;
};

struct VS_OUT_FOR_LOCAL
{
	vector			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	vector			vPosition_World : TEXCOORD1;
};

struct VS_OUT_FOR_WORLD
{
	vector			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	vector			vPosition_Local : TEXCOORD1;
};


VS_OUT_FOR_LOCAL VS_LOCAL(VS_IN In) // 정점을 받아오고 변환하여 리턴한다.
{
	VS_OUT_FOR_LOCAL			Out;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition_World = mul(vector(In.vPosition, 1.f), g_matWorld);
	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

VS_OUT_FOR_WORLD VS_WORLD(VS_IN In) // 정점을 받아오고 변환하여 리턴한다.
{
	VS_OUT_FOR_WORLD			Out;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vPosition_Local = vector(In.vPosition, 1.f);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN_FOR_LOCAL
{
	vector			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	vector			vPosition_World : TEXCOORD1;
};

struct PS_IN_FOR_WORLD
{
	vector			vPosition : POSITION;
	float2			vTexUV : TEXCOORD0;
	vector			vPosition_Local : TEXCOORD1;
};

struct PS_OUT
{
	vector			vColor : COLOR;
};

PS_OUT PS_LOCAL_Y(PS_IN_FOR_LOCAL In)
{
	PS_OUT			Out;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	if (In.vPosition_World.y >= g_fY)
		discard;
	else
		Out.vColor *= vector(1.f, 0.f, 0.f, 1.f);


	return Out;
}

PS_OUT PS_WORLD(PS_IN_FOR_WORLD In)
{
	PS_OUT			Out;

	if (In.vPosition_Local.x >= g_fX)
		discard;
	else
		Out.vColor = tex2D(BaseSampler, In.vTexUV);


	return Out;
}

PS_OUT PS_LOCAL_X(PS_IN_FOR_LOCAL In)
{
	PS_OUT			Out;

	if (In.vPosition_World.x >= g_fX)
		discard;
	else
		Out.vColor = tex2D(BaseSampler, In.vTexUV);


	return Out;
}

PS_OUT PS_MAIN(PS_IN_FOR_LOCAL In)
{
	PS_OUT			Out;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);


	return Out;
}

technique Default_Device
{
	pass Hp_Local_Y
	{
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_LOCAL();
		PixelShader = compile ps_3_0 PS_LOCAL_Y();
	}

	pass Hp_World_X
	{
		CullMode = none;

		//AlphaBlendEnable = true;
		//SrcBlend = SrcAlpha;
		//DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_WORLD();
		PixelShader = compile ps_3_0 PS_WORLD();
	}

	pass Hp_Local_X
	{
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_LOCAL();
		PixelShader = compile ps_3_0 PS_LOCAL_X();
	}

	pass Hp_Defualt
	{
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_LOCAL();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}