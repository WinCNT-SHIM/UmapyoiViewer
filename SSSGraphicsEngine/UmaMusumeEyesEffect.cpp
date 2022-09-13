#include "GraphicsEnginePCH.h"
#include "UmaMusumeEyesEffect.h"

using namespace GraphicsEngine;

UmaMusumeEyesEffect::UmaMusumeEyesEffect(std::shared_ptr<DXDevice> device, string m_FilePath)
	: EffectBase(device, m_FilePath, _T("UmaMusumeEyes"))
{
	// 이펙트에서 PosColorTech라는 테크닉에 대한 포인터를 획득
	m_Tech = m_FX->GetTechniqueByName("UmaMusumeEyesTech");
	

	// 이펙트의 상수 버퍼에 대한 포인터 획득
	// Per Object Data
	m_World = m_FX->GetVariableByName("gWorld")->AsMatrix();
	m_View = m_FX->GetVariableByName("gView")->AsMatrix();
	m_Proj = m_FX->GetVariableByName("gProj")->AsMatrix();
	m_WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_WorldInvTranspose = m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	m_TexTransform = m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	m_Material = m_FX->GetVariableByName("gMaterial");
	m_ObjectID = m_FX->GetVariableByName("gObjectID");

	// cbSkinned - BoneTransforms
	m_BoneTMs = m_FX->GetVariableByName("gBoneTransforms")->AsMatrix();

	// Texture
	m_DiffuseMap    = m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	m_EyesHighlight00Map = m_FX->GetVariableByName("gEyesHighlight00Map")->AsShaderResource();
	m_EyesHighlight01Map = m_FX->GetVariableByName("gEyesHighlight01Map")->AsShaderResource();
	m_EyesHighlight02Map = m_FX->GetVariableByName("gEyesHighlight02Map")->AsShaderResource();


	// 인풋 레이아웃 설정
	D3DX11_PASS_DESC passDesc;
	m_Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->GetDevice()->CreateInputLayout(
		VertexStruct::VertexBasicSkinned::InputLayout
		, 7
		, passDesc.pIAInputSignature
		, passDesc.IAInputSignatureSize
		, m_InputLayout.GetAddressOf())
	);
}

UmaMusumeEyesEffect::~UmaMusumeEyesEffect()
{
}

void UmaMusumeEyesEffect::Release()
{
	m_World.Reset();
	m_View.Reset();
	m_Proj.Reset();
	m_WorldViewProj.Reset();
	m_WorldInvTranspose.Reset();
	m_TexTransform.Reset();
	m_Material.Reset();
	m_ObjectID.Reset();

	m_BoneTMs.Reset();

	m_DiffuseMap.Reset();
	m_EyesHighlight00Map.Reset();
	m_EyesHighlight01Map.Reset();
	m_EyesHighlight02Map.Reset();

	__super::Release();
}

void UmaMusumeEyesEffect::SetPerObjectData(std::shared_ptr<RenderingData>& renderingData)
{
	SetWorld(renderingData->m_GeoInfo->m_World);
	SetView(renderingData->m_GeoInfo->m_View);
	SetProj(renderingData->m_GeoInfo->m_Proj);
	SetWorldViewProj(renderingData->m_GeoInfo->m_WorldViewProj);
	SetWorldInvTranspose(renderingData->m_GeoInfo->m_WorldInvTranspose);
	SetTexTransform(renderingData->m_GeoInfo->m_TexTransform);
	SetMaterial(renderingData->m_MaterialInfo);
	SetObjectID(renderingData->m_ObjectInfo->m_ObjectID);
}

void UmaMusumeEyesEffect::SetTextureMap(TextureInfo& textureInfo)
{
	// diff
	if (textureInfo.m_DiffuseMapID != UINT_MAX)
		m_DiffuseMap->SetResource(ResourceManager::GetInstance()->GetTextureResource(textureInfo.m_DiffuseMapID)->GetSRV().Get());
	else
		m_DiffuseMap->SetResource(nullptr);


	// Highlight00
	if (textureInfo.m_EyesHighlight00MapID != UINT_MAX)
		m_EyesHighlight00Map->SetResource(ResourceManager::GetInstance()->GetTextureResource(textureInfo.m_EyesHighlight00MapID)->GetSRV().Get());
	else
		m_EyesHighlight00Map->SetResource(nullptr);

	// Highlight01
	if (textureInfo.m_EyesHighlight01MapID != UINT_MAX)
		m_EyesHighlight01Map->SetResource(ResourceManager::GetInstance()->GetTextureResource(textureInfo.m_EyesHighlight01MapID)->GetSRV().Get());
	else
		m_EyesHighlight01Map->SetResource(nullptr);

	// Highlight02
	if (textureInfo.m_EyesHighlight02MapID != UINT_MAX)
		m_EyesHighlight02Map->SetResource(ResourceManager::GetInstance()->GetTextureResource(textureInfo.m_EyesHighlight02MapID)->GetSRV().Get());
	else
		m_EyesHighlight02Map->SetResource(nullptr);
}

void UmaMusumeEyesEffect::EffectRendering(std::shared_ptr<RenderingData>& renderingData)
{
	// 인풋 레이아웃 설정
	m_DeviceContext->IASetInputLayout(m_InputLayout.Get());


	// 상수 버퍼 설정
	// Per Object Data
	SetPerObjectData(renderingData);

	// cbSkinned - BoneTransforms
	SetBoneTransforms(renderingData->m_AnimInfo->m_BoneMatrixList);

	// Texture
	SetTextureMap(renderingData->m_MaterialInfo->m_TextureInfo);


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

void UmaMusumeEyesEffect::SetMaterial(std::shared_ptr<MaterialInfo> materialInfo)
{
	// 뒤의 diffuseMapID, NormalMapID는 제외한다
	m_Material->SetRawValue(
		materialInfo.get()
		, 0
		, sizeof(MaterialInfo) - sizeof(TextureInfo)
	);
}

void UmaMusumeEyesEffect::SetBoneTransforms(std::vector<XMMATRIX>& finalBoneTMList)
{
	// 예외 처리
	if (!m_BoneTMs)
		return;
	if (finalBoneTMList.empty())
		return;

	//XMMATRIX* _M = FinalBoneTMList.data();
	m_BoneTMs->SetMatrixArray(
		reinterpret_cast<const float*>(finalBoneTMList.data())
		, 0, finalBoneTMList.size()
	);
}
