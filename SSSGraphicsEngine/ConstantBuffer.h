#pragma once

#include "EffectBase.h"

namespace GraphicsEngine
{
	class ConstantBuffer
		: public EffectBase
	{
	public:
		ConstantBuffer(std::shared_ptr<DXDevice> device, string m_FilePath);
		virtual ~ConstantBuffer();

	private:
		//////////////////////////////////////////////////////////////////////////
		// 상수 버퍼
		//////////////////////////////////////////////////////////////////////////
		// 셰이더의 상수 버퍼에 대한 포인터
		// Per Frame Data
		//ComPtr<ID3DX11EffectVariable> m_CameraInfo;
		//ComPtr<ID3DX11EffectVariable> m_DirLightCount;
		//ComPtr<ID3DX11EffectVariable> m_DirLights;
		//ComPtr<ID3DX11EffectVariable> m_SpotLightCount;
		//ComPtr<ID3DX11EffectVariable> m_SpotLights;
		//ComPtr<ID3DX11EffectVariable> m_PointLightCount;
		//ComPtr<ID3DX11EffectVariable> m_PointLights;

	private:
		//ID3DX11EffectConstantBuffer* m_ConstantBuffer;
		//ComPtr<ID3DX11EffectConstantBuffer> GetConstantBuffer() { return m_CBPerFrame; }
		//ID3DX11EffectConstantBuffer* GetConstantBuffer() { return m_CBPerFrame; }

	private:
		// ==================== 셰이더의 상수 버퍼와 소통하기 위한 함수 ====================
	public:
		virtual void UpdatePerFrameData(std::shared_ptr<PerFrameData>& m_PerFrameData) override;
		virtual void EffectRendering(std::shared_ptr<RenderingData>& renderingData) override {}
		virtual void Release();
	};
}
