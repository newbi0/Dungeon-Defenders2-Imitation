
matrix				g_matWorld, g_matView, g_matProj;

texture				g_BaseTexture;

sampler BaseSampler = sampler_state
{
	texture = g_BaseTexture;
	minfilter = linear;
	magfilter = linear;
	mipfilter = linear;
};

cbuffer UVMoveAmount
{
	float	g_fUVMoveAmount;
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


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out;

	In.vTexUV.x += g_fUVMoveAmount;
	if (In.vTexUV.x > 1.f)
		In.vTexUV.x -= 1.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);
	Out.vColor.a = Out.vColor.r;

	return Out;
}

technique Default_Device
{
	pass Default_Rendering
	{
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}