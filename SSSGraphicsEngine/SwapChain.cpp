#include "GraphicsEnginePCH.h"
#include "SwapChain.h"

namespace GraphicsEngine
{
	void SwapChain::Init(HWND hWnd, std::shared_ptr<DXDevice> device, ScreenInfo& screenInfo)
	{
		m_hWnd = hWnd;

		// ==================== 4X MSAA 품질 수준 지원을 체크한다 ====================
		HR(device->GetDevice()->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM,		// 텍스처 형식
			4,								// 다중 샘플링 중 샘플 수(4X이므로 4를 지정)
			&screenInfo.m_4xMsaaQuality				// 어댑터에서 지원하는 품질 수준의 수
		));
		assert(screenInfo.m_4xMsaaQuality > 0); // 반환된 품질 수는 0보다 커야해서 assert로 강제

		//this->OnResize(device, screenInfo);
	}

	void SwapChain::Release()
	{
		//ReleaseCOM(m_SwapChain);
		m_SwapChain.ReleaseAndGetAddressOf();
	}

	void SwapChain::OnResize(const ComPtr<ID3D11Device>& device, ScreenInfo& screenInfo)
	{
		// 초기화 느낌의 해제
		this->Release();

		// ==================== DXGI_SWAP_CHAIN_DESC 구조체를 설정한다 ====================
		// DXGI_SWAP_CHAIN_DESC 구조체의 인스턴스를 만들어서 원하는 교환 사슬의 특성들을 설정
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		// DXGI_MODE_DESC BufferDesc: 후면 버퍼의 속성들을 서술하는 개별적인 구조체
		// 창의 클라이언트 영역의 크기(0이면 출력 창 크기로 설정)
		sd.BufferDesc.Width = screenInfo.m_ScreenWidth;
		sd.BufferDesc.Height = screenInfo.m_ScreenHeight;
		// 디스플레이 모드갱신율
		sd.BufferDesc.RefreshRate.Numerator = 60;	// 최고 유리수
		sd.BufferDesc.RefreshRate.Denominator = 1;	// 최저 유리수
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// 후면 버퍼 픽셀 형식
		// 디스플레이 스캔라인 모드
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // 미지정
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// 미지정
		// 4X MSAA 사용 여부 체크
		if (screenInfo.m_Enable4xMsaa)
		{
			// 픽셀당 멀티샘플 수
			sd.SampleDesc.Count = 4;
			// 원하는 품질 수준 지정
			// 품질 수준의 범위는 텍스처 형식과 픽셀당 샘플 수에 의존한다
			// 유요한 품질 수준의 범위는 0에서 CheckMultisampleQualityLevels로 얻은 값 -1까지이다
			sd.SampleDesc.Quality = screenInfo.m_4xMsaaQuality - 1;
		}
		else
		{
			sd.SampleDesc.Count = 1;	// 픽셀당 멀티샘플 수
			sd.SampleDesc.Quality = 0;	// 이미지 품질 수준
		}
		// 후면 버퍼에 대한 사용 용도 설정(셰이더 입력, 혹은 렌더 타겟 출력)
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// 후면 버퍼를 Render Target으로 사용
		// Swap Chain에서 사용할 후면 버퍼의 개수
		sd.BufferCount = 1; //(1: 이중 버퍼링, 2: 삼중 버퍼링ㄴ)
		// 렌더링 결과를 표시할 윈도우의 핸들
		sd.OutputWindow = m_hWnd;
		// 창 모드, 전체 화면 모드 설정
		sd.Windowed = true;
		// 표시한 후, 전면 버퍼의 내용을 처리하기 위한 옵션
		// DXGI_SWAP_EFFECT_DISCARD: 디스플레이 드라이버가 Swap Chain에 대해 가장 효율적인 제시 기술을 선택함
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 레거시 코드지만 사용...
		// 스왑 체인 동작에 대한 추가 옵션
		// 0: 창 => 전체 화면 시, 현재 데스크톱 디스플레이 모드(해상도?)가 사용
		// DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH: 창 => 전체 화면 시, 후면 버퍼 설정에 부합하는 모드가 자동 선택
		sd.Flags = 0;


		// ==================== Swap Chain를 생성한다 ====================
		// IDXGIFactory 인터페이스의 CreateSwapChain 함수로 Swap Chain 인터페이스를 생성한다
		// Swap Chain을 만들기 위해서는 장치 생성에 사용된 IDXGIFactory 인스턴스를 사용해야 한다
		// 다음은 장치 생성에 사용된 IDXGIFactory 인스턴스를 얻기 위한 일련의 과정이다
		IDXGIDevice* _DxgiDevice = 0;
		HR(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&_DxgiDevice));
		IDXGIAdapter* _DxgiAdapter = 0;
		HR(_DxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&_DxgiAdapter));
		IDXGIFactory* _DxgiFactory = 0;
		HR(_DxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&_DxgiFactory)); // IDXGIFactory 인스턴스

		// Swap Chain을 생성한다
		HR(_DxgiFactory->CreateSwapChain(device.Get(), &sd, &m_SwapChain));

		// 사용한 COM 인터페이스을 해제한다
		ReleaseCOM(_DxgiDevice);
		ReleaseCOM(_DxgiAdapter);
		ReleaseCOM(_DxgiFactory);
	}

	void SwapChain::Present()
	{
		assert(m_SwapChain);

		// todo: Present에 의한 프레임 제어는 일단 생각하지 말자
		HR(m_SwapChain->Present(0, 0));
	}
}