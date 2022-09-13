/// <summary>
/// Per Frame 상수 버퍼를 프레임에 1번만 업데이트하기 위한 더미 FX파일
/// 따라서 실제 그리기 처리는 없다
/// 다른 방법은 아직 몰라서 Mk.1으로 제작
/// </summary>
#include "Struct.fx"
#include "LightHelper.fx"

static const uint g_LightInfoMax = 50;

// 상수 버퍼는 16바이트 정렬이 되도록 pad를 넣을 것
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

// 접근용 Tech
technique11 CBPerFrameTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, VS()));
		SetPixelShader(CompileShader(ps_5_0, PS()));
	}
}
