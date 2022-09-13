#pragma once

#include "EffectBase.h"

namespace GraphicsEngine
{
	class PosColorEffect
		: public EffectBase
	{
	public:
		PosColorEffect(std::shared_ptr<DXDevice> device, string m_FilePath);
		virtual ~PosColorEffect();

	private:
		//////////////////////////////////////////////////////////////////////////
		// ��� ����
		//////////////////////////////////////////////////////////////////////////
		// ���̴��� ��� ���ۿ� ���� ������
		//Per Object Data
		ComPtr<ID3DX11EffectMatrixVariable> m_WorldViewProj;

	private:
		// ==================== ���̴��� ��� ���ۿ� �����ϱ� ���� �Լ� ====================
		void SetWorldViewProj(CXMMATRIX M) { m_WorldViewProj->SetMatrix(reinterpret_cast<const float*>(&M)); }

	public:
		virtual void UpdatePerFrameData(std::shared_ptr<PerFrameData>& m_PerFrameData) override {}
		virtual void EffectRendering(std::shared_ptr<RenderingData>& renderingData) override;
		virtual void Release();
	};
}
