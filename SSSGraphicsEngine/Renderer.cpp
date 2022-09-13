#include "GraphicsEnginePCH.h"
#include "Renderer.h"

#include "RenderTarget.h"
#include "Effects.h"
#include "InputLayouts.h"

// �Ŵ���
#include "ResourceManager.h"

// ������ ������
#include "GraphicsRenderingData.h"

// ������ �н�
#include "ForwardRenderingPass.h"

using namespace GraphicsEngine;

// ���� ����
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
	// ������ ���μ���!!
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

	// ��ũ�� ������ ��������
	m_ScreenInfo.m_ScreenWidth = screenWidth;
	m_ScreenInfo.m_ScreenHeight = screenHeight;
	m_ScreenInfo.m_Enable4xMsaa = false; // �ϴ� ��Ƽ ���ø��� ����
	//m_4xMsaaQuality�� ���� ü�� ���� �� ������


	// ����̽��� ����̽� ���ؽ�Ʈ�� �ʱ�ȭ(����)�Ѵ�
	m_D3DXDevice->Init();

	// ���� ü���� �ʱ�ȭ(����)�Ѵ�
	m_SwapChain->Init(m_hWnd, m_D3DXDevice, m_ScreenInfo);

	// ����ۿ� ���� Ÿ�ٸ� �ʱ�ȭ�Ѵ�
	// (�׷��� ���⼭ �ϴ� �� ���� ����)
	m_RenderTarget->Init(m_D3DXDevice, m_SwapChain);

	// �� ���� �ʱ�ȭ �۾��� OnResize �Լ����� ����
	OnResize(m_ScreenInfo.m_ScreenWidth, m_ScreenInfo.m_ScreenHeight);

	//////////////////////////////////////////////////////////////////////////
	// ������ �н� �ʱ�ȭ
	//////////////////////////////////////////////////////////////////////////
	InitRenderingPass();

	// ==================== ��ǲ ���̾ƿ��� ����Ʈ ������ �ʱ�ȭ ====================
	Effects::InitAll(m_D3DXDevice);
	InputLayouts::InitAll(m_D3DXDevice);
}

void Renderer::Release()
{
	m_PerFrameData.reset();

	// ���� ť clear
	for (int i = 0; i < m_RenderQueue.size(); i++)
		m_RenderQueue.front().reset();
	m_RenderQueue.clear();

	// ������ �н� Release
	m_ForwardRenderingPass->Release();

	//Effects::FinalizeAll();
	//InputLayouts::FinalizeAll();

	m_SwapChain->Release();
	m_RenderTarget->Release();
	m_D3DXDevice->Release();
}

void Renderer::BeginRender()
{
	// ���� Ÿ��(�� ����)�� Ŭ�����Ѵ�
	m_RenderTarget->Clear();

	// todo: Font ���� �� �߰�����
	// ��� ���� �ܰ��� ���� ���ٽ� ���¸� �����Ѵ�
	// �� �ϸ� ���̰� ��׷�����
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

	// Per Frame Data �ʱ�ȭ
	m_PerFrameData->Clear();
	// ���� ť(����) �ʱ�ȭ
	m_RenderQueue.clear();
}

void Renderer::EndRender()
{
	// �ĸ� ���۸� ȭ�鿡 Present�Ѵ�
	m_SwapChain->Present();
}

void Renderer::OnResize(const int& screenWidth, const int& screenHeight)
{
	m_ScreenInfo.m_ScreenWidth = screenWidth;
	m_ScreenInfo.m_ScreenHeight = screenHeight;

	// ���� ü���� OnResize
	m_SwapChain->OnResize(m_D3DXDevice->GetDevice().Get(), m_ScreenInfo);

	// ���� Ÿ��(�����)�� OnResize
	m_RenderTarget->OnResize(m_ScreenInfo);

	// ���� Ÿ�� ����
	m_D3DXDevice->SetRenderTargets(m_RenderTarget);

	// ����Ʈ ����
	m_D3DXDevice->SetSetViewports(m_ScreenInfo);
}

bool Renderer::IsValidDevice()
{
	return m_D3DXDevice->IsValid();
}
