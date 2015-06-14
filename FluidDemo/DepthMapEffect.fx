cbuffer cbPerObject
{
	float4x4	World;
	float4x4	View;
	float4x4	Proj;
	float4x4	WorldInvTransView;
};

Texture2D colorTex;

SamplerState colorSamplerState
{
	Filter = MIN_MAG_LINEAR_MIP_POINT;
	AddressU = WRAP;
	AddressV = WRAP;
};

struct VertexIn
{
	float3 PosL  : POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
};

struct VertexOut
{
	float4 PosH  : SV_POSITION;
	float4 PosV : TEXCOORD0;
	float3 NormalV : NORMAL;
	float2 Tex : TEXCOORD1;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	// 转换到投影空间
	vout.PosH = mul(float4(vin.PosL, 1.0f), World);
	vout.PosV = mul(vout.PosH, View);
	vout.PosH = mul(vout.PosV, Proj);

	vout.NormalV = mul(float4(vin.Normal,0.0f), WorldInvTransView).xyz;

	vout.Tex = vin.Tex;//这句没用，只是为了填充结构

	return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
	pin.NormalV = normalize(pin.NormalV);

	float4 color = float4(pin.NormalV, pin.PosV.z);

	return color;
}

VertexOut ColorVS(VertexIn vin)
{
	VertexOut vout;

	// 转换到投影空间
	vout.PosH = mul(float4(vin.PosL, 1.0f), World);
	vout.PosV = mul(vout.PosH, View);
	vout.PosH = mul(vout.PosV, Proj);

	vout.NormalV = vin.Normal;//这句没用，只是用来填充结构体

	vout.Tex = vin.Tex;

	return vout;
}

float4 ColorPS(VertexOut pin) : SV_Target
{
	float4 color = colorTex.Sample(colorSamplerState, pin.Tex);

	return color;
}

technique11 Tech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}

	pass P1
	{
		SetVertexShader(CompileShader(vs_5_0, ColorVS()));
		SetPixelShader(CompileShader(ps_5_0, ColorPS()));
	}
}