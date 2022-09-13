/// <summary>
/// DX11�� ���ư��� �׷��Ƚ� ����
/// 
/// Created: 2022/06/27 �ɽ¼�
/// Updated: 2022/06/27 �ɽ¼�
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
	// ������ �� �Ҹ���
	//////////////////////////////////////////////////////////////////////////
	public:
		DX11GraphicsEngine();
		virtual	~DX11GraphicsEngine();


	//////////////////////////////////////////////////////////////////////////
	// ����
	//////////////////////////////////////////////////////////////////////////
	private:
		// ���� �� �μ��� ����
		HWND m_hWnd;

	//////////////////////////////////////////////////////////////////////////
	// �Լ�
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

		// ���콺 �Է�. ���� �޽��� ���
		void OnMouseDown(int x, int y) override;
		void OnMouseUp(int x, int y) override;
		void OnMouseMove(int btnState, int x, int y) override;

		// ���ҽ� ����
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

		// ���ҽ��� ������
		void ReleaseResourceOnly() override;
	};
}
