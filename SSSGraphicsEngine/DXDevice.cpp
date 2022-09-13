#include "GraphicsEnginePCH.h"
#include "DXDevice.h"
#include "RenderTarget.h"

namespace GraphicsEngine
{
	DXDevice::~DXDevice()
	{
		// D3D11Device가 소멸되기 전에 리소스 매니저의 버퍼 등등을 먼저 해제해야 한다
		// 안 그러면 D3D11 WARNING: Live Object...와 같은 경고가 뜨고
		// 일반적인 정상적인 메모리 누적 추적에도 영향(이건 대체 왜?)이 생긴다
		// ==> 렌더러 쪽에서 해제하도록 바꿈
		//ResourceManager::GetInstance()->Release();
		this->Release();
	}

	void DXDevice::Init()
	{
		UINT createDevieceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDevieceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG || _DEBUG

		// ==================== D3 디바이스와 디바이스 컨텍스트를 생성한다 ====================
		/// <summary>
		/// D3D11CreateDevice 함수 정리
		/// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdevice
		/// </summary>
		/// <param name="pAdapter">디스플레이 어댑터 지정</param>
		/// <param name="DriverType">드라이브 타입 설정
		///		D3D_DRIVER_TYPE_HARDWARE: 하드웨어에서 Direct3D 기능을 구현하는 하드웨어 드라이버
		///			하드웨어 가속이 적용된다
		///		D3D_DRIVER_TYPE_REFERENCE: 모든 Direct3D 기능을 지원하는 소프트웨어 구현인 참조 드라이버
		///			속도보다는 정확성에 중점을 준 설정
		///		D3D_DRIVER_TYPE_SOFTWARE: 소프트웨어에 Direct3D 기능을 구현하는 소프트웨어 드라이버</param>
		///		D3D_DRIVER_TYPE_WARP: 고성능 소프트웨어 래스터라이저인 WARP 드라이버
		/// <param name="Software">소프트웨어 구동기를 지정</param>
		/// <param name="Flags">추가적인 장치 생성 플래그들(OR로 결합 가능)을 지정
		///		D3D11_CREATE_DEVICE_DEBUG: 디버그 계층 활성화 플래그
		///			Direct3D가 출력 창에 디버그 메시지를 보냄
		///		D3D11_CREATE_DEVICE_SINGLETHREADED： 단일 스레드에서 성능을 향상시키는 플래그
		///			Direct3D가 여러 개의 스레드에서 호출되지 않는다는 보장이 필요< / param>
		/// <param name="pFeatureLevels">기능 수준의 순서를 결정하는 D3D_FEATURE_LEVEL 배열(포인터)</param>
		/// <param name="FeatureLevels">pFeatureLevels의 원소 개수</param>
		/// <param name="SDKVersion">항상 D3D11_SDK_VERSION를 사용</param>
		/// <param name="ppDevice">생성한 장치를 반환</param>
		/// <param name="pFeatureLevel">pFeatureLevels 배열에서 처음으로 지원되는 기능을 반환</param>
		/// <param name="ppImmediateContext">생성된 장치 문맥을 반환</param>
		/// <returns></returns>
		HRESULT hr = D3D11CreateDevice(
			nullptr,					// NULL은 기본 디스플레이
			D3D_DRIVER_TYPE_HARDWARE,	// 기본적으로 하드웨어 가속이 되는 하드웨어 드라이버를 설정함
			nullptr,					// 하드웨어를 사용해서 렌더링하므로 소프트웨어 구동기는 Null로 설정함
			createDevieceFlags,			// 디버그 모드에서는 디버그 계층 활성화함
			nullptr, 0,					// Null이면 디폴트 기능 수준 배열을 사용한다
			D3D11_SDK_VERSION,			// 항상 D3D11_SDK_VERSION를 사용
			&m_D3DXDevice,				// Device
			&m_FeatureLevel,			// 적용된 기능 수준
			&m_D3DXDeviceContext		// DeviceContext
		);

		if (FAILED(hr))
		{
			MessageBox(0, _T("D3D11CreateDevice Failed."), 0, 0);
			//return false;
		}
		if (m_FeatureLevel != D3D_FEATURE_LEVEL_11_0)
		{
			MessageBox(0, _T("D3D11CreateDevice Failed."), 0, 0);
			//return false;
		}
	}

	void DXDevice::Release()
	{
		//ReleaseCOM(m_D3DXDevice);
		//ReleaseCOM(m_D3DXDeviceContext);
		//m_D3DXDevice.Reset();
		m_D3DXDevice.ReleaseAndGetAddressOf();
		//m_D3DXDeviceContext.Reset();
		m_D3DXDeviceContext.ReleaseAndGetAddressOf();

	}

	void DXDevice::SetRenderTargets(shared_ptr<RenderTarget> renderTarget)
	{
		vector<shared_ptr<RenderTarget>> vec = { renderTarget };
		SetRenderTargets(vec);
	}

	void DXDevice::SetRenderTargets(vector<shared_ptr<RenderTarget>> renderTargets)
	{
		// 렌더 타겟의 숫자가 최대(8개)를 넘을 경우
		assert(renderTargets.size() < 9);
		int _cntRenderTargets = renderTargets.size();
		if (_cntRenderTargets > 8) _cntRenderTargets = 8;

		// 렌더 타겟 뷰와 뎁스 스텐실 뷰의 연속성을 위한 처리
		ID3D11RenderTargetView* _arrRTV[8] = {};
		ID3D11DepthStencilView* _arrDSV[8] = {};

		for (int i = 0; i < _cntRenderTargets; i++)
		{
			_arrRTV[i] = renderTargets[i]->GetRTV().Get();
			_arrDSV[i] = renderTargets[i]->GetDSV().Get();
		}

		//// ==================== 생성한 뷰를 출력 병합기(Output Merger) 단계에 Binding ====================
		m_D3DXDeviceContext->OMSetRenderTargets(
			_cntRenderTargets,	// 바인드할 렌더 타겟의 수
			&_arrRTV[0],	// 렌더 타겟 뷰들(포인터)의 배열에 대한 포인터
			_arrDSV[0]		// 파이프라인에 바인딩할 깊이/스텐실 뷰의 포인터
		);
	}

	void DXDevice::SetSetViewports(ScreenInfo& screenInfo)
	{
		// ==================== 뷰포트 설정 ====================
		// 뷰포트에 대한 설정을 D3D11_VIEWPORT 구조체에 서술한다
		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0.0f;								// 뷰포트의 시작 X좌표
		vp.TopLeftY = 0.0f;								// 뷰포트의 시작 Y좌표
		vp.Width = static_cast<float>(screenInfo.m_ScreenWidth);	// 뷰포트의 넓이
		vp.Height = static_cast<float>(screenInfo.m_ScreenHeight);	// 뷰포트의 높이
		vp.MinDepth = 0.0f;								// 최소 깊이 버퍼 값
		vp.MaxDepth = 1.0f;								// 최대 깊이 버터 값
		// 파이프라인의 래스터라이저 단계에 뷰포트 배열을 바인딩한다
		m_D3DXDeviceContext->RSSetViewports(
			1,		// 바인딩할 뷰포트의 수
			&vp		// D3D11_VIEWPORT 구조체
		);
	}

}