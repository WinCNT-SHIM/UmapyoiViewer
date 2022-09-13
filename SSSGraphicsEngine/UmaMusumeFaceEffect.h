#pragma once
#include "EffectBase.h"

namespace GraphicsEngine
{
	class UmaMusumeFaceEffect
		: public EffectBase
	{
	public:
		UmaMusumeFaceEffect(std::shared_ptr<DXDevice> device, string m_FilePath);
		virtual ~UmaMusumeFaceEffect();

	private:
		//////////////////////////////////////////////////////////////////////////
		// 상수 버퍼
		//////////////////////////////////////////////////////////////////////////
		// 셰이더의 상수 버퍼에 대한 포인터
		//Per Object Data
		ComPtr<ID3DX11EffectMatrixVariable> m_World;
		ComPtr<ID3DX11EffectMatrixVariable> m_View;
		ComPtr<ID3DX11EffectMatrixVariable> m_Proj;
		ComPtr<ID3DX11EffectMatrixVariable> m_WorldViewProj;
		ComPtr<ID3DX11EffectMatrixVariable> m_WorldInvTranspose;
		ComPtr<ID3DX11EffectMatrixVariable> m_TexTransform;
		ComPtr<ID3DX11EffectVariable> m_Material;
		ComPtr<ID3DX11EffectVariable> m_ObjectID;

		// cbSkinned - BoneTransforms
		ComPtr<ID3DX11EffectMatrixVariable> m_BoneTMs;

		ComPtr<ID3DX11EffectShaderResourceVariable> m_DiffuseMap;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_TripleMaskMap;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_OptionMaskMap;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_ToonShadowMap;
		ComPtr<ID3DX11EffectShaderResourceVariable> m_EnvironmentMap;

	private:
		// ==================== 셰이더의 상수 버퍼와 소통하기 위한 함수 ====================
		//Per Object Data
		void SetWorld(CXMMATRIX M) { m_World->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetView(CXMMATRIX M) { m_View->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetProj(CXMMATRIX M) { m_Proj->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetWorldViewProj(CXMMATRIX M) { m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetWorldInvTranspose(CXMMATRIX M) { m_WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetTexTransform(CXMMATRIX M) { m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetMaterial(std::shared_ptr<MaterialInfo> materialInfo);
		void SetObjectID(const uint32& objectID) { m_ObjectID->SetRawValue(&objectID, 0, sizeof(uint32)); }

		// cbSkinned - BoneTransform
		void SetBoneTransforms(std::vector<XMMATRIX>& finalBoneTMList);
		//void SetBoneTransforms(XMMATRIX finalBoneTMList[125]);

		//void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		//void SetDirLights(const DirectionalLight* lights) { m_WorldDirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }

		// 텍스쳐 맵 추가
		//void SetDiffuseMap(ID3D11ShaderResourceView* tex) { m_DiffuseMap->SetResource(tex); }
		//void SetDiffuseMap(ID3D11ShaderResourceView* tex) { m_DiffuseMap->SetResource(tex); }

		//// 노멀 맵
		//void SetNormalMap(ID3D11ShaderResourceView* _map) { NormalMap->SetResource(_map); }

		//// 환경 맵(큐브 맵)
		//void SetCubeMap(ID3D11ShaderResourceView* _map)
		//{
		//	if (_map != nullptr)
		//	{
		//		ReflectionEnabled->SetBool(true);
		//		CubeMap->SetResource(_map);
		//	}
		//	else
		//	{
		//		ReflectionEnabled->SetBool(false);
		//	}
		//}

	private:
		inline void SetPerObjectData(std::shared_ptr<RenderingData>& renderingData);
		inline void SetTextureMap(TextureInfo& textureInfo);

	public:
		virtual void Release();
		void UpdatePerFrameData(std::shared_ptr<PerFrameData>& m_PerFrameData) override {}
		virtual void EffectRendering(std::shared_ptr<RenderingData>& renderingData) override;
	};
}