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

BlendState TransparentBS
{
	BlendEnable[0] = TRUE;
	SrcBlend = SRC_ALPHA;
	DestBlend = ONE;
	BlendOp = ADD;
	RenderTargetWriteMask[0] = 0x0F;
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
	float4	Color		: COLOR;		// o3
	float3	NormalV		: NORMAL1;		// o6
	float4	SpecPower	: TEXCOORD1;	// o7 : 하이라이트?
};

struct PixelOut
{
	float4 LitColor : SV_Target0;
	//float4 DepthColor : SV_Target1;
};

VertexOut UmaMusumeCheekVS(VertexIn vin)
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

	vout.Color = vin.Color; // o3
	
	vout.NormalV = mul(vout.NormalW, (float3x3) gView); //o6

	// 하이라이트 처리
	float gSpecularPower = 0.15f;	// 원래는 상수 버퍼이지만 디폴트 값으로 일단 구현
	vout.SpecPower.x = exp2(gSpecularPower * (-10) + 11);
	vout.SpecPower.yzw = float3(0, 0, 0);
	
	return vout;
}

// Normal Mapping에서 파라미터 추가
PixelOut UmaMusumeCheekPS(
	VertexOut    pin,
	uniform bool gUseTexure,
	uniform bool gNormalPPL,
	uniform bool gDebugNormal,
	uniform bool gDebugTangent,
	uniform bool gDebugNormapMap)
{
	discard;
	
	PixelOut pout;

	//// 볼은 임시로 구현(원래는 정도를 조절하는 처리가 있을 것임)
	//float4 diff = gDiffuseMap.Sample(samAnisotropic, pin.Tex);
	
	//diff *= diff; // Gamma Correction
	//if (diff.r == 1.0f && diff.g == 1.0f && diff.b == 1.0f)
	//	discard;
	////if (diff.g > 0.92f || diff.b > 0.92f)
	//if (diff.g > 0.99f)
	//	discard;
	//if (diff.b > 0.99f)
	//	discard;

	//pout.LitColor = sqrt(pout.LitColor);
	//pout.LitColor = diff;

	//float4 r0, r1;
	//uint4 bitmask, uiDest;
	//float4 fDest;
	
	//float4 cb0[7];
	//cb0[2] = float4(0.00, 0.00, 0.00, 0.00);
	//cb0[3] = float4(0.00, 0.00, 0.00, 0.00);
	//cb0[4] = float4(0.00, 0.00, 0.00, 0.00);
	//cb0[5] = float4(0.00, 0.00, 0.00, 0.00);
	//cb0[6] = float4(0.30, 1.00, 1.00, 0.00);
		
	//r0.xyzw = float4(0.f, 0.f, 0.f, 0.f); // Unity Black
	//r0.xyz = cb0[6].xxx * r0.xyz;
	//r0.y = cb0[3].x * r0.y;
	//r0.x = r0.x * cb0[2].x + r0.y;
	//r0.x = r0.z * cb0[4].x + r0.x;
	//r0.y = 1 + -r0.x;
	
	//r1.xyzw = gDiffuseMap.Sample(samLinear, pin.Tex);
	//r0.yzw = r1.xyz * r0.yyy;
	//pout.LitColor.w = r1.w;
	//r0.xyz = cb0[5].xyz * r0.xxx + r0.yzw;
	//r0.xyz = float3(-1, -1, -1) + r0.xyz;
	//r0.xyz = cb0[6].yyy * r0.xyz + float3(1, 1, 1);
	//r0.w = dot(r0.xyz, float3(0.212500006, 0.715399981, 0.0720999986));
	//r0.xyz = cb0[6].zzz * r0.xyz;
	//r1.x = 1 + -cb0[6].z;

	//pout.LitColor.xyz = r0.www * r1.xxx + r0.xyz;

	return pout;
}

technique11 UmaMusumeCheekTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, UmaMusumeCheekVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, UmaMusumeCheekPS(true, true, false, false, false)));
		//SetBlendState(TransparentBS, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
	}
}
