#include "GraphicsEnginePCH.h"
#include "ConstantBuffer.h"

using namespace GraphicsEngine;

ConstantBuffer::ConstantBuffer(std::shared_ptr<DXDevice> device, string m_FilePath)
	: EffectBase(device, m_FilePath, _T("CBPerFrame"))
{
	// ����Ʈ���� PosColorTech��� ��ũ�п� ���� �����͸� ȹ��
	m_Tech = m_FX->GetTechniqueByName("CBPerFrameTech");

	// ��� ���� ����
	m_CBPerFrame = m_FX->GetConstantBufferByName("cbPerFrame");


	// ����Ʈ�� ��� ���ۿ� ���� ������ ȹ��
	// Per Frame Data
	//m_CameraInfo = m_FX->GetVariableByName("gCameraInfo");
	//m_DirLightCount = m_FX->GetVariableByName("gDirLightCount");
	//m_DirLights = m_FX->GetVariableByName("gDirLights");
	//m_SpotLightCount = m_FX->GetVariableByName("gSpotLightCount");
	//m_SpotLights = m_FX->GetVariableByName("gSpotLights");
	//m_PointLightCount = m_FX->GetVariableByName("gPointLightCount");
	//m_PointLights = m_FX->GetVariableByName("gPointLights");
}

ConstantBuffer::~ConstantBuffer()
{
}

void ConstantBuffer::Release()
{
	//m_CameraInfo.Reset();
	//m_DirLightCount.Reset();
	//m_DirLights.Reset();
	//m_SpotLightCount.Reset();
	//m_SpotLights.Reset();
	//m_PointLightCount.Reset();
	//m_PointLights.Reset();


	//m_CB->Release();

	__super::Release();
}

void ConstantBuffer::UpdatePerFrameData(std::shared_ptr<PerFrameData>& m_PerFrameData)
{
	// Camera Info
	m_CBPerFrame->GetMemberByName("gCameraInfo")->SetRawValue(
		m_PerFrameData->m_CameraInfo.get()
		, 0, sizeof(CameraInfo)
	);

	// Directional Light Count
	unsigned int dirLightCount = m_PerFrameData->m_DirectionalLightInfos.size();
	m_CBPerFrame->GetMemberByName("gDirLightCount")->AsScalar()->SetInt(dirLightCount);

	// Directional Light
	// ������ --> ������ �����ϰ� �װ� GPU�� �ѱ��
	std::vector<DirectionalLightInfo> cbPerFrameDirectionalLight;
	for (auto& item : m_PerFrameData->m_DirectionalLightInfos)
	{
		cbPerFrameDirectionalLight.emplace_back(std::move(*item.get()));
	}

	m_CBPerFrame->GetMemberByName("gDirLights")->SetRawValue(
		cbPerFrameDirectionalLight.data()
		, 0, sizeof(DirectionalLightInfo) * dirLightCount
	);

	//m_DirLightCount->SetRawValue(
	//	&m_PerFrameData->m_DirectionalLightInfos
	//	, 0, sizeof(&m_PerFrameData->m_DirectionalLightInfos)
	//);
	
	//m_DirLights->SetRawValue(
	//	&m_PerFrameData->m_DirectionalLightInfos
	//	, 0, sizeof(&m_PerFrameData->m_DirectionalLightInfos)
	//);

	//m_DirLights;
	//m_SpotLightCount;
	//m_SpotLights;
	//m_PointLightCount;
	//m_PointLights;

	D3DX11_TECHNIQUE_DESC techDesc;
	m_Tech->GetDesc(&techDesc);
	// ������ ���鼭 ����Ʈ�� ��ũ���� �� �н��� �����ؼ� ���� ������ �׸���
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_Tech->GetPassByIndex(p)->Apply(0, m_DeviceContext.Get());
	}
}
