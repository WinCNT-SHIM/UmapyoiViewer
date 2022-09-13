#include "GraphicsEnginePCH.h"
#include "Renderer.h"

#include "RenderTarget.h"
#include "Effects.h"
#include "InputLayouts.h"

// 매니저
#include "ResourceManager.h"

// 렌더링 데이터
#include "GraphicsRenderingData.h"

// 렌더링 패스
#include "ForwardRenderingPass.h"

using namespace GraphicsEngine;

// 전역 변수
std::shared_ptr<GraphicsEngine::Renderer> GraphicsEngine::Renderer::m_Instance = nullptr;

Renderer::Renderer()
	: m_hWnd(nullptr)
	, m_ScreenInfo{}
	, m_D3DXDevice(make_shared<DXDevice>())
	, m_SwapChain(make_shared<SwapChain>())
	, m_RenderTarget(make_shared<RenderTarget>())
	, m_UsingPostProcessing(false)
{

}

void Renderer::InitRenderingPass()
{
	m_ForwardRenderingPass = std::make_shared<ForwardRenderingPass>();
	m_ForwardRenderingPass->Init(m_ScreenInfo.m_ScreenWidth, m_ScreenInfo.m_ScreenHeight);
}

void Renderer::ForwardProcessing()
{
	// 포워드 프로세싱!!
	m_ForwardRenderingPass->Render(m_PerFrameData, m_RenderQueue);
}

void Renderer::PostProcessing()
{

}


Renderer::~Renderer()
{
	
}

void Renderer::Init(HWND hWnd, int screenWidth, int screenHeight)
{
	m_hWnd = hWnd;

	// 스크린 정보를 설정하자
	m_ScreenInfo.m_ScreenWidth = screenWidth;
	m_ScreenInfo.m_ScreenHeight = screenHeight;
	m_ScreenInfo.m_Enable4xMsaa = false; // 일단 멀티 샘플링은 끈다
	//m_4xMsaaQuality는 스왑 체인 생성 시 설정됨


	// 디바이스와 디바이스 컨텍스트를 초기화(생성)한다
	m_D3DXDevice->Init();

	// 스왑 체인을 초기화(생성)한다
	m_SwapChain->Init(m_hWnd, m_D3DXDevice, m_ScreenInfo);

	// 백버퍼용 렌더 타겟를 초기화한다
	// (그런데 여기서 하는 건 별로 없음)
	m_RenderTarget->Init(m_D3DXDevice, m_SwapChain);

	// 그 뒤의 초기화 작업은 OnResize 함수에서 수행
	OnResize(m_ScreenInfo.m_ScreenWidth, m_ScreenInfo.m_ScreenHeight);

	//////////////////////////////////////////////////////////////////////////
	// 렌더링 패스 초기화
	//////////////////////////////////////////////////////////////////////////
	InitRenderingPass();

	// ==================== 인풋 레이아웃과 이펙트 파일을 초기화 ====================
	Effects::InitAll(m_D3DXDevice);
	InputLayouts::InitAll(m_D3DXDevice);
}

void Renderer::Release()
{
	m_PerFrameData.reset();

	// 렌더 큐 clear
	for (int i = 0; i < m_RenderQueue.size(); i++)
		m_RenderQueue.front().reset();
	m_RenderQueue.clear();

	// 렌더링 패스 Release
	m_ForwardRenderingPass->Release();

	//Effects::FinalizeAll();
	//InputLayouts::FinalizeAll();

	m_SwapChain->Release();
	m_RenderTarget->Release();
	m_D3DXDevice->Release();
}

void Renderer::BeginRender()
{
	// 렌더 타겟(백 버퍼)을 클리어한다
	m_RenderTarget->Clear();

	// todo: Font 만들 때 추가하자
	// 출력 병합 단계의 깊이 스텐실 상태를 설정한다
	// 안 하면 깊이가 어그러진다
	//m_D3DXDeviceContext->OMSetDepthStencilState(NormalDSS, 0);
}

void Renderer::PostPerFrameData(std::shared_ptr<PerFrameData> perFrameData)
{
	m_PerFrameData = perFrameData;
}

void Renderer::PostRenderingData(std::shared_ptr<RenderingData> renderingData)
{
	m_RenderQueue.emplace_back(renderingData);
}

void Renderer::Render()
{
	if (m_UsingPostProcessing)
	{

	}
	else
	{
		if (!m_RenderQueue.empty())
			ForwardProcessing();
	}

	// Per Frame Data 초기화
	m_PerFrameData->Clear();
	// 렌더 큐(벡터) 초기화
	m_RenderQueue.clear();
}

void Renderer::EndRender()
{
	// 후면 버퍼를 화면에 Present한다
	m_SwapChain->Present();
}

void Renderer::OnResize(const int& screenWidth, const int& screenHeight)
{
	m_ScreenInfo.m_ScreenWidth = screenWidth;
	m_ScreenInfo.m_ScreenHeight = screenHeight;

	// 스왑 체인의 OnResize
	m_SwapChain->OnResize(m_D3DXDevice->GetDevice().Get(), m_ScreenInfo);

	// 렌더 타겟(백버퍼)의 OnResize
	m_RenderTarget->OnResize(m_ScreenInfo);

	// 렌더 타겟 설정
	m_D3DXDevice->SetRenderTargets(m_RenderTarget);

	// 뷰포트 설정
	m_D3DXDevice->SetSetViewports(m_ScreenInfo);
}

bool Renderer::IsValidDevice()
{
	return m_D3DXDevice->IsValid();
}
