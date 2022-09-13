#include "GraphicsEnginePCH.h"
#include "DXDevice.h"
#include "RenderTarget.h"

namespace GraphicsEngine
{
	DXDevice::~DXDevice()
	{
		// D3D11Device�� �Ҹ�Ǳ� ���� ���ҽ� �Ŵ����� ���� ����� ���� �����ؾ� �Ѵ�
		// �� �׷��� D3D11 WARNING: Live Object...�� ���� ��� �߰�
		// �Ϲ����� �������� �޸� ���� �������� ����(�̰� ��ü ��?)�� �����
		// ==> ������ �ʿ��� �����ϵ��� �ٲ�
		//ResourceManager::GetInstance()->Release();
		this->Release();
	}

	void DXDevice::Init()
	{
		UINT createDevieceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
		createDevieceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG || _DEBUG

		// ==================== D3 ����̽��� ����̽� ���ؽ�Ʈ�� �����Ѵ� ====================
		/// <summary>
		/// D3D11CreateDevice �Լ� ����
		/// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/nf-d3d11-d3d11createdevice
		/// </summary>
		/// <param name="pAdapter">���÷��� ����� ����</param>
		/// <param name="DriverType">����̺� Ÿ�� ����
		///		D3D_DRIVER_TYPE_HARDWARE: �ϵ����� Direct3D ����� �����ϴ� �ϵ���� ����̹�
		///			�ϵ���� ������ ����ȴ�
		///		D3D_DRIVER_TYPE_REFERENCE: ��� Direct3D ����� �����ϴ� ����Ʈ���� ������ ���� ����̹�
		///			�ӵ����ٴ� ��Ȯ���� ������ �� ����
		///		D3D_DRIVER_TYPE_SOFTWARE: ����Ʈ��� Direct3D ����� �����ϴ� ����Ʈ���� ����̹�</param>
		///		D3D_DRIVER_TYPE_WARP: ���� ����Ʈ���� �����Ͷ������� WARP ����̹�
		/// <param name="Software">����Ʈ���� �����⸦ ����</param>
		/// <param name="Flags">�߰����� ��ġ ���� �÷��׵�(OR�� ���� ����)�� ����
		///		D3D11_CREATE_DEVICE_DEBUG: ����� ���� Ȱ��ȭ �÷���
		///			Direct3D�� ��� â�� ����� �޽����� ����
		///		D3D11_CREATE_DEVICE_SINGLETHREADED�� ���� �����忡�� ������ ����Ű�� �÷���
		///			Direct3D�� ���� ���� �����忡�� ȣ����� �ʴ´ٴ� ������ �ʿ�< / param>
		/// <param name="pFeatureLevels">��� ������ ������ �����ϴ� D3D_FEATURE_LEVEL �迭(������)</param>
		/// <param name="FeatureLevels">pFeatureLevels�� ���� ����</param>
		/// <param name="SDKVersion">�׻� D3D11_SDK_VERSION�� ���</param>
		/// <param name="ppDevice">������ ��ġ�� ��ȯ</param>
		/// <param name="pFeatureLevel">pFeatureLevels �迭���� ó������ �����Ǵ� ����� ��ȯ</param>
		/// <param name="ppImmediateContext">������ ��ġ ������ ��ȯ</param>
		/// <returns></returns>
		HRESULT hr = D3D11CreateDevice(
			nullptr,					// NULL�� �⺻ ���÷���
			D3D_DRIVER_TYPE_HARDWARE,	// �⺻������ �ϵ���� ������ �Ǵ� �ϵ���� ����̹��� ������
			nullptr,					// �ϵ��� ����ؼ� �������ϹǷ� ����Ʈ���� ������� Null�� ������
			createDevieceFlags,			// ����� ��忡���� ����� ���� Ȱ��ȭ��
			nullptr, 0,					// Null�̸� ����Ʈ ��� ���� �迭�� ����Ѵ�
			D3D11_SDK_VERSION,			// �׻� D3D11_SDK_VERSION�� ���
			&m_D3DXDevice,				// Device
			&m_FeatureLevel,			// ����� ��� ����
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
		// ���� Ÿ���� ���ڰ� �ִ�(8��)�� ���� ���
		assert(renderTargets.size() < 9);
		int _cntRenderTargets = renderTargets.size();
		if (_cntRenderTargets > 8) _cntRenderTargets = 8;

		// ���� Ÿ�� ��� ���� ���ٽ� ���� ���Ӽ��� ���� ó��
		ID3D11RenderTargetView* _arrRTV[8] = {};
		ID3D11DepthStencilView* _arrDSV[8] = {};

		for (int i = 0; i < _cntRenderTargets; i++)
		{
			_arrRTV[i] = renderTargets[i]->GetRTV().Get();
			_arrDSV[i] = renderTargets[i]->GetDSV().Get();
		}

		//// ==================== ������ �並 ��� ���ձ�(Output Merger) �ܰ迡 Binding ====================
		m_D3DXDeviceContext->OMSetRenderTargets(
			_cntRenderTargets,	// ���ε��� ���� Ÿ���� ��
			&_arrRTV[0],	// ���� Ÿ�� ���(������)�� �迭�� ���� ������
			_arrDSV[0]		// ���������ο� ���ε��� ����/���ٽ� ���� ������
		);
	}

	void DXDevice::SetSetViewports(ScreenInfo& screenInfo)
	{
		// ==================== ����Ʈ ���� ====================
		// ����Ʈ�� ���� ������ D3D11_VIEWPORT ����ü�� �����Ѵ�
		D3D11_VIEWPORT vp;
		vp.TopLeftX = 0.0f;								// ����Ʈ�� ���� X��ǥ
		vp.TopLeftY = 0.0f;								// ����Ʈ�� ���� Y��ǥ
		vp.Width = static_cast<float>(screenInfo.m_ScreenWidth);	// ����Ʈ�� ����
		vp.Height = static_cast<float>(screenInfo.m_ScreenHeight);	// ����Ʈ�� ����
		vp.MinDepth = 0.0f;								// �ּ� ���� ���� ��
		vp.MaxDepth = 1.0f;								// �ִ� ���� ���� ��
		// ������������ �����Ͷ����� �ܰ迡 ����Ʈ �迭�� ���ε��Ѵ�
		m_D3DXDeviceContext->RSSetViewports(
			1,		// ���ε��� ����Ʈ�� ��
			&vp		// D3D11_VIEWPORT ����ü
		);
	}

}