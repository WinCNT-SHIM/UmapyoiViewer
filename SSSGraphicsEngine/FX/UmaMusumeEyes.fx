#include "CBPerFrame.fx"

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gView;
	float4x4 gProj;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
	uint gObjectID;
	float3 pad4;
};

cbuffer cbSkinned : register(b2)
{
	float4x4 gBoneTransforms[170];
};


// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gEyesHighlight00Map;
Texture2D gEyesHighlight01Map;
Texture2D gEyesHighlight02Map;


SamplerState samLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = WRAP;
	AddressV = WRAP;
};

SamplerState samAnisotropic
{
	Filter = ANISOTROPIC;	//Filter = MIN_MAG_MIP_LINEAR;
	MaxAnisotropy = 4;
	AddressU = WRAP;		//AddressU = CLAMP;
	AddressV = WRAP;		//AddressV = CLAMP;
};


struct VertexIn
{
	float3	PosL		: POSITION;
	float3	NormalL		: NORMAL;
	float2	Tex			: TEXCOORD;
	float3	TangentL	: TANGENT;
	float3	Weights		: WEIGHTS;
	uint4	BoneIndices : BONEINDICES;
	float4	Color		: COLOR;
};

struct VertexOut
{
	float4	PosH		: SV_POSITION;
	float3	PosW		: POSITION;		// o5
	float3	NormalW		: NORMAL0;		// o4
	float2	Tex			: TEXCOORD0;
	float3	TangentW	: TANGENT;
};

struct PixelOut
{
	float4 LitColor : SV_Target0;
	//float4 DepthColor : SV_Target1;
};


VertexOut UmaMusumeEyesVS(VertexIn vin)
{
	VertexOut vout;
	
	// Init array or else we get strange warnings about SV_POSITION.
	// 버텍스 가중치 배열을 초기화한다. 아니면 SV_POSITION 값에 대한 이상한 워닝을 볼 것이다.
	float weights[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	weights[0] = vin.Weights.x;
	weights[1] = vin.Weights.y;
	weights[2] = vin.Weights.z;
	weights[3] = 1.0f - weights[0] - weights[1] - weights[2];

	float3 posL = float3(0.0f, 0.0f, 0.0f);
	float3 normalL = float3(0.0f, 0.0f, 0.0f);
	float3 TangentL = float3(0.0f, 0.0f, 0.0f);

	for (int i = 0; i < 4; ++i)
	{
		// Assume no nonuniform scaling when transforming normals,
		// so  that we do not have to use the inverse-transpose.
		// 논유니폼 스케일이 없다고 가정하므로, 노말값을 위한 역행렬의 전치행렬이 필요없다.
		posL     += weights[i] * mul(float4(vin.PosL, 1.0f),  gBoneTransforms[vin.BoneIndices[i]]).xyz;
		normalL  += weights[i] * mul(vin.NormalL,  (float3x3) gBoneTransforms[vin.BoneIndices[i]]);
		TangentL += weights[i] * mul(vin.TangentL, (float3x3) gBoneTransforms[vin.BoneIndices[i]]);
	}

	// Transform to world space space.
	vout.PosW		= mul(float4(posL, 1.0f), gWorld).xyz;
	vout.NormalW	= mul(normalL, (float3x3) gWorldInvTranspose); // 이때는 역전치행렬을 쓴다.
	vout.TangentW	= mul(float4(TangentL, 1.0f), gWorld).xyz;

	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(posL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

// Normal Mapping에서 파라미터 추가
PixelOut UmaMusumeEyesPS(
	VertexOut    pin,
	uniform bool gUseTexure,
	uniform bool gNormalPPL,
	uniform bool gDebugNormal,
	uniform bool gDebugTangent,
	uniform bool gDebugNormapMap)
{
	PixelOut pout;

	// LightColor0값은 임의로 설정
	float4 gLightColor0 = float4(1.0f, 1.0f, 1.0f, 1.0f);
	
	//// Interpolating normal can unnormalize it, so normalize it.
    //pin.NormalW = normalize(pin.NormalW);

	//// The toEye vector is used in lighting.
	//float3 toEye = gCameraInfo.gEyePosW - pin.PosW;

	//// Cache the distance to the eye from this surface point.
	//float distToEye = length(toEye); 

	//// Normalize.
	//toEye /= distToEye;


	// 눈동자 Diffuse Map의 좌표 재설정
	// x좌표만 1/4 해준다
	float2 texUV = float2(pin.Tex.x * 0.25f, pin.Tex.y);

    float4 texColor = float4(1, 1, 1, 1);
    if(gUseTexure)
	{
		texColor = gDiffuseMap.Sample(samAnisotropic, texUV);
	}


	//////////////////////////////////////////////////////////////////////////
	// Gamma Correction
	//////////////////////////////////////////////////////////////////////////
	texColor *= texColor;		//texColor.rgb = pow(texColor.rgb, 2.2);


	// 주 광원의 색상을 결합
	texColor *= gLightColor0;


	// 눈동자에 하이라이트 추가
	float2 texUV2 = pin.Tex * float2(1, 2);
	float4 eyesHighlight00 = gEyesHighlight00Map.Sample(samAnisotropic, texUV2);
	eyesHighlight00 *= eyesHighlight00;
	float4 eyesHighlight01 = gEyesHighlight01Map.Sample(samAnisotropic, texUV2);
	eyesHighlight01 *= eyesHighlight01;
	float4 eyesHighlight02 = gEyesHighlight02Map.Sample(samAnisotropic, texUV2);
	eyesHighlight02 *= eyesHighlight02;
	
	float4 lerpResHighlight = lerp(float4(1.f, 1.f, 1.f, 1.f), texColor, (1.0f - (eyesHighlight00 + eyesHighlight01)));

	float temp = lerpResHighlight * lerpResHighlight;
	float4 lerpResult78 = lerp(temp, lerpResHighlight, 0.26);
	
	pout.LitColor = sqrt(lerpResHighlight);
	return pout;
}

technique11 UmaMusumeEyesTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, UmaMusumeEyesVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, UmaMusumeEyesPS(true, true, false, false, false)));
	}
}