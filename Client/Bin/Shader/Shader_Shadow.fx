matrix	g_matWorld;
matrix	g_matLightProj, g_matLightView;

struct VS_IN
{
	float3			vPosition : POSITION;
	float2			vTexUV : TEXCOORD;
};

struct VS_OUT
{
	vector			vPosition : POSITION;
	vector			vDepth : TEXCOORD0;
	float2			vTexUV : TEXCOORD1;
};

VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT Out = (VS_OUT)0.f;

	Out.vPosition = mul(vector(In.vPosition, 1.f), g_matWorld);
	Out.vPosition = mul(Out.vPosition, g_matLightView);
	Out.vPosition = mul(Out.vPosition, g_matLightProj);

	Out.vDepth = Out.vPosition;

	Out.vTexUV = In.vTexUV;

	return Out;
}

struct PS_IN
{
	vector			vPosition : POSITION;
	vector			vDepth : TEXCOORD0;
	float2			vTexUV : TEXCOORD1;
};

struct PS_OUT
{
	vector		vColor : COLOR;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT Out = (PS_OUT)0.f;

	float fDepthColor = In.vDepth.z / In.vDepth.w;

	Out.vColor = vector(fDepthColor.xxx, 1.f);

	return Out;
}