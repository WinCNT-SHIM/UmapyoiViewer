/// <summary>
/// 
/// 
/// 
/// Created: 2022/06/27 �ɽ¼�
/// Updated: 2022/06/27 �ɽ¼�
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

		/// ������ ������ ���� �� ����Ǵ� �Լ�(RTV, DSV ���� ������ �����Ǵ� ��)
		void OnResize(ScreenInfo& screenInfo);

		/// �ĸ� ���ۿ� ���� ���ٽ� ���۸� Ŭ�����ϴ� �Լ�
		void Clear();

		ComPtr<ID3D11RenderTargetView> GetRTV() const { return m_RenderTargetView; }
		ComPtr<ID3D11DepthStencilView> GetDSV() const { return m_DepthStencilView; }

	private:
		// ����̽��� ������ ����
		weak_ptr<DXDevice> m_D3DXDevice;
		// ���� ü���� ������ ����
		weak_ptr<SwapChain> m_SwapChain;

		/// ���� Ÿ�� ����
		// ���� Ÿ���� ���� �ؽ�ó(����ϴ� ���� ����)
		ComPtr<ID3D11Texture2D> m_RenderTargetTexture;
		// ������ �ڵ鸵�ϴ� ���� Ÿ�� ��
		ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
		// 
		ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;
		
		/// ���� ���ٽ�(���� ���ٽ�) ����
		/// 1��1 ������ ���� ���� Ÿ���� ������ ���� ������ �ؽ�ó�� ���´�
		// ���� ���ٽǿ� ���� ���� �ؽ�ó(����ϴ� ���� ����)
		ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
		// ������ �ڵ鸵�ϴ� ���� ���ٽ� ��
		ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
		// ���� ���� �� ����
		ComPtr<ID3D11DepthStencilState> m_NormalDSS;
	};
}
