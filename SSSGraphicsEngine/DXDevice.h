/// <summary>
/// ����̽��� ����̽� ���ؽ�Ʈ�� ����, �����ϴ� Ŭ����
/// 
/// Created : 2022/06/29 �ɽ¼�
/// Updated : 2022/07/02 �ɽ¼�
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

		// ���� Ÿ��(1��)�� ����
		void SetRenderTargets(std::shared_ptr<RenderTarget> renderTarget);
		// ���� Ÿ�� �迭(n��)�� ����
		void SetRenderTargets(std::vector<std::shared_ptr<RenderTarget>> renderTargets);
		// ����Ʈ�� ����(���� ť��ʿ��� ����)
		void SetSetViewports(ScreenInfo& screenInfo);

		ComPtr<ID3D11Device> GetDevice() const { return m_D3DXDevice; }
		ComPtr<ID3D11DeviceContext> GetDeviceContext() const { return m_D3DXDeviceContext; }

	private:
		ComPtr<ID3D11Device> m_D3DXDevice;
		ComPtr<ID3D11DeviceContext> m_D3DXDeviceContext;

		D3D_FEATURE_LEVEL m_FeatureLevel;
	};
}