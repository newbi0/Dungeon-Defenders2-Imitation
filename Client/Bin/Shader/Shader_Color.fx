
matrix				g_matWorld, g_matView, g_matProj;

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

struct VS_OUT
{
	vector			vPosition : POSITION;
	float2			vTexUV : TEXCOORD;
};


VS_OUT VS_MAIN(VS_IN In) // 정점을 받아오고 변환하여 리턴한다.
{
	VS_OUT			Out;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	vector			vPosition : POSITION;
	float2			vTexUV : TEXCOORD;
};

struct PS_OUT
{
	vector			vColor : COLOR;
};


PS_OUT PS_YELLOW(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = vector(1.f, 1.f, 0.f, 1.f);

	return Out;
}

PS_OUT PS_RED(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = vector(1.f, 0.f, 0.f, 1.f);

	return Out;
}

technique Default_Device
{
	pass Yellow_WireFrame
	{
		fillmode = wireframe;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_YELLOW();
	}

	pass Red_WireFrame
	{
		fillmode = wireframe;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_RED();
	}
}