#pragma once
#include "EffectBase.h"

namespace GraphicsEngine
{
	class BasicEffect
		: public EffectBase
	{
	public:
		BasicEffect(std::shared_ptr<DXDevice> device, string m_FilePath);
		virtual ~BasicEffect();

	private:
		//////////////////////////////////////////////////////////////////////////
		// ��� ����
		//////////////////////////////////////////////////////////////////////////
		// ���̴��� ��� ���ۿ� ���� ������//Per Object Data
		ComPtr<ID3DX11EffectMatrixVariable> m_World;
		ComPtr<ID3DX11EffectMatrixVariable> m_WorldViewProj;
		ComPtr<ID3DX11EffectMatrixVariable> m_WorldInvTranspose;
		ComPtr<ID3DX11EffectMatrixVariable> m_TexTransform;
		ComPtr<ID3DX11EffectVariable> m_Material;
		ComPtr<ID3DX11EffectVariable> m_ObjectID;

		ComPtr<ID3DX11EffectShaderResourceVariable> m_DiffuseMap;

	private:
		// ==================== ���̴��� ��� ���ۿ� �����ϱ� ���� �Լ� ====================
		// Per Frame Data

		//Per Object Data
		void SetWorld(CXMMATRIX M) { m_World->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetWorldViewProj(CXMMATRIX M) { m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetWorldInvTranspose(CXMMATRIX M) { m_WorldInvTranspose->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetTexTransform(CXMMATRIX M) { m_TexTransform->SetMatrix(reinterpret_cast<const float*>(&M)); }
		void SetMaterial(std::shared_ptr<MaterialInfo> materialInfo);
		void SetObjectID(const uint32& objectID) { m_ObjectID->SetRawValue(&objectID, 0, sizeof(uint32)); }

		//void SetEyePosW(const XMFLOAT3& v) { EyePosW->SetRawValue(&v, 0, sizeof(XMFLOAT3)); }
		//void SetDirLights(const DirectionalLight* lights) { m_WorldDirLights->SetRawValue(lights, 0, 3 * sizeof(DirectionalLight)); }

		// �ؽ��� �� �߰�
		void SetDiffuseMap(ID3D11ShaderResourceView* tex) { m_DiffuseMap->SetResource(tex); }
		//void SetDiffuseMap(ID3D11ShaderResourceView* tex) { m_DiffuseMap->SetResource(tex); }

		//// ��� ��
		//void SetNormalMap(ID3D11ShaderResourceView* _map) { NormalMap->SetResource(_map); }

		//// ȯ�� ��(ť�� ��)
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

	public:
		virtual void Release();
		void UpdatePerFrameData(std::shared_ptr<PerFrameData>& m_PerFrameData) override {}
		virtual void EffectRendering(std::shared_ptr<RenderingData>& renderingData) override;
	};
}