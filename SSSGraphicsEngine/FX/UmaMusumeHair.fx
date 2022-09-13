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

VertexOut UmaMusumeHairVS(VertexIn vin)
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
PixelOut UmaMusumeHairPS(
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
	
	//pout.LitColor = triple.yyyy;
	//pout.LitColor = float4(pin.NormalW, 1.0f);
	//return pout;
	
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
	// Directional Lighting.
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
	float gToonStep = 0.3f;
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


	// 15. 다크 사이드 맵 교체
	//float4 gToonDarkColor = float4(1.0f, 1.0f, 1.0f, 0.0f);
	float4 gToonDarkColor = float4(1.0f, 1.0f, 1.0f, 0.0f);
	if (true)	//if (pin.Color.w)
	{
		if (gToonDarkColor.w <= 0.5f)
			texColor = realShowVal * (toon * gToonDarkColor) + (1 - realShowVal) * texColor;
		else
			texColor = realShowVal * (toon + gToonDarkColor) + (1 - realShowVal) * texColor;
	}


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


	// 21. 반사광 레벨 계산
	float gSpecularPower = 0.15;
	float cosObjEye = dot(pxToEyeFix, pin.NormalW);
	triple.y = min(pow(max(cosObjEye, 0), (10 * gSpecularPower + 1)), 1) * triple.y;


	// 22. 하이라이트 추가
	float4 gSpecularColor = float4(0.75f, 0.75f, 0.75f, 1.0f);
	texColor = texColor + gLightColor0 * gSpecularColor * triple.y;


	// 23. Ambient Light 및 환경 맵 처리
	float gEnvRate = 0.4f;
	float gEnvBias = 5.0f;		// 0 ~ 8
	float envVal   = 0.05f;		// 캐릭터마다 기본값이 다른듯

	//vout.NormalV = mul(vout.NormalW, (float3x3) gView); //o6

	//float2 envUV = float2(0.5, 0.5) * (pin.NormalV.xy + float2(1.f, 1.f));
	float2 envUV = float2(0.5, 0.5) * (mul(pin.NormalW, (float3x3) gView).xy + float2(1.f, 1.f));
	float4 env = gEnvironmentMap.Sample(samAnisotropic, envUV);
	//env *= env; // Gamma Correction? 여기에도 필요한가???
	
	float envPower = gEnvRate * envVal;
	texColor = lerp(texColor, texColor * env * gEnvBias, envPower);


	// 19. 림 라이트 광도 계산
	float gRimStep = 0.15;
	float gRimFeather = 0.001;
	float4 gRimColor = float4(1.0f, 1.0f, 1.0f, 0.2f);
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
	//float rimAntiShadow = max(saturate(diffuseFactor / gDirLightCount * 2.0f), 0) + gRimShadow + gRimShadowRate;
	//float rimAntiShadow = max(saturate(diffuseFactor), 0) + gRimShadow + gRimShadowRate;
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
	//pout.DepthColor = float4(pin.PosH.zzz, 1.0f);
	
	return pout;
}

technique11 UmaMusumeHairTech
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, UmaMusumeHairVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, UmaMusumeHairPS(true, true, false, false, false)));
		//SetRasterizerState( WireframeRS ); // 렌더 스테이트 작성 예제
	}
}


// 버텍스
// Fri Aug 19 21:36:13 2022
//cbuffer cb3 : register(b3)
//{
//	float4 cb3[40];
//}

//cbuffer cb2 : register(b2)
//{
//	float4 cb2[21];
//}

//cbuffer cb1 : register(b1)
//{
//	float4 cb1[4];
//}

//cbuffer cb0 : register(b0)
//{
//	float4 cb0[10];
//}

////  declarations
//#define cmp -

//void main(
//  float4 v0 : POSITION0,
//  float2 v1 : TEXCOORD0,
//  float3 v2 : NORMAL0,
//  float4 v3 : COLOR0,
//  out float4 o0 : SV_POSITION0,
//  out float2 o1 : TEXCOORD0,
//  out float p1 : TEXCOORD5,
//  out float4 o2 : TEXCOORD7,
//  out float4 o3 : TEXCOORD8,
//  out float4 o4 : TEXCOORD1,
//  out float4 o5 : TEXCOORD2,
//  out float4 o6 : TEXCOORD3,
//  out float4 o7 : TEXCOORD4,
//  out float4 o8 : TEXCOORD10)
//{
//	float4 r0, r1, r2, r3;
//	uint4 bitmask, uiDest;
//	float4 fDest;

//	r0.xyz = cb1[1].xyz * v0.yyy;
//	r0.xyz = cb1[0].xyz * v0.xxx + r0.xyz;
//	r0.xyz = cb1[2].xyz * v0.zzz + r0.xyz;
//	r0.xyz = cb1[3].xyz + r0.xyz;
//	r1.xyzw = cb2[18].xyzw * r0.yyyy;
//	r1.xyzw = cb2[17].xyzw * r0.xxxx + r1.xyzw;
//	r1.xyzw = cb2[19].xyzw * r0.zzzz + r1.xyzw;
//	o0.xyzw = cb2[20].xyzw + r1.xyzw;
//	r1.xyzw = cb1[1].xyzw * v0.yyyy;
//	r1.xyzw = cb1[0].xyzw * v0.xxxx + r1.xyzw;
//	r1.xyzw = cb1[2].xyzw * v0.zzzz + r1.xyzw;
//	r1.xyzw = cb1[3].xyzw + r1.xyzw;
//	r0.w = cb2[10].z * r1.y;
//	r0.w = cb2[9].z * r1.x + r0.w;
//	r0.w = cb2[11].z * r1.z + r0.w;
//	r0.w = cb2[12].z * r1.w + r0.w;
//	r0.w = -cb0[7].w + -r0.w;
//	r0.w = saturate(r0.w / cb0[8].w);
//	r0.w = 1 + -r0.w;
//	r1.x = saturate(r0.y / cb0[9].y);
//	r1.x = 1 + -r1.x;
//	p1.x = saturate(r0.w * r1.x + cb0[9].x);
//	o1.xy = v1.xy * cb3[0].xy + cb3[0].zw;
//	r0.w = -1 + v3.z;
//	r0.w = cb3[34].x * r0.w + 1;
//	r1.xyz = -cb3[36].xyz + r0.xyz;
//	r1.w = dot(r1.xyz, r1.xyz);
//	r1.w = rsqrt(r1.w);
//	r1.xyz = r1.xyz * r1.www;
//	r2.xyz = cb1[1].xyz * v2.yyy;
//	r2.xyz = cb1[0].xyz * v2.xxx + r2.xyz;
//	r2.xyz = cb1[2].xyz * v2.zzz + r2.xyz;
//	r1.w = dot(r2.xyz, r1.xyz);
//	r1.w = cmp(r1.w >= 0);
//	r1.w = r1.w ? 1.000000 : 0;
//	r1.w = r1.w * 2 + -1;
//	r3.xyz = r1.xyz * r1.www;
//	r1.xyz = r1.xyz * r1.www + -r2.xyz;
//	r1.w = dot(r2.xyz, r3.xyz);
//	r0.w = abs(r1.w) * r0.w;
//	o2.xyz = r0.www * r1.xyz + r2.xyz;
//	o4.xyz = r2.xyz;
//	o3.xyzw = v3.xyzw;
//	o5.xyz = r0.xyz;
//	r0.x = -cb3[38].x + r0.y;
//	r0.x = saturate(r0.x / cb3[38].y);
//	r0.x = 1 + -r0.x;
//	o8.xyzw = cb3[39].xyzw * r0.xxxx;
//	r0.xyz = cb2[10].xyz * cb1[1].yyy;
//	r0.xyz = cb2[9].xyz * cb1[1].xxx + r0.xyz;
//	r0.xyz = cb2[11].xyz * cb1[1].zzz + r0.xyz;
//	r0.xyz = cb2[12].xyz * cb1[1].www + r0.xyz;
//	r0.xyz = v2.yyy * r0.xyz;
//	r1.xyz = cb2[10].xyz * cb1[0].yyy;
//	r1.xyz = cb2[9].xyz * cb1[0].xxx + r1.xyz;
//	r1.xyz = cb2[11].xyz * cb1[0].zzz + r1.xyz;
//	r1.xyz = cb2[12].xyz * cb1[0].www + r1.xyz;
//	r0.xyz = r1.xyz * v2.xxx + r0.xyz;
//	r1.xyz = cb2[10].xyz * cb1[2].yyy;
//	r1.xyz = cb2[9].xyz * cb1[2].xxx + r1.xyz;
//	r1.xyz = cb2[11].xyz * cb1[2].zzz + r1.xyz;
//	r1.xyz = cb2[12].xyz * cb1[2].www + r1.xyz;
//	o6.xyz = r1.xyz * v2.zzz + r0.xyz;
//	r0.x = cb3[3].x * -10 + 11;
//	o7.x = exp2(r0.x);
//	o7.yzw = float3(0, 0, 0);
//	return;
//}












// 픽셀
// Fri Aug 19 21:34:57 2022
//Texture2D<float4> t6 : register(t6);

//Texture2D<float4> t5 : register(t5);

//Texture2D<float4> t4 : register(t4);

//Texture2D<float4> t3 : register(t3);

//Texture2D<float4> t2 : register(t2);

//Texture2D<float4> t1 : register(t1);

//Texture2D<float4> t0 : register(t0);

//SamplerState s6_s : register(s6);

//SamplerState s5_s : register(s5);

//SamplerState s4_s : register(s4);

//SamplerState s3_s : register(s3);

//SamplerState s2_s : register(s2);

//SamplerState s1_s : register(s1);

//SamplerState s0_s : register(s0);

//cbuffer cb4 : register(b4)
//{
//	float4 cb4[38];
//}

//cbuffer cb3 : register(b3)
//{
//	float4 cb3[12];
//}

//cbuffer cb2 : register(b2)
//{
//	float4 cb2[1];
//}

//cbuffer cb1 : register(b1)
//{
//	float4 cb1[5];
//}

//cbuffer cb0 : register(b0)
//{
//	float4 cb0[7];
//}




////  declarations
//#define cmp -


//void main(
//  float4 v0 : SV_POSITION0,
//  float2 v1 : TEXCOORD0,
//  float w1 : TEXCOORD5,
//  float4 v2 : TEXCOORD7,
//  float4 v3 : TEXCOORD8,
//  float4 v4 : TEXCOORD1,
//  float4 v5 : TEXCOORD2,
//  float4 v6 : TEXCOORD3,
//  float4 v7 : TEXCOORD4,
//  float4 v8 : TEXCOORD10,
//  out float4 o0 : SV_Target0)
//{
//	float4 r0, r1, r2, r3, r4, r5, r6, r7;
//	uint4 bitmask, uiDest;
//	float4 fDest;

//	r0.xyzw = t1.Sample(s1_s, v1.xy).xyzw;
//	r0.z = cmp(r0.z < cb4[14].z);
//	if (r0.z != 0)
//		discard;

//	r1.xyzw = t0.Sample(s0_s, v1.xy).xyzw;
//	r1.xyz = cb0[2].xyz * r1.xyz;


//// 6. 선택적 마스크 맵의 전처리
//	r2.xyzw = t2.Sample(s2_s, v1.xy).xyzw;
//	r0.z = asint(cb4[1].x);
//	r2.xy = float2(-0, -0.5) + r2.yz;
//	r0.zw = r0.zz * r2.xy + float2(0, 0.5);


//// 7. 그림자 처리 - 픽셀 위치 -> 렌즈 위치 단위 벡터
// 시선 벡터 구하는 것 같다?
//	r2.xyz = cb1[4].xyz + -v5.xyz;
//	r2.w = dot(r2.xyz, r2.xyz);
//	r2.w = rsqrt(r2.w);
//	r2.xyz = r2.xyz * r2.www;


//// 8. 그림자 처리 - 조명 장치 벡터
// Directional Light의 단위 벡터 구하는 듯
//	r2.w = dot(cb2[0].xyz, cb2[0].xyz);
//	r2.w = rsqrt(r2.w);
//	r3.xyz = cb2[0].xyz * r2.www;


//// 9. 그림자 처리 - 자신의 조명 방향을 사용할지 여부를 결정
//	r2.w = dot(cb4[37].xyz, cb4[37].xyz);
//	r2.w = rsqrt(r2.w);
//	r3.w = asint(cb4[36].w);
//	r3.w = cmp(r3.w >= 1);
//	r3.w = r3.w ? 1.000000 : 0;
//	r4.xyz = cb4[37].xyz * r2.www + -r3.xyz;
//	r3.xyz = r3.www * r4.xyz + r3.xyz;


// 10. 그림자 처리 - 법선 방향과 빛 방향 사이의 각도의 cos 값을 계산
// 11. 그림자 처리 - 각도 cos 값 보정
//	r2.w = dot(v2.xyz, r3.xyz);			// N dot L
//	r3.w = 0.5 * r2.w;					// 하프 램버트?
//	r2.w = r2.w * 0.5 + 0.5;			// 하프 램버트?


// 12. 섀도우 핸들링 - 섀도우 레벨 얻기
//	r4.x = -cb4[4].x + cb4[3].w;
//	r0.x = -r0.x * r2.w + r4.x;
//	r0.x = r0.x / cb4[4].x;
//	r0.x = saturate(1 + r0.x);
//	r2.w = cmp(0 >= cb4[4].x);
//	r2.w = r2.w ? 1.000000 : 0;
//	r0.x = r2.w * -r0.x + r0.x;


// 13. 툰 섀도우 맵으로 작업하기
//	r4.xyzw = t3.Sample(s4_s, v1.xy).xyzw;
//	r4.xyz = cb0[2].xyz * r4.xyz;
//	r4.xyz = cb4[25].xyz * r4.xyz;


// 14. 광택 색상 조정
//	r2.w = 1 + -v3.w;
//	r2.w = cb4[37].w * r2.w;
//	r3.w = min(0, r3.w);
//	r3.w = 0.5 + r3.w;
//	r2.w = dot(r2.ww, r3.ww);
//	r2.w = 1 + -r2.w;
//	r3.w = cmp(0.5 >= cb4[6].w);
//	r4.w = r3.w ? 1.000000 : 0;
//	r4.w = r4.w * r2.w;
//	r5.xyz = float3(-1, -1, -1) + cb4[6].xyz;
//	r5.xyz = r4.www * r5.xyz + float3(1, 1, 1);
//	r6.xyz = r3.www ? float3(0, 0, 0) : cb4[6].xyz;
//	r6.xyz = r6.xyz * r2.www;
//	r1.xyz = r1.xyz * r5.xyz + r6.xyz;


// 15. 다크 사이드 맵 교체
//	r3.w = cmp(0.5 >= cb4[7].w);
//	r4.w = r3.w ? 1.000000 : 0;
//	r4.w = r4.w * r2.w;
//	r5.xyz = float3(-1, -1, -1) + cb4[7].xyz;
//	r5.xyz = r4.www * r5.xyz + float3(1, 1, 1);
//	r6.xyz = r3.www ? float3(0, 0, 0) : cb4[7].xyz;
//	r6.xyz = r6.xyz * r2.www;
//	r4.xyz = r4.xyz * r5.xyz + r6.xyz;
//	r4.xyz = r4.xyz + -r1.xyz;
//	r1.xyz = r0.xxx * r4.xyz + r1.xyz;


// 16. 먼지(더트) 수준 얻기...는 생략
//	r4.xyzw = t4.Sample(s5_s, v1.xy).xyzw;
//	r4.xyz = cb4[24].xxx * r4.xyz;
//	r2.w = cb4[19].x * r4.y;
//	r2.w = r4.x * cb4[18].x + r2.w;
//	r2.w = r4.z * cb4[20].x + r2.w;


// 18. 림 라이트 각 수정
//	r3.w = cmp(-0 >= cb4[10].w);
//	r3.w = r3.w ? 1 : -1;
//	r4.x = cb3[9].x;
//	r4.y = cb3[10].x;
//	r4.z = cb3[11].x;
//	r4.w = cmp(-0 >= cb4[11].x);
//	r4.w = r4.w ? 1 : -1;
//	r5.x = cb3[9].y;
//	r5.y = cb3[10].y;
//	r5.z = cb3[11].y;
//	r6.xyz = r4.xyz * r3.www + -r2.xyz;
//	r6.xyz = abs(cb4[10].www) * r6.xyz + r2.xyz;
//	r7.xyz = r5.xyz * r4.www + -r6.xyz;
//	r6.xyz = abs(cb4[11].xxx) * r7.xyz + r6.xyz;



//	r3.w = dot(r6.xyz, v4.xyz);
//	r4.w = cb4[8].x + -cb4[8].y;
//	r4.w = r4.w + -r3.w;
//	r4.w = r4.w / cb4[8].y;
//	r4.w = saturate(1 + r4.w);
//	r4.w = r4.w * r4.w;
//	r4.w = r4.w * r4.w;
//	r4.w = cb4[9].w * r4.w;
//	r4.w = r4.w * r0.w;
//	r6.xy = cmp(float2(-0, -0) >= cb4[13].yz);
//	r6.xy = r6.xy ? float2(1, 1) : float2(-1, -1);
//	r4.xyz = r4.xyz * r6.xxx + -r2.xyz;
//	r2.xyz = abs(cb4[13].yyy) * r4.xyz + r2.xyz;
//	r4.xyz = r5.xyz * r6.yyy + -r2.xyz;
//	r2.xyz = abs(cb4[13].zzz) * r4.xyz + r2.xyz;
//	r2.x = dot(r2.xyz, v4.xyz);
//	r2.y = cb4[11].y + -cb4[11].z;
//	r2.x = r2.y + -r2.x;
//	r2.x = r2.x / cb4[11].z;
//	r2.x = saturate(1 + r2.x);
//	r2.x = r2.x * r2.x;
//	r2.x = r2.x * r2.x;
//	r2.x = cb4[9].w * r2.x;
//	r0.w = r2.x * r0.w;


// 21. 반사광(스펙큘러) 레벨 계산
//	r2.x = max(0, r3.w);
//	r2.y = cb4[3].x * 10 + 1;
//	r2.y = max(0, r2.y);
//	r2.x = log2(r2.x);
//	r2.x = r2.y * r2.x;
//	r2.x = exp2(r2.x);
//	r2.x = min(1, r2.x);
//	r0.y = r2.x * r0.y;



//	r2.xyz = cb4[2].xyz * cb0[2].xyz;
//	r2.xyz = r2.xyz * r0.yyy;
//	r2.xyz = max(float3(0, 0, 0), r2.xyz);
//	r1.xyz = r2.xyz + r1.xyz;
//	r2.xyz = cb4[23].xyz + -cb4[21].xyz;
//	r2.xyz = r0.xxx * r2.xyz + cb4[21].xyz;
//	r2.xyz = r2.xyz + -r1.xyz;
//	r1.xyz = r2.www * r2.xyz + r1.xyz;


// 23. Ambient Light 및 환경 맵 처리
//	r0.xy = float2(1, 1) + v6.xy;
//	r0.xy = float2(0.5, 0.5) * r0.xy;
//	r5.xyzw = t5.Sample(s3_s, r0.xy).xyzw;
//	r2.xyz = r5.xyz * r1.xyz;
//	r0.x = cb4[3].y * r0.z;
//	r2.xyz = r2.xyz * cb4[3].zzz + -r1.xyz;
//	r0.xyz = r0.xxx * r2.xyz + r1.xyz;


// 24. 림 라이트의 추가 처리 및 추가(19 및 20 참조)
//	r1.xyz = cb4[9].xyz + -cb4[2].xyz;
//	r1.xyz = cb4[10].yyy * r1.xyz + cb4[2].xyz;
//	r2.x = dot(r3.xyz, v4.xyz);
//	r2.x = max(0, r2.x);
//	r1.xyz = r1.xyz * r4.www;
//	r2.y = cb4[10].x + r2.x;
//	r2.y = cb4[10].z + r2.y;
//	r1.xyz = r2.yyy * r1.xyz;
//	r2.yz = cb4[28].xy * r1.xy;
//	r0.xyz = r1.xyz * cb4[28].xyz + r0.xyz;



//	r3.xyz = cb4[12].xyz + -cb4[2].xyz;
//	r3.xyz = cb4[13].xxx * r3.xyz + cb4[2].xyz;
//	r3.xyz = r3.xyz * r0.www;
//	r0.w = cb4[13].w + r2.x;
//	r3.xyz = r3.xyz * r0.www;
//	r0.xyz = r3.xyz * cb4[28].xyz + r0.xyz;
//	r0.w = r2.y + r2.z;
//	r0.w = r1.z * cb4[28].z + r0.w;
//	r0.w = cmp(9.99999975e-06 >= r0.w);
//	r1.xyz = r0.www ? cb4[21].xyz : cb4[22].xyz;
//	r1.xyz = r1.xyz + -r0.xyz;
//	r0.xyz = r2.www * r1.xyz + r0.xyz;
//	r1.xyz = cb4[30].xyz * cb4[15].xyz;
//	r0.w = 1 + -r2.w;
//	r2.xyzw = t6.Sample(s6_s, v1.xy).xyzw;
//	r2.xyz = cb4[32].xyz * r2.xyz;
//	r2.xyz = r2.xyz * r0.www;
//	r0.xyz = r0.xyz * r1.xyz + r2.xyz;
//	r0.xyz = v8.xyz + r0.xyz;
//	r0.xyz = -cb0[6].xyz + r0.xyz;
//	r0.xyz = w1.xzz * r0.xyz + cb0[6].xyz;
//	r0.w = dot(r0.xyz, float3(0.212500006, 0.715399981, 0.0720999986));
//	r1.x = 1 + -cb4[16].x;
//	r0.xyz = cb4[16].xxx * r0.xyz;
//	o0.xyz = r0.www * r1.xxx + r0.xyz;
//	o0.w = r1.w;
//	return;
//}