/// <summary>
/// ���ؽ��� �÷� ������ ������ �ִ� ���̴�
/// 
/// Created : 2022/07/04 �ɽ¼�
/// Updated : 2022/07/04 �ɽ¼�
/// </summary>

/// <summary>
/// Effect Framework
/// Ư���� ������ ȿ���� �����ϴµ� �Բ� ���̴� ������ ���α׷�(�Լ�)��� ���� ������Ʈ��
/// '����Ʈ(Effect)'��� �θ��� ������ ����ȭ�ϰ� �����ϴ� Framework�� �����ϴ������� ���ǿ� �ڵ� ����
/// �ϳ��� Effect�� ��� �ϳ��� ���ؽ� ���̴��� �ϳ��� �ȼ� ���̴�,
/// �׸��� �� Effect�� �����ϴµ� �ʿ��� ���� ������Ʈ�� �����ȴ�
/// 
/// DX11���� ����Ʈ �����ӿ�ũ�� ����ϱ� ���ؼ��� �������� ��� ���� d3dx11Effect.h��
/// ���̺귯�� Effects11.lib(����� ����� Effects11d.lib)�� �ʿ��ϴ�
/// 
/// ���ؽ�/�ȼ�/���� ���̴�, �׼����̼�, ��� ���� ���� ���̴� �ڵ带
/// ����Ʈ ����(Ȯ���ڰ� .fx)�� ����Ѵ�
/// </summary>

#include "CBPerFrame.fx"

/// <summary>
/// ��� ����(constant buffer)
/// cbuffer�� constant buffer�� ����
/// ���̴����� ���� ������ ������ ���(���� ������ ���)
/// </summary>
cbuffer cbPerObject : register(b1)
{
	float4x4 gWorldViewProj; 
};

struct VertexIn
{
    // L: ���� ����
	float3 PosL  : POSITION;
    float4 Color : COLOR;
};

struct VertexOut
{
    // H: ���� ���� ����
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
/// ��ũ�п� ����� ���� ������Ʈ�� �����Ͷ����� ����
/// 'D3D11_FILL_' ���λ縦 ������ �ʴ´�
/// </summary>
//RasterizerState WireframeRS
//{
//	FillMode = WIREFRAME;
//	CullMode = Back;
//	FrontCounterClockwise = false;
//}

/// <summary>
/// ���(technique)�� �н�(pass)�� ����
/// ����Ʈ ���Ͽ��� technique11�� �����ϴ� �������� �ϳ� �̻��� ���(technique)�� �����Ѵ�
/// �ϳ��� ��ũ���� Ư�� ������ ��ũ���� �����ϴ� �� ���̴� �ϳ� �̻��� �н��� �����ȴ�
/// �� �н����� ���ϱ����� �ٸ� ������� �������ϰ�,
/// �� �н��� ����� Ư�� ������� �����ؼ� ���ϴ� ����� ��´�
/// 
/// �н�(Pass)�� pass�� �����ϴ� ���ǿ� �����Ѵ�(�ϳ��� ���ǿ� �ϳ��� �н��� ������)
/// �ϳ��� �н��� �ϳ��� ���� ���̴�, �ȼ� ���̴�, ���� ������Ʈ�� �����Ǹ�,
/// �߰������� ���� ���̴��� �׼����̼� ���� ���̴����� ���Խ�ų �� �ִ�
/// 
/// ��) ���� ������Ʈ�� ���, ���������� �ۼ��ؾ� �� ������
/// ����Ʈ ���Ϻ��ٴ� ���ø����̼ǿ��� �����ϴ� ���� �� ���� ���� ����
/// </summary>
technique11 PosColorTech
{
    pass P0
    {
		SetVertexShader(CompileShader(vs_5_0, VSPosColor()));
		SetGeometryShader( NULL );
		SetPixelShader(CompileShader(ps_5_0, PSPosColor()));
		//SetRasterizerState( WireframeRS ); // ���� ������Ʈ �ۼ� ����
	}
}
