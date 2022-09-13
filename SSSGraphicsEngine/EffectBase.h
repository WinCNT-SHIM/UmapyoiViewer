#pragma once

#include "GraphicsRenderingData.h"

namespace GraphicsEngine
{
	class EffectBase
	{
	public:
		EffectBase() = delete;
		EffectBase(std::shared_ptr<DXDevice> device, string filename, tstring effectName);
		virtual ~EffectBase();

	private:
		EffectBase(const EffectBase& rhs);
		EffectBase& operator=(const EffectBase& rhs) = delete;

	protected:
		// 이펙트 객체
		// 하나의 Effect는 적어도 하나의 버텍스 셰이더와 하나의 픽셀 셰이더,
		// 그리고 그 Effect를 구현하는데 필요한 렌더 스테이트로 구성된다
		ComPtr<ID3DX11Effect> m_FX;

		// 기법(Technique)에 대한 포인터
		ComPtr<ID3DX11EffectTechnique> m_Tech;

		// 캐싱용
		ComPtr<ID3D11DeviceContext> m_DeviceContext;

		// 이펙트의 이름
		tstring m_Name;

		// 인풋 레이아웃
		ComPtr<ID3D11InputLayout> m_InputLayout;

		// 상수 버퍼
		//ID3DX11EffectConstantBuffer* m_CBPerFrame;
		ComPtr<ID3DX11EffectConstantBuffer> m_CBPerFrame;

	public:
		tstring GetName() const { return m_Name; }
		ComPtr<ID3DX11EffectConstantBuffer> GetCBPerFrame() { return m_CBPerFrame; }
		void SetCBPerFrame(ComPtr<ID3DX11EffectConstantBuffer> cbPerFrame);

	public:
		virtual void UpdatePerFrameData(std::shared_ptr<PerFrameData>& m_PerFrameData) abstract;
		virtual void EffectRendering(std::shared_ptr<RenderingData>& renderingData) abstract;

	public:
		virtual void Release();
	};
}