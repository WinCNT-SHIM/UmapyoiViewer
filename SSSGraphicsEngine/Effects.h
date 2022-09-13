/// <summary>
/// FX의 Effect들을 관리하는 파일
/// 새로운 FX이 추가되면 Effect 클래스를 상속받은
/// 새로운 개별 Effect 클래스를 만들어야 한다(...)
/// 
/// Created : 2022/06/29 심승수
/// Updated : 2022/07/02 심승수
/// </summary>

#ifndef EFFECTS_H
#define EFFECTS_H

#define  _DX11RENDERER
#include "IDefine.h"

namespace GraphicsEngine
{
#pragma region Effect
	class Effect
	{
	public:
		Effect(shared_ptr<DXDevice> device, string filename);
		virtual ~Effect();

	private:
		Effect(const Effect& rhs);
		Effect& operator=(const Effect& rhs);

	protected:
		// 이펙트 객체
		// 하나의 Effect는 적어도 하나의 버텍스 셰이더와 하나의 픽셀 셰이더,
		// 그리고 그 Effect를 구현하는데 필요한 렌더 스테이트로 구성된다
		ID3DX11Effect* m_FX;
	};
#pragma endregion Effect


#pragma region EffectImpl
#pragma region PosColorEffect
	/// <summary>
	/// POSITION, COLOR 이펙트(셰이더 프로그램)를 다루기 위한 클래스
	/// </summary>
	//class PosColorEffect : public Effect
	//{
	//public:
	//	PosColorEffect(shared_ptr<DXDevice> device, string m_FilePath);
	//	~PosColorEffect() {};

	//	// ==================== 셰이더의 상수 버퍼와 소통하기 위한 함수 ====================
	//	void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	//	// 기법(Technique)에 대한 포인터
	//	ComPtr<ID3DX11EffectTechnique> PosColorTech;
	//	// 셰이더의 상수 버퍼에 대한 포인터
	//	ComPtr<ID3DX11EffectMatrixVariable> WorldViewProj;
	//};
#pragma endregion PosColorEffect

#pragma region WireEffect

	class WireEffect : public Effect
	{
	public:
		WireEffect(shared_ptr<DXDevice> device, string m_FilePath);
		~WireEffect() {};

	private:
		// 상수 버퍼(행렬)
		ComPtr<ID3DX11EffectMatrixVariable> WorldViewProj;
		ComPtr<ID3DX11EffectVectorVariable> WireColor;

	public:
		ComPtr<ID3DX11EffectTechnique> ColorTech;

		void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetWireColor(const XMFLOAT4& v) { WireColor->SetRawValue(&v, 0, sizeof(XMFLOAT4)); }
	};
#pragma endregion WireEffect
#pragma endregion EffectImpl


#pragma region Effects
	class Effects
	{
	public:
		static void InitAll(shared_ptr<DXDevice> device);
		static void FinalizeAll();

		//static BasicEffect* BasicFX;
		//static LightTexEffect* BasicTexFX;
		//static PPEffect* PPFX;
		//static PosColorEffect* PosColorFX;
		static WireEffect* WireFX;
	};
#pragma endregion

}
#endif // EFFECTS_H