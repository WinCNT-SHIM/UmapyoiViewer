/// <summary>
/// 
/// 
/// 
/// Created: 2022/06/27 심승수
/// Updated: 2022/06/27 심승수
/// </summary>
#pragma once

#define  _DX11RENDERER
#include "IDefine.h"

class ScreenInfo;

namespace GraphicsEngine
{
	class RenderTarget
	{
	public:
		void Init(shared_ptr<DXDevice> device, shared_ptr<SwapChain> swapChain);
		void Release();

	public:
		void SetDevice(shared_ptr<DXDevice> device, shared_ptr<SwapChain> swapChain);

		/// 윈도우 사이즈 변경 시 실행되는 함수(RTV, DSV 등이 실제로 생성되는 곳)
		void OnResize(ScreenInfo& screenInfo);

		/// 후면 버퍼와 깊이 스텐실 버퍼를 클리어하는 함수
		void Clear();

		ComPtr<ID3D11RenderTargetView> GetRTV() const { return m_RenderTargetView; }
		ComPtr<ID3D11DepthStencilView> GetDSV() const { return m_DepthStencilView; }

	private:
		// 디바이스를 가지고 있자
		weak_ptr<DXDevice> m_D3DXDevice;
		// 스왑 체인을 가지고 있자
		weak_ptr<SwapChain> m_SwapChain;

		/// 렌더 타겟 관련
		// 렌더 타겟의 실제 텍스처(사용하는 일은 없음)
		ComPtr<ID3D11Texture2D> m_RenderTargetTexture;
		// 실제로 핸들링하는 렌터 타겟 뷰
		ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
		// 
		ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;
		
		/// 깊이 스텐실(뎁스 스텐실) 관련
		/// 1대1 대응을 위해 렌터 타겟은 각각의 깊이 스텐일 텍스처를 같는다
		// 깊이 스텐실에 대한 실제 텍스처(사용하는 일은 없음)
		ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
		// 실제로 핸들링하는 깊이 스텐실 뷰
		ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
		// 깊이 상태 값 설정
		ComPtr<ID3D11DepthStencilState> m_NormalDSS;
	};
}
