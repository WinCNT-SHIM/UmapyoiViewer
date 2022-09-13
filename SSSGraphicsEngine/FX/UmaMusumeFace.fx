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
Texture2D gTripleMaskMap;
Texture2D gOptionMaskMap;
Texture2D gToonShadowMap;
Texture2D gEnvironmentMap;

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

// 테크닉에 사용할 렌더 스테이트의 래스터라이저 설정('D3D11_FILL_'를 붙이지 않는다)
RasterizerState WireframeRS
{
	FillMode = WIREFRAME;
	CullMode = Back;
	FrontCounterClockwise = false;
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

VertexOut UmaMusumeFaceVS(VertexIn vin)
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
PixelOut UmaMusumeFacePS(
	VertexOut    pin,
	uniform bool gUseTexure,
	uniform bool gNormalPPL,
	uniform bool gDebugNormal,
	uniform bool gDebugTangent,
	uniform bool gDebugNormapMap)
{
	PixelOut pout;


	// 임의로 설정
	float4 gLightColor0 = float4(1.0f, 1.0f, 1.0f, 1.0f);

	////노멀값 확인
	//pout.LitColor.rgb = pin.NormalW;
	//return pout;
	
	// 오프셋를 적용해보자
	// 오프셋은 TripleMaskMap에서 z값에 저장되어 있음
	//	r0.xyzw = t1.Sample(s1_s, v1.xy).xyzw;
	//	r0.z = cmp(r0.z < cb4[14].z);
	//	if (r0.z != 0)
	//		discard;
	float4 triple =  gTripleMaskMap.Sample(samAnisotropic, pin.Tex);
	//triple *= triple;	// Gamma Correction
	if (triple.z < 0)	//	if (triple.z < gCutOff)
		discard;

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
	//texColor *= texColor;		//texColor.rgb = pow(texColor.rgb, 2.2);


	// 5. 주 광원의 색상을 결합
	texColor *= gLightColor0;


	//////////////////////////////////////////////////////////////////////////
	// Lighting.
	//////////////////////////////////////////////////////////////////////////
	float4 litColor = texColor;
	float diffuseFactor = 0;
	float specFactor = 0;
	float3 DirectionalLightDir = float3(0.f, 0.f, 0.f);
	if (gDirLightCount > 0)
	{  
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float diffFctr = 0;
		float specFctr = 0;

		// Sum the light contribution from each light source.  
		[unroll]
		for (int i = 0; i < gDirLightCount; ++i)
		{
			float4 A, D, S;
			ComputeHalfLambertDirectionalLight(
				gMaterial
				, gDirLights[i]
				, pin.NormalW
				, toEye
				, A, D, S
				, diffFctr
				, specFctr
			);
			ambient += A;
			diffuse += D;
			spec    += S;
			diffuseFactor += diffFctr;
			specFactor += specFctr;

			// Directional Lighting의 방향 구하기
			DirectionalLightDir += gDirLights[i].Direction;
		}
		
		DirectionalLightDir = normalize(DirectionalLightDir);
		
		//saturate(ambient);
		//saturate(diffuse);
		//saturate(spec);

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


	// 6. 선택적 마스크 맵 관련 처리
	//	r2.xyzw = t2.Sample(s2_s, v1.xy).xyzw;
	//	r0.z = asint(cb4[1].x);
	//	r2.xy = float2(-0, -0.5) + r2.yz;
	//	r0.zw = r0.zz * r2.xy + float2(0, 0.5);
	uint gUseOptionMaskMap = 1;		// 원래는 상수 버퍼, 디폴트 값으로 두고 일단 만든다
	float4 option = gOptionMaskMap.Sample(samAnisotropic, pin.Tex);
	//option *= option;	// Gamma Correction
	float metalVal = option.y * gUseOptionMaskMap; // gUseOptionMaskMap은 0과 1의 값인듯
	float distanceRimVal = (option.z - 0.5f) * gUseOptionMaskMap + 0.5f;


	// 12. 섀도우 핸들링 - 섀도우 레벨 얻기
	float gToonStep = 0.4f;
	float gToonFeather = 0.001f;
	float realShowVal = saturate((gToonStep - triple.x * diffuseFactor) / gToonFeather);
	if (!gToonFeather)
		realShowVal = 0;


	// 13. 툰 섀도우 맵으로 작업하기
	float4 gGlobalToonColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
	float4 toon = gToonShadowMap.Sample(samAnisotropic, pin.Tex);
	//toon *= toon;	// Gamma Correction
	toon = gGlobalToonColor * gLightColor0 * toon;


	// 14. 광택 색상 조정
	float gVertextColorToonPower;
	float4 gToonBrightColor = float4(1.0f, 1.0f, 1.0f, 0.0f);
	if (true)	//if (pin.Color.w)
	{
		if (gToonBrightColor.w <= 0.5f)
			texColor *= gToonBrightColor;
		else
			texColor += gToonBrightColor;
	}
	//pout.LitColor.rgb = texColor;
	//return pout;


	// 15. 다크 사이드 맵 교체
	float4 gToonDarkColor = float4(1.0f, 1.0f, 1.0f, 0.0f);
	if (true)	//if (pin.Color.w)
	{
		if (gToonDarkColor.w <= 0.5f)
			texColor = realShowVal * (toon * gToonDarkColor) + (1 - realShowVal) * texColor;
		else
			texColor = realShowVal * (toon + gToonDarkColor) + (1 - realShowVal) * texColor;
	}
	//pout.LitColor.rgb = texColor;
	//return pout;


	// 16. 먼지(더트) 수준 얻기...는 생략


	// 18. 림 라이트 (끼인)각 수정
	float3 pxToCam = normalize(gCameraInfo.gEyePosW - pin.PosW);
	float gRimHorizonOffset = 0.0f;		// 0 ~ 1
	float gRimVerticalOffset = 0.0;		// 0 ~ 1
	float3 horizonBias = float3(gView[0][0], gView[0][1], gView[0][2]);
	float3 verticalBias = float3(gView[0][1], gView[1][1], gView[2][1]);
	//float3 pxToEyeFix = -gRimHorizonOffset * horizonBias + (1 - abs(gRimHorizonOffset)) * toEye;
	float3 pxToEyeFix = -gRimHorizonOffset * horizonBias + (1 - abs(gRimHorizonOffset)) * pxToCam;
	pxToEyeFix = -gRimVerticalOffset * verticalBias + (1 - abs(gRimVerticalOffset)) * pxToEyeFix;


	// 21. Specular 레벨 계산
	float gSpecularPower = 0.15f;
	float cosObjEye = dot(pxToEyeFix, normalize(pin.NormalW));
	triple.y = min(pow(max(cosObjEye, 0), (10 * gSpecularPower + 1)), 1.0f) * triple.y;
	//pout.LitColor = triple.yyyy;
	//return pout;


	// 22. Specular 추가
	//float4 gSpecularColor = float4(1.0f, 1.0, 1.0f, 1.0f);
	float4 gSpecularColor = float4(0.15f, 0.15f, 0.15f, 1.0f);
	texColor = texColor + gLightColor0 * gSpecularColor * triple.y;
	//pout.LitColor.rgb = texColor;
	//return pout;


	// 23. Ambient Light 및 환경 맵 처리
	float gEnvRate = 0.4f;
	float gEnvBias = 5.0f;		// 0 ~ 8
	float envVal   = 0.0f;		// 얼굴은 환경맵을 적용시키지 말자

	float2 envUV = float2(0.5, 0.5) * (mul(pin.NormalW, (float3x3) gView).xy + float2(1.f, 1.f));
	float4 env = gEnvironmentMap.Sample(samAnisotropic, envUV);
	//env *= env; // Gamma Correction? 여기에도 필요한가???
	
	float envPower = gEnvRate * envVal;
	texColor = lerp(texColor, texColor * env * gEnvBias, envPower);
	//pout.LitColor.rgb = texColor;
	//return pout;


	// 19. 림 라이트 광도 계산
	float gRimStep = 0.15;
	float gRimFeather = 0.001;
	float4 gRimColor = float4(1.0f, 1.0f, 1.0f, 0.4f);
	float3 normalW = normalize(pin.NormalW);
	float rimPower = saturate((gRimStep - dot(pxToEyeFix, normalW)) / gRimFeather);
	rimPower = pow(rimPower, 3) * gRimColor.w * distanceRimVal;


	// 20. 림 라이트 광도 계산 한 번 더
	float gRimHorizonOffset2 = 0.0f; // 0 ~ 1
	float gRimVerticalOffset2 = 0.0; // 0 ~ 1
	float3 pxToEyeFix2 = -gRimHorizonOffset2 * horizonBias + (1 - abs(gRimVerticalOffset2)) * pxToEyeFix;
	pxToEyeFix2 = -gRimVerticalOffset2 * verticalBias + (1 - abs(gRimVerticalOffset2)) * pxToEyeFix2;

	float gRimStep2 = 0.15;
	float gRimFeather2 = 0.001;
	float rimPower2 = saturate((gRimStep2 - dot(pxToEyeFix, normalW)) / gRimFeather2);
	rimPower2 = pow(rimPower2, 3) * gRimColor.w * distanceRimVal;


	// 24. 림 라이트의 추가 처리 및 추가(19 및 20 참조)
	float gRimSpecRate = 1.f;	// 0 ~ 1
	float gRimShadow = 2.f;		// 0 ~ 2
	float gRimShadowRate = 1.f;		// ???
	float4 gGlobalRimColor =  float4(1.0f, 1.0f, 1.0f, 0.0f);
	
	float4 rimBase = lerp(gSpecularColor, gRimColor, gRimSpecRate) * rimPower;
	float rimAntiShadow = max(dot(DirectionalLightDir, pin.NormalW), 0) + gRimShadow + gRimShadowRate;
	float realRim = rimBase * rimAntiShadow * gGlobalRimColor;
	
	texColor += realRim;

	//// 림 라이트 테스트
	//pout.LitColor.rgb = float3(realRim, realRim, realRim);
	//return pout;


	// 25. 림 라이트 한 번 더!
	float4 gRimColor2 = float4(0.0f, 0.0f, 0.0f, 0.0f); // 디폴트는 검정색
	float gRimSpecRate2 = 1.f; // 0 ~ 1
	float gRimShadowRate2 = 1.f; // ???
	float4 rimBase2 = lerp(gSpecularColor, gRimColor2, gRimSpecRate2) * rimPower2;
	float rimAntiShadow2 = max(dot(DirectionalLightDir, pin.NormalW), 0) + gRimShadowRate2;
	float realRim2 = rimBase2 * rimAntiShadow2 * gGlobalRimColor;
	
	texColor += realRim2;
	//pout.LitColor.rgb = float3(realRim2, realRim2, realRim2);
	//return pout;


	// 26. 먼지(더트) 추가는 생략
	// 27. irradiance map 적용도 생략
	// 28. 채도 및 기타 사항 적용도 생략


	//////////////////////////////////////////////////////////////////////////
	// Gamma Correction
	//////////////////////////////////////////////////////////////////////////
	//litColor.rgb = sqrt(litColor.rgb);		//litColor.rgb = pow(litColor.rgb, 1 / 2.2);
	

	pout.LitColor = texColor.rgba;
	return pout;
}

technique11 UmaMusumeFaceTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, UmaMusumeFaceVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, UmaMusumeFacePS(true, true, false, false, false)));
		//SetRasterizerState( WireframeRS ); // 렌더 스테이트 작성 예제
	}
}