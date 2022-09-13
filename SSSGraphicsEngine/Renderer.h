/// <summary>
/// 직접 그리기 처리를 담당하는 클래스
/// DX11GraphicsEngine에서 처리하려고 했지만 인터페이스의 관계로 분리하게 됨...
/// 
/// Created : 2022/08/05 심승수
/// Updated : 2022/08/05 심승수
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
		// 생성 시 인수를 보관
		HWND m_hWnd;
		// 스크린 크기, MSAA 관련 정보
		ScreenInfo m_ScreenInfo;
		// D3DX 디바이스와 디바이스 컨텍스트를 생성 및 관리
		std::shared_ptr<DXDevice> m_D3DXDevice;
		// D3DX의 스왑 체인을 생성 및 관리
		std::shared_ptr<SwapChain> m_SwapChain;
		// 백버퍼용 렌더 타겟 뷰와 깊이 스텐실 뷰의 생성 및 관리
		std::shared_ptr<RenderTarget> m_RenderTarget;

		// Post Processing 사용 여부
		bool m_UsingPostProcessing;

	private:
		std::shared_ptr<PerFrameData> m_PerFrameData;
		// 이름은 큐지만 clear가 복잡해서 벡터로 바꿈
		std::vector<std::shared_ptr<RenderingData>> m_RenderQueue;

		// 렌더링 패스
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
	// 멤버 함수
	//////////////////////////////////////////////////////////////////////////
	private:
		void ForwardProcessing();
		void PostProcessing();

	//////////////////////////////////////////////////////////////////////////
	// 일반 함수
	//////////////////////////////////////////////////////////////////////////
	public:
		void Init(HWND hWnd, int screenWidth, int screenHeight);
		void InitRenderingPass();
		void Release();

		void Update(float deltaTime);

		void BeginRender();
		// 렌더링 큐에 데이터를 보관하는 함수
		void PostPerFrameData(std::shared_ptr<PerFrameData> perFrameData);
		void PostRenderingData(std::shared_ptr<RenderingData> renderingData);
		// 렌더링 큐의 데이터를 그리기 처리하고 초기화하는 함수
		void Render();
		void PostProcess();
		void EndRender();

		void OnResize(const int& screenWidth, const int& screenHeight);

		bool IsValidDevice();
		//void SetClientSize(int width, int height);
		//void SetWinMinMax(bool isMinimized, bool isMaxmized);
	};
}