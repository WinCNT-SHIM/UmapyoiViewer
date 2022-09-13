#pragma once

#include "GraphicsRenderingData.h"

namespace GraphicsEngine
{
	class EffectBase
	{
	public:
		EffectBase() = delete;
		EffectBase(std::shared_ptr<DXDevice> device, string filename, tstring effectName);
		virtual ~EffectBase();

	private:
		EffectBase(const EffectBase& rhs);
		EffectBase& operator=(const EffectBase& rhs) = delete;

	protected:
		// ����Ʈ ��ü
		// �ϳ��� Effect�� ��� �ϳ��� ���ؽ� ���̴��� �ϳ��� �ȼ� ���̴�,
		// �׸��� �� Effect�� �����ϴµ� �ʿ��� ���� ������Ʈ�� �����ȴ�
		ComPtr<ID3DX11Effect> m_FX;

		// ���(Technique)�� ���� ������
		ComPtr<ID3DX11EffectTechnique> m_Tech;

		// ĳ�̿�
		ComPtr<ID3D11DeviceContext> m_DeviceContext;

		// ����Ʈ�� �̸�
		tstring m_Name;

		// ��ǲ ���̾ƿ�
		ComPtr<ID3D11InputLayout> m_InputLayout;

		// ��� ����
		//ID3DX11EffectConstantBuffer* m_CBPerFrame;
		ComPtr<ID3DX11EffectConstantBuffer> m_CBPerFrame;

	public:
		tstring GetName() const { return m_Name; }
		ComPtr<ID3DX11EffectConstantBuffer> GetCBPerFrame() { return m_CBPerFrame; }
		void SetCBPerFrame(ComPtr<ID3DX11EffectConstantBuffer> cbPerFrame);

	public:
		virtual void UpdatePerFrameData(std::shared_ptr<PerFrameData>& m_PerFrameData) abstract;
		virtual void EffectRendering(std::shared_ptr<RenderingData>& renderingData) abstract;

	public:
		virtual void Release();
	};
}