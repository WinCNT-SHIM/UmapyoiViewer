/// <summary>
/// 버텍스와 컬러 정보만 가지고 있는 쉐이더
/// 
/// Created : 2022/07/04 심승수
/// Updated : 2022/07/04 심승수
/// </summary>

/// <summary>
/// Effect Framework
/// 특정한 렌더링 효과를 구현하는데 함께 쓰이는 셰이터 프로그램(함수)들과 렌더 스테이트를
/// '이펙트(Effect)'라고 부르는 단위로 조직화하고 관리하는 Framework를 제공하는일종의 편의용 코드 집합
/// 하나의 Effect는 적어도 하나의 버텍스 셰이더와 하나의 픽셀 셰이더,
/// 그리고 그 Effect를 구현하는데 필요한 렌더 스테이트로 구성된다
/// 
/// DX11에서 이펙트 프레임워크를 사용하기 위해서는 개별적인 헤더 파일 d3dx11Effect.h와
/// 라이브러리 Effects11.lib(디버그 빌드는 Effects11d.lib)가 필요하다
/// 
/// 버텍스/픽셀/기하 셰이더, 테셀레이션, 상수 버퍼 등의 셰이더 코드를
/// 이펙트 파일(확장자가 .fx)에 기술한다
/// </summary>

#include "CBPerFrame.fx"

/// <summary>
/// 상수 버퍼(constant buffer)
/// cbuffer는 constant buffer의 약자
/// 셰이더에서 접근 가능한 데이터 블록(전역 변수과 비슷)
/// </summary>
cbuffer cbPerObject : register(b1)
{
	float4x4 gWorldViewProj; 
};

struct VertexIn
{
    // L: 로컬 공간
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
    // H: 동차 절단 공간
	float4 PosH  : SV_POSITION;
    float4 Color : COLOR;
};

VertexOut VSPosColor(VertexIn vin)
{
    VertexOut vout;

    // Transform to homogeneous clip space.
    vout.PosH = mul(float4(vin.PosL, 1.0f), gWorldViewProj);

    // Just pass vertex color into the pixel shader.
    vout.Color = vin.Color;

    return vout;
}

float4 PSPosColor(VertexOut pin) : SV_Target
{
	// The toEye vector is used in lighting.
	float3 toEye = gCameraInfo.gEyePosW;

	// Cache the distance to the eye from this surface point.
	float distToEye = length(toEye);

	// Normalize.
	toEye /= distToEye;

	return pin.Color;
}

/// <summary>
/// 테크닉에 사용할 렌더 스테이트의 래스터라이저 설정
/// 'D3D11_FILL_' 접두사를 붙이지 않는다
/// </summary>
//RasterizerState WireframeRS
//{
//	FillMode = WIREFRAME;
//	CullMode = Back;
//	FrontCounterClockwise = false;
//}

/// <summary>
/// 기법(technique)과 패스(pass)의 정의
/// 이펙트 파일에는 technique11로 시작하는 섹션으로 하나 이상의 기법(technique)를 정의한다
/// 하나의 테크닉은 특정 렌더링 테크닉을 구현하는 데 쓰이는 하나 이상의 패스로 구성된다
/// 각 패스마다 기하구조를 다른 방식으로 렌더링하고,
/// 각 패스의 결과를 특정 방식으로 결합해서 원하는 결과를 얻는다
/// 
/// 패스(Pass)는 pass로 시작하는 섹션에 정의한다(하나의 섹션에 하나의 패스를 정의함)
/// 하나의 패스는 하나의 정점 셰이더, 픽셀 셰이더, 렌더 스테이트로 구성되며,
/// 추가적으로 기하 셰이더와 테셀레이션 관련 셰이더들을 포함시킬 수 있다
/// 
/// 팁) 렌더 스테이트의 경우, 가변적으로 작성해야 할 때에는
/// 이펙트 파일보다는 어플리케이션에서 설정하는 것이 더 나을 수도 있음
/// </summary>
technique11 PosColorTech
{
    pass P0
    {
		SetVertexShader(CompileShader(vs_5_0, VSPosColor()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_5_0, PSPosColor()));
		//SetRasterizerState( WireframeRS ); // 렌더 스테이트 작성 예제
	}
}
