
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
	vector			vPosition : POSITION0;
	float2			vTexUV : TEXCOORD;
};


VS_OUT VS_MAIN(VS_IN In) // ������ �޾ƿ��� ��ȯ�Ͽ� �����Ѵ�.
{
	VS_OUT			Out = (VS_OUT)0.f;

	matrix			matWV, matWVP;

	matWV = mul(g_matWorld, g_matView);
	matWVP = mul(matWV, g_matProj);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	vector			vPosition : POSITION0;
	float2			vTexUV : TEXCOORD;
};

struct PS_OUT
{
	vector			vColor : COLOR;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0.f;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor *= vector(1.f, 1.f, 1.f, 1.f);

	return Out;
}

technique Default_Device
{
	pass Render_Default
	{
		AlphaBlendEnable = true;
		SrcBlend = srcalpha;
		DestBlend = one;

		CullMode = none;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}