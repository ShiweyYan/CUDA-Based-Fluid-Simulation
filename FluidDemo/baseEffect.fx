#include "Light.fx"

cbuffer cbNoChange
{
	float4x4	View;
	float4x4	World;
	float4x4	Proj;
	DirLight	DLight;
	Material	Mat;
};
 
struct VertexIn
{
    float3 PosL  : POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;

	float3 InstancePos : INSTANCEPOS;
};
 
struct VertexOut
{
    float4 PosH  : SV_POSITION;
	float4 Color : COLOR;
	float3 Normal : NORMAL;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;
	vin.PosL.x += vin.InstancePos.x;
	vin.PosL.y += vin.InstancePos.y;
	vin.PosL.z += vin.InstancePos.z;
	vout.PosH = mul(float4(vin.PosL, 1.0f), World);
	vout.PosH = mul(vout.PosH, View);
	vout.PosH = mul(vout.PosH, Proj);

	vout.Color = vin.Color;
	vout.Normal = vin.Normal;

	return vout;
}

float4 PS(VertexOut pin) :SV_Target
{
	return pin.Color*(-dot(DLight.dir,pin.Normal))*DLight.diffuse*Mat.diffuse;
}
 
 
technique11 Tech
{
    pass P0
    {
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}