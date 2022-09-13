#include "GraphicsEnginePCH.h"
#include "PosColorEffect.h"

using namespace GraphicsEngine;

PosColorEffect::PosColorEffect(std::shared_ptr<DXDevice> device, string m_FilePath)
	: EffectBase(device, m_FilePath, _T("PosColor"))
{
	// 이펙트에서 PosColorTech라는 테크닉에 대한 포인터를 획득
	m_Tech = m_FX->GetTechniqueByName("PosColorTech");
	

	// 이펙트의 상수 버퍼에 대한 포인터 획득
	// Per Object Data
	m_WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();


	// 인풋 레이아웃 설정
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
	// 인풋 레이아웃 설정
	m_DeviceContext->IASetInputLayout(m_InputLayout.Get());


	// 상수 버퍼에 설정할 값을 세팅함
	// World, View, Projection
	SetWorldViewProj(renderingData->m_GeoInfo->m_WorldViewProj);


	// 상수 버퍼를 갱신하고 실제로 Draw Call를 하는 부분
	D3DX11_TECHNIQUE_DESC techDesc;
	m_Tech->GetDesc(&techDesc);
	// 루프를 돌면서 이펙트의 테크닉의 각 패스를 적용해서 기하 구조를 그린다
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		/// <summary>
		/// GetPassByIndex 함수
		/// --> 주어진 색인에 해당하는 패스에 대한 포인터를 반환한다
		/// Apply 함수
		/// --> GPU 메모리에 저장된 상수 버퍼를 새 변수 값으로 갱신하고,
		///     패스에 설정된 셰이더 함수들을 파이프라인에 바운딩하고,
		///     패스에 설정된 렌더 스테이트를 적용한다
		/// </summary>
		m_Tech->GetPassByIndex(p)->Apply(0, m_DeviceContext.Get());

		// 리소스에서 해당 메쉬 리소스를 가져와 그린다
		shared_ptr<GraphicsEngine::Mesh> meshResource = ResourceManager::GetInstance()->GetMeshResource(renderingData->m_ObjectInfo->m_MeshID);
		if (meshResource)
		{
			meshResource->Render();
		}
	}
}
