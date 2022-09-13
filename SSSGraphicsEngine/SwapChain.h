/// <summary>
/// ���� ü���� ����, �����ϴ� Ŭ����
/// 
/// Created : 2022/06/29 �ɽ¼�
/// Updated : 2022/06/29 �ɽ¼�
/// </summary>
#pragma once

#define  _DX11RENDERER
#include "IDefine.h"

namespace GraphicsEngine
{
	class SwapChain
	{
	public:
		void Init(HWND hWnd, std::shared_ptr<DXDevice> device, ScreenInfo& screenInfo);
		void Release();
		bool IsValid() { return (m_SwapChain != nullptr); }

		//ComPtr<ID3D11RenderTargetView> GetBackBufferRenderTarget() { return m_pRenderTargetView; }

		ComPtr<IDXGISwapChain> GetSwapChain() const { return m_SwapChain; }

		void OnResize(const ComPtr<ID3D11Device>& device, ScreenInfo& screenInfo);
		void Present();

	private:
		HWND m_hWnd;
		// DX�� Swap Cahin
		ComPtr<IDXGISwapChain> m_SwapChain;
	};
}
