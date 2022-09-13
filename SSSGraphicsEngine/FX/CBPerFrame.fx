/// <summary>
/// Per Frame ��� ���۸� �����ӿ� 1���� ������Ʈ�ϱ� ���� ���� FX����
/// ���� ���� �׸��� ó���� ����
/// �ٸ� ����� ���� ���� Mk.1���� ����
/// </summary>
#include "Struct.fx"
#include "LightHelper.fx"

static const uint g_LightInfoMax = 50;

// ��� ���۴� 16����Ʈ ������ �ǵ��� pad�� ���� ��
cbuffer cbPerFrame : register(b0)
{
	CameraInfo gCameraInfo;

	// Directional Light
	uint gDirLightCount;
	float3 pad1;
	DirectionalLight gDirLights[g_LightInfoMax];

	// Spot Light
	uint gSpotLightCount;
	float3 pad2;
	SpotLight gSpotLights[g_LightInfoMax];

	// Point Light
	uint gPointLightCount;
	float3 pad3;
	PointLight gPointLights[g_LightInfoMax];
};


struct PerFrameIn
{
	//float3 PosL : POSITION;
	//float4 Color : COLOR;
};

struct PerFrameOut
{
	//float4 PosH : SV_POSITION;
	//float4 Color : COLOR;
};

PerFrameOut VS(PerFrameIn vin)
{
	PerFrameOut vout;
	//vout.PosH = (1.f, 1.f, 1.f, 1.f);
	return vout;
}

float4 PS(PerFrameOut pin) : SV_Target
{
	return float4(gCameraInfo.gEyePosW, 1.0f);
}

// ���ٿ� Tech
technique11 CBPerFrameTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
