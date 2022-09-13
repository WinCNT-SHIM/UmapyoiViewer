/// <summary>
/// FX�� Effect���� �����ϴ� ����
/// ���ο� FX�� �߰��Ǹ� Effect Ŭ������ ��ӹ���
/// ���ο� ���� Effect Ŭ������ ������ �Ѵ�(...)
/// 
/// Created : 2022/06/29 �ɽ¼�
/// Updated : 2022/07/02 �ɽ¼�
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
		// ����Ʈ ��ü
		// �ϳ��� Effect�� ��� �ϳ��� ���ؽ� ���̴��� �ϳ��� �ȼ� ���̴�,
		// �׸��� �� Effect�� �����ϴµ� �ʿ��� ���� ������Ʈ�� �����ȴ�
		ID3DX11Effect* m_FX;
	};
#pragma endregion Effect


#pragma region EffectImpl
#pragma region PosColorEffect
	/// <summary>
	/// POSITION, COLOR ����Ʈ(���̴� ���α׷�)�� �ٷ�� ���� Ŭ����
	/// </summary>
	//class PosColorEffect : public Effect
	//{
	//public:
	//	PosColorEffect(shared_ptr<DXDevice> device, string m_FilePath);
	//	~PosColorEffect() {};

	//	// ==================== ���̴��� ��� ���ۿ� �����ϱ� ���� �Լ� ====================
	//	void SetWorldViewProj(CXMMATRIX M) { WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	//	// ���(Technique)�� ���� ������
	//	ComPtr<ID3DX11EffectTechnique> PosColorTech;
	//	// ���̴��� ��� ���ۿ� ���� ������
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
		// ��� ����(���)
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