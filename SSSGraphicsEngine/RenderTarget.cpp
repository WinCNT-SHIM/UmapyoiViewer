#include "GraphicsEnginePCH.h"
#include "RenderTarget.h"

namespace GraphicsEngine
{
	void RenderTarget::Init(shared_ptr<DXDevice> device, shared_ptr<SwapChain> swapChain)
	{
		SetDevice(device, swapChain);
	}

	void RenderTarget::Release()
	{
		//ReleaseCOM(m_DepthStencilBuffer);
		//ReleaseCOM(m_DepthStencilView);

		m_RenderTargetTexture.ReleaseAndGetAddressOf();
		m_RenderTargetView.ReleaseAndGetAddressOf();
		m_ShaderResourceView.ReleaseAndGetAddressOf();
		m_DepthStencilBuffer.ReleaseAndGetAddressOf();
		m_DepthStencilView.ReleaseAndGetAddressOf();
		m_NormalDSS.ReleaseAndGetAddressOf();
	}

	void RenderTarget::SetDevice(shared_ptr<DXDevice> device, shared_ptr<SwapChain> swapChain)
	{
		m_D3DXDevice = device;
		m_SwapChain = swapChain;
	}

	void RenderTarget::OnResize(ScreenInfo& screenInfo)
	{
		this->Release();

		// ==================== ���� Ÿ�� ���� ���� ====================
		// ���ҽ��� ���� �並 �����ϰ� �� �並 ���������� �ܰ迡 Binding�ؾ� �Ѵ�
		// Direct3D�� ���ۿ� �׸��� ó���� �ϱ� ���ؼ��� �ĸ� ���۰�
		// ������������ ��� ���ձ�(Output Merger) �ܰ迡 Binding�Ǿ�� �ϴµ�,
		// �׷��� ���ؼ��� �켱 �ĸ� ���ۿ� ���� ���� Ÿ�� �並 �����ؾ� �Ѵ�
		ID3D11Texture2D* _BackBuffer = nullptr;
		// Swap Chain�� �����͸� ȹ��
		HR(m_SwapChain.lock()->GetSwapChain()->GetBuffer(
			0,							// �ĸ� ������ �ε���(1���̹Ƿ� 0�� ����)
			__uuidof(ID3D11Texture2D),	// ������ �������̽� ���� ����
			reinterpret_cast<void**>(&_BackBuffer)) // �ĸ� ������ ������ ��ȯ
		);

		if (_BackBuffer)
		{
			/// <summary>
			/// ���� Ÿ�� �並 �����ϴ� �Լ�
			/// </summary>
			/// <param name="pResource">���� Ÿ�� �並 ���� ���ҽ�</param>
			/// <param name="pDesc">���� Ÿ�� ��� ���� ������ ���� ���ҽ� �����ϱ� ���� ����ü</param>
			/// <param name="ppRTView">������ ���� Ÿ�� �並 ��ȯ</param>
			m_D3DXDevice.lock()->GetDevice()->CreateRenderTargetView(
				_BackBuffer,		// ���⼱ �ĸ� ����
				nullptr,			// ���ҽ��� ù ��° �Ӹ� ������ ���� �� �����Ѵ�
				&m_RenderTargetView	// ������ ���� Ÿ�� �並 ��ȯ
			);
			// ����� COM �������̽��� ����(Reference count�� ���δ�)
			ReleaseCOM(_BackBuffer);
		}


		// ==================== ���� / ���ٽ� ���ۿ� �׿� ���� �� ���� ====================
		// ���� / ���ٽ� ���۴� ���� / ���ٽ� ������ ��� 2���� �ؽ�Ʈ
		// 2���� �ؽ�Ʈ�� �����ϱ� ���ؼ��� �켱 D3D11_TEXTURE2D_DESC ����ü�� �ۼ��Ѵ�
		D3D11_TEXTURE2D_DESC _DepthStencilDesc;
		// �ؽ�ó�� ���� ����(�ؼ� ����)
		_DepthStencilDesc.Width  = (UINT)screenInfo.m_ScreenWidth;
		_DepthStencilDesc.Height = (UINT)screenInfo.m_ScreenHeight;
		// �Ӹ� ����(����/���ٽ� ������ �ؽ�ó�� �Ӹ� ������ �ϳ��� ������ ��)
		_DepthStencilDesc.MipLevels = 1;
		// �ؽ�ó �迭�� �ؽ�ó ����(����/���ٽ� ���۴� �ؽ�ó �ϳ��� �ʿ�)
		_DepthStencilDesc.ArraySize = 1;
		// �ؼ��� ����( ������ DXGI_FORMAT)
		_DepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // ���� 24��Ʈ, ���ٽ� 8��Ʈ

		// 4X MSAA ��� ���� üũ(Swap Chain�� ������ ��ġ�ؾ� ��)
		if (screenInfo.m_Enable4xMsaa)
		{
			_DepthStencilDesc.SampleDesc.Count = 4;						// �ȼ��� ��Ƽ���� ��
			_DepthStencilDesc.SampleDesc.Quality = screenInfo.m_4xMsaaQuality - 1;	// ���ϴ� ǰ�� ���� ����
		}
		else
		{
			_DepthStencilDesc.SampleDesc.Count = 1;		// �ȼ��� ��Ƽ���� ��
			_DepthStencilDesc.SampleDesc.Quality = 0;	// �̹��� ǰ�� ����
		}

		// �ؽ�ó�� �뵵(����/���ٽ� ���۴� �Ϲ������� Default�� �����Ѵ�)
		_DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;			// GPU�� ���ҽ��� �а� ����(CPU�� �Ұ���)
		// ���ҽ��� ���������� �ܰ迡 ���ε��ϱ� ���� �÷���
		_DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// ����/���ٽ� ������ ��� �����ϴ� �÷���
		// CPU�� ���ҽ� �׼��� ��� ����
		_DepthStencilDesc.CPUAccessFlags = 0;					// 0: CPU �׼��� �ʿ� ����
		// ��Ÿ �÷��׵�(����/���ٽ� ���ۿ��� ���� �� ��)
		_DepthStencilDesc.MiscFlags = 0;						// 0: ���� �� ��

		// �ۼ��� D3D11_TEXTURE2D_DESC ����ü�� ����/���ٽ� ���ۿ� �׿� ���� �並 �����Ѵ�
		// ����/���ٽ� ���� ����
		HR(m_D3DXDevice.lock()->GetDevice()->CreateTexture2D(
			&_DepthStencilDesc,		// ������ �ؽ�ó�� �����ϴ� ����ü
			nullptr,				// ����ó�� ä�� �ʱ� �������� ������(����/���ٽ� ���ۿ��� ���ʿ�)
			&m_DepthStencilBuffer)	// ����/���ڽ� ���� ��ȯ
		);
		// ����/���ٽ� ���ۿ� ���� �� ����
		if (m_DepthStencilBuffer)
		{
			HR(m_D3DXDevice.lock()->GetDevice()->CreateDepthStencilView(
				m_DepthStencilBuffer.Get(),	// �並 �����ϰ��� �ϴ� ���ҽ�(���⼱ ����/���ٽ� ����)
				nullptr,				// D3D11_DEPTH_STENCIL_VIEW_DESC ����ü(�������� �ƴ϶�� null ����)
				&m_DepthStencilView)	// ����/���ڽ� �� ��ȯ
			);
		}

		// ����/���ٽ� ���� ���� �����Ѵ�
		D3D11_DEPTH_STENCIL_DESC equalsDesc;
		ZeroMemory(&equalsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		equalsDesc.DepthEnable = true;
		equalsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;		// ���̹��ۿ� ����� �Ѵ�
		equalsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		HR(m_D3DXDevice.lock()->GetDevice()->CreateDepthStencilState(&equalsDesc, &m_NormalDSS));
	}

	void RenderTarget::Clear()
	{
		// �ĸ� ���۸� Ŭ�����Ѵ�(������ DirectXColors.h�� ���� ���
		FLOAT color[4] = { 0.152941f, 0.164062f, 0.296875f, 1.000000000f };
		m_D3DXDevice.lock()->GetDeviceContext()->ClearRenderTargetView(
			m_RenderTargetView.Get(),
			//DirectX::Colors::Gray // DirectXColors.h
			color
		);

		// ���� / ���ٽ� ���۸� Ŭ�����Ѵ�
		m_D3DXDevice.lock()->GetDeviceContext()->ClearDepthStencilView(
			m_DepthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,	// ���� ���۸� 1.0f�� Ŭ����
			0		// ���ٽ� ���۸� 0���� Ŭ����
		);

		// ���� / ���ٽ� ���¸� �缳���Ѵ�
		m_D3DXDevice.lock()->GetDeviceContext()->OMSetDepthStencilState(
			m_NormalDSS.Get(), 0
		);
	}
}