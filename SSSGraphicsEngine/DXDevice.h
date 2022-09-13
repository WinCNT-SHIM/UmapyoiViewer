/// <summary>
/// 디바이스와 디바이스 컨텍스트를 생성, 관리하는 클래스
/// 
/// Created : 2022/06/29 심승수
/// Updated : 2022/07/02 심승수
/// </summary>
#pragma once

#define  _DX11RENDERER
#include "IDefine.h"


namespace GraphicsEngine
{
	class RenderTarget;

	class DXDevice
	{
	public:
		DXDevice() = default;
		virtual ~DXDevice();

	public:
		void Init();
		void Release();
		bool IsValid() { return (m_D3DXDevice != nullptr); }

		// 렌더 타겟(1개)을 설정
		void SetRenderTargets(std::shared_ptr<RenderTarget> renderTarget);
		// 렌더 타겟 배열(n개)을 설정
		void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget>> renderTargets);
		// 뷰포트를 설정(동접 큐브맵에서 사용됨)
		void SetSetViewports(ScreenInfo& screenInfo);

		ComPtr<ID3D11Device> GetDevice() const { return m_D3DXDevice; }
		ComPtr<ID3D11DeviceContext> GetDeviceContext() const { return m_D3DXDeviceContext; }

	private:
		ComPtr<ID3D11Device> m_D3DXDevice;
		ComPtr<ID3D11DeviceContext> m_D3DXDeviceContext;

		D3D_FEATURE_LEVEL m_FeatureLevel;
	};
}