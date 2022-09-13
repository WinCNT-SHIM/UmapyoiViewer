#include "CBPerFrame.fx"

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gWorldInvTranspose;
	float4x4 gWorldViewProj;
	float4x4 gTexTransform;
	Material gMaterial;
	uint gObjectID;
	float3 pad4;
};

// Nonnumeric values cannot be added to a cbuffer.
Texture2D gDiffuseMap;
Texture2D gNormalMap;	// Normal Mapping
TextureCube gCubeMap;	//Cube Mapping

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
	float3 PosL       : POSITION;
	float3 NormalL    : NORMAL;
	float2 Tex        : TEXCOORD;
	float3 TangentL : TANGENT;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PosH     : SV_POSITION;
    float3 PosW     : POSITION;
    float3 NormalW  : NORMAL;
	float2 Tex      : TEXCOORD;
	float3 TangentW : TANGENT;
};

struct PixelOut
{
	float4 LitColor : SV_Target0;
	//float4 DepthColor : SV_Target1;
};

VertexOut BasicVS(VertexIn vin)
{
	VertexOut vout;
	
	// Transform to world space space.
	vout.PosW    = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.NormalW = mul(vin.NormalL, (float3x3) gWorldInvTranspose);
	// ==================== Normal Mapping 추가 코드 ====================
	vout.TangentW = mul(vin.TangentL, (float3x3) gWorld);
		
	// Transform to homogeneous clip space.
	vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);
	
	// Output vertex attributes for interpolation across triangle.
	vout.Tex = mul(float4(vin.Tex, 0.0f, 1.0f), gTexTransform).xy;

	return vout;
}

// Normal Mapping에서 파라미터 추가
PixelOut BasicPS(
	VertexOut    pin,
	uniform bool gUseTexure,
	uniform bool gNormalPPL,
	uniform bool gDebugNormal,
	uniform bool gDebugTangent,
	uniform bool gDebugNormapMap)
{
	PixelOut pout;
	
	// Interpolating normal can unnormalize it, so normalize it.
    pin.NormalW = normalize(pin.NormalW);

	// The toEye vector is used in lighting.
	float3 toEye = gCameraInfo.gEyePosW - pin.PosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye); 

	// Normalize.
	toEye /= distToEye;
	
    // Default to multiplicative identity.
    float4 texColor = float4(1, 1, 1, 1);
    if(gUseTexure)
	{
		texColor = gDiffuseMap.Sample( samAnisotropic, pin.Tex );
	}


	//////////////////////////////////////////////////////////////////////////
	// Gamma Correction
	//////////////////////////////////////////////////////////////////////////
	texColor *= texColor;		//texColor.rgb = pow(texColor.rgb, 2.2);

	//////////////////////////////////////////////////////////////////////////
	// Normal mapping
	//////////////////////////////////////////////////////////////////////////
	float3 normalMapSample = gNormalMap.Sample(samLinear, pin.Tex).rgb;
	float3 bumpedNormalW = NormalSampleToWorldSpace(normalMapSample, pin.NormalW, pin.TangentW);
	
	//////////////////////////////////////////////////////////////////////////
	// Lighting.
	//////////////////////////////////////////////////////////////////////////
	float4 litColor = texColor;
	if (gDirLightCount > 0)
	{  
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.  
		[unroll]
		for (int i = 0; i < gDirLightCount; ++i)
		{
			float4 A, D, S;
			// 노말맵을 사용하지 않은 상태를 보여준다.
			if (gNormalPPL)
			{
				ComputeDirectionalLight(gMaterial, gDirLights[i], pin.NormalW, toEye,
					A, D, S);
			}
			// 노말맵을 사용한 상태를 보여준다.
			else
			{
				ComputeDirectionalLight(gMaterial, gDirLights[i], bumpedNormalW, toEye,
					A, D, S);
			}

			ambient += A;
			diffuse += D;
			spec    += S;
		}
		
		saturate(ambient);
		saturate(diffuse);
		saturate(spec);

		// Modulate with late add.
		litColor = texColor*(ambient + diffuse) + spec;
		
		//// Cube Mapping
		//if (gReflectionEnabled)
		//{
		//	float3 incident = -toEye;		// incident : 입사 벡터
		//	float3 reflectionVector;
		//	//reflectionVector = reflect(incident, pin.NormalW);
		//	//reflectionVector = incident - 2.0f * dot(incident, pin.NormalW) * pin.NormalW;
		//	reflectionVector = incident - gRefractiveIndex * dot(incident, pin.NormalW) * pin.NormalW;
		//	//float4 reflectionColor = gCubeMap.Sample(samAnisotropic, reflectionVector);
		//	float4 reflectionColor = gCubeMap.Sample(samLinear, reflectionVector);

		//	litColor += gMaterial.Reflect * reflectionColor;
		//}
	}

	// Common to take alpha from diffuse material and texture.
	litColor.a = gMaterial.Diffuse.a * texColor.a;
	
	// 노말 디버그용
	if (gDebugNormal)
	{
		litColor.r = pin.NormalW.x;
		litColor.g = pin.NormalW.y;
		litColor.b = pin.NormalW.z;
		litColor.a = 1.0f;
	}
	else if (gDebugTangent)
	{
		litColor.r = pin.TangentW.x;
		litColor.g = pin.TangentW.y;
		litColor.b = pin.TangentW.z;
		litColor.a = 1.0f;
	}
	// 노말 출력 테스트용
	else if (gDebugNormapMap)
	{
		litColor.r = bumpedNormalW.x;
		litColor.g = bumpedNormalW.y;
		litColor.b = bumpedNormalW.z;
		litColor.a = 1.0f;
	}

	//////////////////////////////////////////////////////////////////////////
	// Gamma Correction
	//////////////////////////////////////////////////////////////////////////
	litColor.rgb = sqrt(litColor.rgb);		//litColor.rgb = pow(litColor.rgb, 1 / 2.2);
	
	
	// 정보를 따로 분리하기 위한 테스트
	pout.LitColor = litColor.rgba;
	//pout.DepthColor = float4(pin.PosH.zzz, 1.0f);
	
	return pout;
}

technique11 BasicTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, BasicVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, BasicPS(true, true, false, false, false)));
	}
}
