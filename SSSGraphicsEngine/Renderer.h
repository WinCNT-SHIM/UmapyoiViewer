/// <summary>
/// ���� �׸��� ó���� ����ϴ� Ŭ����
/// DX11GraphicsEngine���� ó���Ϸ��� ������ �������̽��� ����� �и��ϰ� ��...
/// 
/// Created : 2022/08/05 �ɽ¼�
/// Updated : 2022/08/05 �ɽ¼�
/// </summary>
#pragma once

#include "RenderStruct.h"

#define  _DX11RENDERER
#include "IDefine.h"

class RenderingData;
class PerFrameData;

namespace GraphicsEngine
{
	class DXDevice;
	class SwapChain;
	class RenderTarget;
	class ForwardRenderingPass;

	class Renderer
	{
	public:
		Renderer();
		~Renderer();
		Renderer(const Renderer& other) = delete;
		Renderer& operator= (const Renderer& other) = delete;
		Renderer(Renderer&& other) = delete;
	private:
		static std::shared_ptr<Renderer> m_Instance;
	public:
		static std::shared_ptr<GraphicsEngine::Renderer> GetInstance()
		{
			if (m_Instance == nullptr)
			{
				m_Instance = std::make_shared<GraphicsEngine::Renderer>();
			}
			return m_Instance;
		}

	private:
		// ���� �� �μ��� ����
		HWND m_hWnd;
		// ��ũ�� ũ��, MSAA ���� ����
		ScreenInfo m_ScreenInfo;
		// D3DX ����̽��� ����̽� ���ؽ�Ʈ�� ���� �� ����
		std::shared_ptr<DXDevice> m_D3DXDevice;
		// D3DX�� ���� ü���� ���� �� ����
		std::shared_ptr<SwapChain> m_SwapChain;
		// ����ۿ� ���� Ÿ�� ��� ���� ���ٽ� ���� ���� �� ����
		std::shared_ptr<RenderTarget> m_RenderTarget;

		// Post Processing ��� ����
		bool m_UsingPostProcessing;

	private:
		std::shared_ptr<PerFrameData> m_PerFrameData;
		// �̸��� ť���� clear�� �����ؼ� ���ͷ� �ٲ�
		std::vector<std::shared_ptr<RenderingData>> m_RenderQueue;

		// ������ �н�
		std::shared_ptr<ForwardRenderingPass> m_ForwardRenderingPass;

	//////////////////////////////////////////////////////////////////////////
	// Getter, Setter
	//////////////////////////////////////////////////////////////////////////
	public:
		std::shared_ptr<DXDevice> GetDXDevice() { return m_D3DXDevice; }
		ComPtr<ID3D11Device> GetDevice() { return m_D3DXDevice->GetDevice(); }
		ComPtr<ID3D11DeviceContext> GetDeviceContext() { return m_D3DXDevice->GetDeviceContext(); }
		std::shared_ptr<SwapChain> GetSwapChain() { return m_SwapChain; }
		std::shared_ptr<RenderTarget> GetRenderTarget() { return m_RenderTarget; }

		void SetUsingPostProcessing(bool val) { m_UsingPostProcessing = val; }

	//////////////////////////////////////////////////////////////////////////
	// ��� �Լ�
	//////////////////////////////////////////////////////////////////////////
	private:
		void ForwardProcessing();
		void PostProcessing();

	//////////////////////////////////////////////////////////////////////////
	// �Ϲ� �Լ�
	//////////////////////////////////////////////////////////////////////////
	public:
		void Init(HWND hWnd, int screenWidth, int screenHeight);
		void InitRenderingPass();
		void Release();

		void Update(float deltaTime);

		void BeginRender();
		// ������ ť�� �����͸� �����ϴ� �Լ�
		void PostPerFrameData(std::shared_ptr<PerFrameData> perFrameData);
		void PostRenderingData(std::shared_ptr<RenderingData> renderingData);
		// ������ ť�� �����͸� �׸��� ó���ϰ� �ʱ�ȭ�ϴ� �Լ�
		void Render();
		void PostProcess();
		void EndRender();

		void OnResize(const int& screenWidth, const int& screenHeight);

		bool IsValidDevice();
		//void SetClientSize(int width, int height);
		//void SetWinMinMax(bool isMinimized, bool isMaxmized);
	};
}