/// <summary>
/// DX11로 돌아가는 그래픽스 엔진
/// 
/// Created: 2022/06/27 심승수
/// Updated: 2022/06/27 심승수
/// </summary>
#pragma once

#define  _DX11RENDERER
#include "IDefine.h"

#include "IGraphicsEngineBase.h"
#include "RenderStruct.h"

class Renderer;

namespace GraphicsEngine
{
	class DX11GraphicsEngine :
		public IGraphicsEngineBase
	{
	//////////////////////////////////////////////////////////////////////////
	// 생성자 및 소멸자
	//////////////////////////////////////////////////////////////////////////
	public:
		DX11GraphicsEngine();
		virtual	~DX11GraphicsEngine();


	//////////////////////////////////////////////////////////////////////////
	// 변수
	//////////////////////////////////////////////////////////////////////////
	private:
		// 생성 시 인수를 보관
		HWND m_hWnd;

	//////////////////////////////////////////////////////////////////////////
	// 함수
	//////////////////////////////////////////////////////////////////////////
	public:
		bool Init(void* hInst, void* hWnd, int screenWidth, int screenHeight) override;
		void Release() override;

		void Update(float deltaTime) override;

		void BeginRender() override;
		void PostPerFrameData(std::shared_ptr<PerFrameData> perFrameData) override;
		void PostRenderingData(std::shared_ptr<RenderingData> renderingData) override;
		void Render() override;
		void PostProcess() override;
		void EndRender() override;

	public:
		void Draw_Test() override;

		void OnResize(const int& screenWidth, const int& screenHeight) override;
		bool IsValidDevice() override;
		void SetClientSize(int width, int height) override;
		void SetWinMinMax(bool isMinimized, bool isMaxmized) override;

		// 마우스 입력. 윈도 메시지 기반
		void OnMouseDown(int x, int y) override;
		void OnMouseUp(int x, int y) override;
		void OnMouseMove(int btnState, int x, int y) override;

		// 리소스 관련
		// Mesh
		const uint32 LoadMesh(
			const std::string& meshName
			, bool isDynamic = false
			, bool isLineList = false
		) override;

		const uint32 LoadMesh(
			const std::string& meshName
			, const std::vector<GraphicsEngine::VertexStruct::VertexBasic>& vertexVector
			, const std::vector<uint32>& indexVector
			, bool isDynamic = false
			, bool isLineList = false
		) override;

		const uint32 LoadMesh(
			const std::string& meshName
			, const std::vector<GraphicsEngine::VertexStruct::VertexBasicSkinned>& vertexVector
			, const std::vector<uint32>& indexVector
			, bool isDynamic = false
			, bool isLineList = false
		) override;

		// Texture
		const uint32 LoadTexture(const std::string& path) override;

		// 리소스만 릴리스
		void ReleaseResourceOnly() override;
	};
}
