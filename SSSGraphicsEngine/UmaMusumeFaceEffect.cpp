#include "GraphicsEnginePCH.h"
#include "UmaMusumeFaceEffect.h"

using namespace GraphicsEngine;

UmaMusumeFaceEffect::UmaMusumeFaceEffect(std::shared_ptr<DXDevice> device, string m_FilePath)
	: EffectBase(device, m_FilePath, _T("UmaMusumeFace"))
{
	// 이펙트에서 PosColorTech라는 테크닉에 대한 포인터를 획득
	m_Tech = m_FX->GetTechniqueByName("UmaMusumeFaceTech");
	

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
	m_TripleMaskMap = m_FX->GetVariableByName("gTripleMaskMap")->AsShaderResource();
	m_OptionMaskMap = m_FX->GetVariableByName("gOptionMaskMap")->AsShaderResource();
	m_ToonShadowMap = m_FX->GetVariableByName("gToonShadowMap")->AsShaderResource();
	m_ToonShadowMap = m_FX->GetVariableByName("gToonShadowMap")->AsShaderResource();
	m_EnvironmentMap = m_FX->GetVariableByName("gEnvironmentMap")->AsShaderResource();


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

UmaMusumeFaceEffect::~UmaMusumeFaceEffect()
{
}

void UmaMusumeFaceEffect::Release()
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
	m_TripleMaskMap.Reset();
	m_OptionMaskMap.Reset();
	m_ToonShadowMap.Reset();
	m_EnvironmentMap.Reset();

	__super::Release();
}

void UmaMusumeFaceEffect::SetPerObjectData(std::shared_ptr<RenderingData>& renderingData)
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

void UmaMusumeFaceEffect::SetTextureMap(TextureInfo& textureInfo)
{
	// diff
	if (textureInfo.m_DiffuseMapID != UINT_MAX)
		m_DiffuseMap->SetResource(ResourceManager::GetInstance()->GetTextureResource(textureInfo.m_DiffuseMapID)->GetSRV().Get());
	else
		m_DiffuseMap->SetResource(nullptr);


	// base
	if (textureInfo.m_TripleMaskMapID != UINT_MAX)
		m_TripleMaskMap->SetResource(ResourceManager::GetInstance()->GetTextureResource(textureInfo.m_TripleMaskMapID)->GetSRV().Get());
	else
		m_TripleMaskMap->SetResource(nullptr);


	// ctrl
	if (textureInfo.m_OptionMaskMapID != UINT_MAX)
		m_OptionMaskMap->SetResource(ResourceManager::GetInstance()->GetTextureResource(textureInfo.m_OptionMaskMapID)->GetSRV().Get());
	else
		m_OptionMaskMap->SetResource(nullptr);


	// shad_c
	if (textureInfo.m_ToonShadowMapID != UINT_MAX)
		m_ToonShadowMap->SetResource(ResourceManager::GetInstance()->GetTextureResource(textureInfo.m_ToonShadowMapID)->GetSRV().Get());
	else
		m_ToonShadowMap->SetResource(nullptr);


	// 환경 맵
	if (textureInfo.m_EnvironmentMapID != UINT_MAX)
		m_EnvironmentMap->SetResource(ResourceManager::GetInstance()->GetTextureResource(textureInfo.m_EnvironmentMapID)->GetSRV().Get());
	else
		m_EnvironmentMap->SetResource(nullptr);
}

void UmaMusumeFaceEffect::EffectRendering(std::shared_ptr<RenderingData>& renderingData)
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

void UmaMusumeFaceEffect::SetMaterial(std::shared_ptr<MaterialInfo> materialInfo)
{
	// 뒤의 diffuseMapID, NormalMapID는 제외한다
	m_Material->SetRawValue(
		materialInfo.get()
		, 0
		, sizeof(MaterialInfo) - sizeof(TextureInfo)
	);
}

void UmaMusumeFaceEffect::SetBoneTransforms(std::vector<XMMATRIX>& finalBoneTMList)
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
