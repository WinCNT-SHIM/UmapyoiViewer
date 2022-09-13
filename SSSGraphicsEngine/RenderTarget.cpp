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

		// ==================== 렌더 타겟 뷰의 생성 ====================
		// 리소스에 대한 뷰를 생성하고 그 뷰를 파이프라인 단계에 Binding해야 한다
		// Direct3D가 버퍼에 그리기 처리를 하기 위해서는 후면 버퍼가
		// 파이프라인의 출력 병합기(Output Merger) 단계에 Binding되어야 하는데,
		// 그러기 위해서는 우선 후면 버퍼에 대한 렌더 타겟 뷰를 생성해야 한다
		ID3D11Texture2D* _BackBuffer = nullptr;
		// Swap Chain의 포인터를 획득
		HR(m_SwapChain.lock()->GetSwapChain()->GetBuffer(
			0,							// 후면 버퍼의 인덱스(1개이므로 0을 설정)
			__uuidof(ID3D11Texture2D),	// 버퍼의 인터페이스 형식 지정
			reinterpret_cast<void**>(&_BackBuffer)) // 후면 버퍼의 포인터 반환
		);

		if (_BackBuffer)
		{
			/// <summary>
			/// 렌더 타겟 뷰를 생성하는 함수
			/// </summary>
			/// <param name="pResource">렌더 타겟 뷰를 만들 리소스</param>
			/// <param name="pDesc">렌더 타겟 뷰로 접근 가능한 하위 리소스 설정하기 위한 구조체</param>
			/// <param name="ppRTView">생성한 렌더 타겟 뷰를 반환</param>
			m_D3DXDevice.lock()->GetDevice()->CreateRenderTargetView(
				_BackBuffer,		// 여기선 후면 버퍼
				nullptr,			// 리소스의 첫 번째 밉맵 레벨에 대한 뷰 생성한다
				&m_RenderTargetView	// 생성한 렌더 타겟 뷰를 반환
			);
			// 사용한 COM 인터페이스을 해제(Reference count를 줄인다)
			ReleaseCOM(_BackBuffer);
		}


		// ==================== 깊이 / 스텐실 버퍼와 그에 대한 뷰 생성 ====================
		// 깊이 / 스텐실 버퍼는 깊이 / 스텐실 정보를 담는 2차원 텍스트
		// 2차원 텍스트를 생성하기 위해서는 우선 D3D11_TEXTURE2D_DESC 구조체를 작성한다
		D3D11_TEXTURE2D_DESC _DepthStencilDesc;
		// 텍스처의 가로 세로(텍셀 단위)
		_DepthStencilDesc.Width  = (UINT)screenInfo.m_ScreenWidth;
		_DepthStencilDesc.Height = (UINT)screenInfo.m_ScreenHeight;
		// 밉맵 레벨(깊이/스텐실 버퍼의 텍스처는 밉맵 레벨이 하나만 있으면 됨)
		_DepthStencilDesc.MipLevels = 1;
		// 텍스처 배열의 텍스처 개수(깊이/스텐실 버퍼는 텍스처 하나만 필요)
		_DepthStencilDesc.ArraySize = 1;
		// 텍셀의 형식( 열거형 DXGI_FORMAT)
		_DepthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // 깊이 24비트, 스텐실 8비트

		// 4X MSAA 사용 여부 체크(Swap Chain의 설정과 일치해야 함)
		if (screenInfo.m_Enable4xMsaa)
		{
			_DepthStencilDesc.SampleDesc.Count = 4;						// 픽셀당 멀티샘플 수
			_DepthStencilDesc.SampleDesc.Quality = screenInfo.m_4xMsaaQuality - 1;	// 원하는 품질 수준 지정
		}
		else
		{
			_DepthStencilDesc.SampleDesc.Count = 1;		// 픽셀당 멀티샘플 수
			_DepthStencilDesc.SampleDesc.Quality = 0;	// 이미지 품질 수준
		}

		// 텍스처의 용도(깊이/스텐실 버퍼는 일반적으로 Default를 설정한다)
		_DepthStencilDesc.Usage = D3D11_USAGE_DEFAULT;			// GPU가 리소스를 읽고 쓰기(CPU는 불가능)
		// 리소스의 파이프라인 단계에 바인딩하기 위한 플래그
		_DepthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;	// 깊이/스텐실 버퍼일 경우 지정하는 플래그
		// CPU의 리소스 액세스 방식 결정
		_DepthStencilDesc.CPUAccessFlags = 0;					// 0: CPU 액세스 필요 없음
		// 기타 플래그들(깊이/스텐실 버퍼에는 적용 안 됨)
		_DepthStencilDesc.MiscFlags = 0;						// 0: 지정 안 함

		// 작성한 D3D11_TEXTURE2D_DESC 구조체로 깊이/스텐실 버퍼와 그에 대한 뷰를 생성한다
		// 깊이/스텐실 버퍼 생성
		HR(m_D3DXDevice.lock()->GetDevice()->CreateTexture2D(
			&_DepthStencilDesc,		// 생성할 텍스처를 서술하는 구조체
			nullptr,				// 덱스처에 채울 초기 데이터의 포인터(깊이/스텐실 버퍼에는 불필요)
			&m_DepthStencilBuffer)	// 깊이/스텔실 버퍼 반환
		);
		// 깊이/스텐실 버퍼에 대한 뷰 생성
		if (m_DepthStencilBuffer)
		{
			HR(m_D3DXDevice.lock()->GetDevice()->CreateDepthStencilView(
				m_DepthStencilBuffer.Get(),	// 뷰를 생성하고자 하는 리소스(여기선 깊이/스텐실 버퍼)
				nullptr,				// D3D11_DEPTH_STENCIL_VIEW_DESC 구조체(무형식이 아니라면 null 가능)
				&m_DepthStencilView)	// 깊이/스텔실 뷰 반환
			);
		}

		// 깊이/스텐실 상태 값을 설정한다
		D3D11_DEPTH_STENCIL_DESC equalsDesc;
		ZeroMemory(&equalsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
		equalsDesc.DepthEnable = true;
		equalsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;		// 깊이버퍼에 쓰기는 한다
		equalsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		HR(m_D3DXDevice.lock()->GetDevice()->CreateDepthStencilState(&equalsDesc, &m_NormalDSS));
	}

	void RenderTarget::Clear()
	{
		// 후면 버퍼를 클리어한다(색상은 DirectXColors.h의 값을 사용
		FLOAT color[4] = { 0.152941f, 0.164062f, 0.296875f, 1.000000000f };
		m_D3DXDevice.lock()->GetDeviceContext()->ClearRenderTargetView(
			m_RenderTargetView.Get(),
			//DirectX::Colors::Gray // DirectXColors.h
			color
		);

		// 깊이 / 스텐실 버퍼를 클리어한다
		m_D3DXDevice.lock()->GetDeviceContext()->ClearDepthStencilView(
			m_DepthStencilView.Get(),
			D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
			1.0f,	// 깊이 버퍼를 1.0f로 클리어
			0		// 스텐실 버퍼를 0으로 클리어
		);

		// 깊이 / 스텐실 상태를 재설정한다
		m_D3DXDevice.lock()->GetDeviceContext()->OMSetDepthStencilState(
			m_NormalDSS.Get(), 0
		);
	}
}