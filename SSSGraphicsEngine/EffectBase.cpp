#include "GraphicsEnginePCH.h"
#include "EffectBase.h"

using namespace GraphicsEngine;

EffectBase::EffectBase(std::shared_ptr<DXDevice> device, string filename, tstring effectName)
	: m_Name(effectName)
{
	// ����̽� ���ؽ�Ʈ�� �����صΰ� ���(���� ������ �� �� ����)
	m_DeviceContext = device->GetDeviceContext();

	std::ifstream m_FXFile(filename, std::ios::binary);

	assert(m_FXFile);

	m_FXFile.seekg(0, std::ios_base::end);
	int size = (int)m_FXFile.tellg();
	m_FXFile.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	m_FXFile.read(&compiledShader[0], size);
	m_FXFile.close();

	/// <summary>
	/// �о�� �����ϵ� ����Ʈ ���Ͽ���
	/// D3DX11CreateEffectFromMemory �Լ��� ����Ʈ ��ü�� ����
	/// </summary>
	HR(D3DX11CreateEffectFromMemory(
		&compiledShader[0]
		, size
		, 0
		, device->GetDevice().Get()
		, &m_FX)
	);

	// Per Frame ��� ���� ȹ��
	m_CBPerFrame = m_FX->GetConstantBufferByName("cbPerFrame");
}

EffectBase::~EffectBase()
{
	
}

void EffectBase::Release()
{
	//m_CBPerFrame.Reset();
	m_InputLayout.Reset();
	ReleaseCOM(m_CBPerFrame);
	m_Tech.Reset();
	m_FX.Reset();
	m_DeviceContext.Reset();
}

void EffectBase::SetCBPerFrame(ComPtr<ID3DX11EffectConstantBuffer> cbPerFrame)
{
	ComPtr<ID3D11Buffer> pConstantBuffer = nullptr;
	cbPerFrame->GetConstantBuffer(pConstantBuffer.GetAddressOf());
	m_CBPerFrame->SetConstantBuffer(pConstantBuffer.Get());
}