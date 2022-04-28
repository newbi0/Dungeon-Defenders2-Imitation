
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


PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	//Out.vColor.a = (Out.vColor.r + Out.vColor.g + Out.vColor.b) / 0.33;

	return Out;
}

PS_OUT PS_BLACKBACKGROUND(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor.a = ((Out.vColor.r + Out.vColor.g + Out.vColor.b) / 0.33) * 0.2;

	return Out;
}

PS_OUT PS_BLACKBACKGROUND_RED(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor.a = ((Out.vColor.r + Out.vColor.g + Out.vColor.b) / 0.33) * 0.1;

	Out.vColor *= vector(1.f, 0.f, 0.f, 0.7f);

	return Out;
}

PS_OUT PS_BLACKBACKGROUND_YELLOW(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor.a = ((Out.vColor.r + Out.vColor.g + Out.vColor.b) / 0.33) * 0.1;

	Out.vColor *= vector(1.f, 1.f, 0.f, 0.7f);

	return Out;
}

PS_OUT PS_RED(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor = vector(1.f, 0.f, 0.f, 1.f) * Out.vColor.a;

	return Out;
}

PS_OUT PS_YELLOW(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor = vector(1.f, 1.f, 0.f, 1.f) * Out.vColor.a;

	return Out;
}

PS_OUT PS_GREEN(PS_IN In)
{
	PS_OUT			Out;

	Out.vColor = tex2D(BaseSampler, In.vTexUV);

	Out.vColor = vector(0.f, 1.f, 0.f, 1.f) * Out.vColor.a;

	return Out;
}


technique Default_Device
{
	//0
	pass Default_Rendering
	{
		CullMode = none;

		/*ZWriteEnable = false;
		ZEnable = true;*/
		
		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		//BlendOp = Max;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}

	//1
	pass Red
	{
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_RED();
	}

	//2
	pass Yellow
	{
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_YELLOW();
	}

	//3
	pass Green
	{
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_GREEN();
	}

	//4
	pass BlackBackGround
	{
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_BLACKBACKGROUND();
	}

	//5
	pass BlackBackGround_Red_NoZ
	{
		ZWriteEnable = false;
		ZEnable = false;

		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_BLACKBACKGROUND_RED();
	}

	//6
	pass BlackBackGround_Yellow
	{
		CullMode = none;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_BLACKBACKGROUND_YELLOW();
	}

	//7
	pass NoUse_Z
	{
		CullMode = none;

		ZEnable = false;
		ZWriteEnable = false;

		AlphaBlendEnable = true;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;

		VertexShader = compile vs_3_0 VS_MAIN();
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}