#include "GraphicsEnginePCH.h"
#include "BasicEffect.h"

using namespace GraphicsEngine;

BasicEffect::BasicEffect(std::shared_ptr<DXDevice> device, string m_FilePath)
	: EffectBase(device, m_FilePath, _T("Basic"))
{
	// 이펙트에서 PosColorTech라는 테크닉에 대한 포인터를 획득
	m_Tech = m_FX->GetTechniqueByName("BasicTech");
	

	// 이펙트의 상수 버퍼에 대한 포인터 획득// Per Object Data
	m_World = m_FX->GetVariableByName("gWorld")->AsMatrix();
	m_WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_WorldInvTranspose = m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	m_TexTransform = m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	m_Material = m_FX->GetVariableByName("gMaterial");
	m_ObjectID = m_FX->GetVariableByName("gObjectID");

	m_DiffuseMap = m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();


	// 인풋 레이아웃 설정
	D3DX11_PASS_DESC passDesc;
	m_Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->GetDevice()->CreateInputLayout(
		VertexStruct::VertexBasic::InputLayout
		, 5
		, passDesc.pIAInputSignature
		, passDesc.IAInputSignatureSize
		, m_InputLayout.GetAddressOf())
	);
}

BasicEffect::~BasicEffect()
{
}

void BasicEffect::Release()
{
	m_World.Reset();
	m_WorldViewProj.Reset();
	m_WorldInvTranspose.Reset();
	m_TexTransform.Reset();
	m_Material.Reset();
	m_ObjectID.Reset();

	m_DiffuseMap.Reset();

	__super::Release();
}

void BasicEffect::SetPerObjectData(std::shared_ptr<RenderingData>& renderingData)
{
	SetWorld(renderingData->m_GeoInfo->m_World);
	SetWorldViewProj(renderingData->m_GeoInfo->m_WorldViewProj);
	SetWorldInvTranspose(renderingData->m_GeoInfo->m_WorldInvTranspose);
	SetTexTransform(renderingData->m_GeoInfo->m_TexTransform);
	SetMaterial(renderingData->m_MaterialInfo);
	SetObjectID(renderingData->m_ObjectInfo->m_ObjectID);
}

void BasicEffect::EffectRendering(std::shared_ptr<RenderingData>& renderingData)
{
	// 인풋 레이아웃 설정
	m_DeviceContext->IASetInputLayout(m_InputLayout.Get());


	// 상수 버퍼 설정
	// Per Object Data
	SetPerObjectData(renderingData);

	// Diffuse Texture
	if (renderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID != UINT_MAX)
	{
		shared_ptr<GraphicsEngine::Texture> textureResource = ResourceManager::GetInstance()->GetTextureResource(renderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID);

		SetDiffuseMap(textureResource->GetSRV().Get());
	}
	else
	{
		SetDiffuseMap(nullptr);
	}

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

void BasicEffect::SetMaterial(std::shared_ptr<MaterialInfo> materialInfo)
{
	// 뒤의 diffuseMapID, NormalMapID는 제외한다
	m_Material->SetRawValue(
		materialInfo.get()
		, 0
		, sizeof(MaterialInfo) - sizeof(TextureInfo)
	);
}
