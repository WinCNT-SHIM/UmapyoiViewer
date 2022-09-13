#include "GraphicsEnginePCH.h"
#include "SwapChain.h"

namespace GraphicsEngine
{
	void SwapChain::Init(HWND hWnd, std::shared_ptr<DXDevice> device, ScreenInfo& screenInfo)
	{
		m_hWnd = hWnd;

		// ==================== 4X MSAA ǰ�� ���� ������ üũ�Ѵ� ====================
		HR(device->GetDevice()->CheckMultisampleQualityLevels(
			DXGI_FORMAT_R8G8B8A8_UNORM,		// �ؽ�ó ����
			4,								// ���� ���ø� �� ���� ��(4X�̹Ƿ� 4�� ����)
			&screenInfo.m_4xMsaaQuality				// ����Ϳ��� �����ϴ� ǰ�� ������ ��
		));
		assert(screenInfo.m_4xMsaaQuality > 0); // ��ȯ�� ǰ�� ���� 0���� Ŀ���ؼ� assert�� ����

		//this->OnResize(device, screenInfo);
	}

	void SwapChain::Release()
	{
		//ReleaseCOM(m_SwapChain);
		m_SwapChain.ReleaseAndGetAddressOf();
	}

	void SwapChain::OnResize(const ComPtr<ID3D11Device>& device, ScreenInfo& screenInfo)
	{
		// �ʱ�ȭ ������ ����
		this->Release();

		// ==================== DXGI_SWAP_CHAIN_DESC ����ü�� �����Ѵ� ====================
		// DXGI_SWAP_CHAIN_DESC ����ü�� �ν��Ͻ��� ���� ���ϴ� ��ȯ �罽�� Ư������ ����
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		// DXGI_MODE_DESC BufferDesc: �ĸ� ������ �Ӽ����� �����ϴ� �������� ����ü
		// â�� Ŭ���̾�Ʈ ������ ũ��(0�̸� ��� â ũ��� ����)
		sd.BufferDesc.Width = screenInfo.m_ScreenWidth;
		sd.BufferDesc.Height = screenInfo.m_ScreenHeight;
		// ���÷��� ��尻����
		sd.BufferDesc.RefreshRate.Numerator = 60;	// �ְ� ������
		sd.BufferDesc.RefreshRate.Denominator = 1;	// ���� ������
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	// �ĸ� ���� �ȼ� ����
		// ���÷��� ��ĵ���� ���
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // ������
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;	// ������
		// 4X MSAA ��� ���� üũ
		if (screenInfo.m_Enable4xMsaa)
		{
			// �ȼ��� ��Ƽ���� ��
			sd.SampleDesc.Count = 4;
			// ���ϴ� ǰ�� ���� ����
			// ǰ�� ������ ������ �ؽ�ó ���İ� �ȼ��� ���� ���� �����Ѵ�
			// ������ ǰ�� ������ ������ 0���� CheckMultisampleQualityLevels�� ���� �� -1�����̴�
			sd.SampleDesc.Quality = screenInfo.m_4xMsaaQuality - 1;
		}
		else
		{
			sd.SampleDesc.Count = 1;	// �ȼ��� ��Ƽ���� ��
			sd.SampleDesc.Quality = 0;	// �̹��� ǰ�� ����
		}
		// �ĸ� ���ۿ� ���� ��� �뵵 ����(���̴� �Է�, Ȥ�� ���� Ÿ�� ���)
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;	// �ĸ� ���۸� Render Target���� ���
		// Swap Chain���� ����� �ĸ� ������ ����
		sd.BufferCount = 1; //(1: ���� ���۸�, 2: ���� ���۸���)
		// ������ ����� ǥ���� �������� �ڵ�
		sd.OutputWindow = m_hWnd;
		// â ���, ��ü ȭ�� ��� ����
		sd.Windowed = true;
		// ǥ���� ��, ���� ������ ������ ó���ϱ� ���� �ɼ�
		// DXGI_SWAP_EFFECT_DISCARD: ���÷��� ����̹��� Swap Chain�� ���� ���� ȿ������ ���� ����� ������
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // ���Ž� �ڵ����� ���...
		// ���� ü�� ���ۿ� ���� �߰� �ɼ�
		// 0: â => ��ü ȭ�� ��, ���� ����ũ�� ���÷��� ���(�ػ�?)�� ���
		// DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH: â => ��ü ȭ�� ��, �ĸ� ���� ������ �����ϴ� ��尡 �ڵ� ����
		sd.Flags = 0;


		// ==================== Swap Chain�� �����Ѵ� ====================
		// IDXGIFactory �������̽��� CreateSwapChain �Լ��� Swap Chain �������̽��� �����Ѵ�
		// Swap Chain�� ����� ���ؼ��� ��ġ ������ ���� IDXGIFactory �ν��Ͻ��� ����ؾ� �Ѵ�
		// ������ ��ġ ������ ���� IDXGIFactory �ν��Ͻ��� ��� ���� �Ϸ��� �����̴�
		IDXGIDevice* _DxgiDevice = 0;
		HR(device->QueryInterface(__uuidof(IDXGIDevice), (void**)&_DxgiDevice));
		IDXGIAdapter* _DxgiAdapter = 0;
		HR(_DxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&_DxgiAdapter));
		IDXGIFactory* _DxgiFactory = 0;
		HR(_DxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&_DxgiFactory)); // IDXGIFactory �ν��Ͻ�

		// Swap Chain�� �����Ѵ�
		HR(_DxgiFactory->CreateSwapChain(device.Get(), &sd, &m_SwapChain));

		// ����� COM �������̽��� �����Ѵ�
		ReleaseCOM(_DxgiDevice);
		ReleaseCOM(_DxgiAdapter);
		ReleaseCOM(_DxgiFactory);
	}

	void SwapChain::Present()
	{
		assert(m_SwapChain);

		// todo: Present�� ���� ������ ����� �ϴ� �������� ����
		HR(m_SwapChain->Present(0, 0));
	}
}