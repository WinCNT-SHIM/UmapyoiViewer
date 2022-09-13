#include "GraphicsEnginePCH.h"
#include "BasicSkinnedEffect.h"

using namespace GraphicsEngine;

BasicSkinnedEffect::BasicSkinnedEffect(std::shared_ptr<DXDevice> device, string m_FilePath)
	: EffectBase(device, m_FilePath, _T("BasicSkinned"))
{
	// ����Ʈ���� PosColorTech��� ��ũ�п� ���� �����͸� ȹ��
	m_Tech = m_FX->GetTechniqueByName("BasicSkinnedTech");
	

	// ����Ʈ�� ��� ���ۿ� ���� ������ ȹ��
	// Per Object Data
	m_World = m_FX->GetVariableByName("gWorld")->AsMatrix();
	m_WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
	m_WorldInvTranspose = m_FX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	m_TexTransform = m_FX->GetVariableByName("gTexTransform")->AsMatrix();
	m_Material = m_FX->GetVariableByName("gMaterial");
	m_ObjectID = m_FX->GetVariableByName("gObjectID");

	// cbSkinned - BoneTransforms
	m_BoneTMs = m_FX->GetVariableByName("gBoneTransforms")->AsMatrix();

	// Texture
	m_DiffuseMap = m_FX->GetVariableByName("gDiffuseMap")->AsShaderResource();


	// ��ǲ ���̾ƿ� ����
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

BasicSkinnedEffect::~BasicSkinnedEffect()
{
}

void BasicSkinnedEffect::Release()
{
	m_World.Reset();
	m_WorldViewProj.Reset();
	m_WorldInvTranspose.Reset();
	m_TexTransform.Reset();
	m_Material.Reset();
	m_ObjectID.Reset();

	m_BoneTMs.Reset();

	m_DiffuseMap.Reset();

	__super::Release();
}

void BasicSkinnedEffect::SetPerObjectData(std::shared_ptr<RenderingData>& renderingData)
{
	SetWorld(renderingData->m_GeoInfo->m_World);
	SetWorldViewProj(renderingData->m_GeoInfo->m_WorldViewProj);
	SetWorldInvTranspose(renderingData->m_GeoInfo->m_WorldInvTranspose);
	SetTexTransform(renderingData->m_GeoInfo->m_TexTransform);
	SetMaterial(renderingData->m_MaterialInfo);
	SetObjectID(renderingData->m_ObjectInfo->m_ObjectID);
}

void BasicSkinnedEffect::EffectRendering(std::shared_ptr<RenderingData>& renderingData)
{
	// ��ǲ ���̾ƿ� ����
	m_DeviceContext->IASetInputLayout(m_InputLayout.Get());


	// ��� ���� ����
	// Per Object Data
	SetPerObjectData(renderingData);

	// cbSkinned - BoneTransforms
	SetBoneTransforms(renderingData->m_AnimInfo->m_BoneMatrixList);

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

void BasicSkinnedEffect::SetMaterial(std::shared_ptr<MaterialInfo> materialInfo)
{
	// ���� diffuseMapID, NormalMapID�� �����Ѵ�
	m_Material->SetRawValue(
		materialInfo.get()
		, 0
		, sizeof(MaterialInfo) - sizeof(TextureInfo)
	);
}

void BasicSkinnedEffect::SetBoneTransforms(std::vector<XMMATRIX>& finalBoneTMList)
{
	// ���� ó��
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
