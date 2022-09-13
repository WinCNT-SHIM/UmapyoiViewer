#include "GraphicsEnginePCH.h"
#include "PosColorEffect.h"

using namespace GraphicsEngine;

PosColorEffect::PosColorEffect(std::shared_ptr<DXDevice> device, string m_FilePath)
	: EffectBase(device, m_FilePath, _T("PosColor"))
{
	// ����Ʈ���� PosColorTech��� ��ũ�п� ���� �����͸� ȹ��
	m_Tech = m_FX->GetTechniqueByName("PosColorTech");
	

	// ����Ʈ�� ��� ���ۿ� ���� ������ ȹ��
	// Per Object Data
	m_WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();


	// ��ǲ ���̾ƿ� ����
	D3DX11_PASS_DESC passDesc;
	m_Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->GetDevice()->CreateInputLayout(
		VertexStruct::VertexPosColor::InputLayout
		, 2
		, passDesc.pIAInputSignature
		, passDesc.IAInputSignatureSize
		, m_InputLayout.GetAddressOf())
	);
}

PosColorEffect::~PosColorEffect()
{
}

void PosColorEffect::Release()
{
	m_WorldViewProj.Reset();
	__super::Release();
}

void PosColorEffect::EffectRendering(std::shared_ptr<RenderingData>& renderingData)
{
	// ��ǲ ���̾ƿ� ����
	m_DeviceContext->IASetInputLayout(m_InputLayout.Get());


	// ��� ���ۿ� ������ ���� ������
	// World, View, Projection
	SetWorldViewProj(renderingData->m_GeoInfo->m_WorldViewProj);


	// ��� ���۸� �����ϰ� ������ Draw Call�� �ϴ� �κ�
	D3DX11_TECHNIQUE_DESC techDesc;
	m_Tech->GetDesc(&techDesc);
	// ������ ���鼭 ����Ʈ�� ��ũ���� �� �н��� �����ؼ� ���� ������ �׸���
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		/// <summary>
		/// GetPassByIndex �Լ�
		/// --> �־��� ���ο� �ش��ϴ� �н��� ���� �����͸� ��ȯ�Ѵ�
		/// Apply �Լ�
		/// --> GPU �޸𸮿� ����� ��� ���۸� �� ���� ������ �����ϰ�,
		///     �н��� ������ ���̴� �Լ����� ���������ο� �ٿ���ϰ�,
		///     �н��� ������ ���� ������Ʈ�� �����Ѵ�
		/// </summary>
		m_Tech->GetPassByIndex(p)->Apply(0, m_DeviceContext.Get());

		// ���ҽ����� �ش� �޽� ���ҽ��� ������ �׸���
		shared_ptr<GraphicsEngine::Mesh> meshResource = ResourceManager::GetInstance()->GetMeshResource(renderingData->m_ObjectInfo->m_MeshID);
		if (meshResource)
		{
			meshResource->Render();
		}
	}
}
