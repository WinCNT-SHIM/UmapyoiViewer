/// <summary>
/// 렌더링 패스의 인터페이스 클래스
/// </summary>
#pragma once

#include "RenderingPassBase.h"
#include "GraphicsRenderingData.h"

namespace GraphicsEngine
{
	class Mesh;
	class SwapChain;
	class RenderTarget;

	class ForwardRenderingPass
		: public RenderingPassBase
	{
	private:
		std::shared_ptr<SwapChain> m_SwapChain;
		std::shared_ptr<RenderTarget> m_RenderTarget;

	public:
		virtual void Init(const uint32& screenWidth, const uint32& screenHeight) override;
		virtual void OnResize(const uint32& screenWidth, const uint32& screenHeight) override;
		virtual void Release() override;

		void BeginRender();
		void EndRender();
		virtual void Render(std::shared_ptr<PerFrameData> m_PerFrameData, std::vector<std::shared_ptr<RenderingData>>& renderQueue);

	private:
		void ExecuteStaticMeshObject(std::shared_ptr<RenderingData>& renderingData);
		void ExecuteForSkinnedMeshObject(std::shared_ptr<RenderingData> renderingData);
	};
}