

texture		g_ShadowTexture;

sampler ShadowSampler = sampler_state
{
	texture = g_ShadowTexture;
};

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
	PS_OUT			Out = (PS_OUT)0.f;

	vector		vColor[3];

	vColor[0] = tex2D(ShadowSampler, In.vTexUV);
	
	Out.vColor = vColor[0];
	
	return Out;
}


technique Default_Device
{
	pass Default_Rendering
	{
		ZWriteEnable = false;
		ZEnable = false;

		AlphaTestEnable = true;
		AlphaRef = 0x01;
		AlphaFunc = greater;

		VertexShader = NULL;
		PixelShader = compile ps_3_0 PS_MAIN();
	}
}