/// <summary>
/// �׷��Ƚ� ������ �������̽�
/// �پ��� ȯ���� �׷��Ƚ� ������ ��ü�� �� �ֵ��� �߻�ȭ...�� �������
/// ���� �� �𸣰ڴ�
/// 
/// Created: 2022. 6. 27 �ɽ¼�
/// Updated: 2022. 6. 30 �ɽ¼�
/// </summary>
#pragma once

#include "DX11Define.h"
#include "GraphicsRenderingData.h"
#include "VertexStruct.h"

#ifdef _SSSEngine_EXPORTS
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#endif

class IGraphicsEngineBase;


// ==> �޸� ������ ���� shared_ptr�� ����...
///// <summary>
///// �׷��Ƚ� ������ ��ü ����, �����ϴ� �Լ�
///// (����� DX11�θ� �����Ѵ�)
///// </summary>
//#ifdef __cplusplus
//extern "C"
//{
//	//DLL_DECLSPEC shared_ptr<IGraphicsEngineBase> CreateGraphicsEngine();
//	//DLL_DECLSPEC void ReleaseGraphicsEngine(IGraphicsEngineBase* pModule);
//	//DLL_DECLSPEC shared_ptr<IGraphicsEngineBase> GetGraphicsEngine();
//}
//#endif//__cplusplus

class DLL_DECLSPEC IGraphicsEngineBase
{
public:
	IGraphicsEngineBase() {}
	virtual ~IGraphicsEngineBase() {};

public:
	static std::shared_ptr<IGraphicsEngineBase> m_Instance;
	static std::shared_ptr<IGraphicsEngineBase> CreateGraphicsEngine();
	static std::shared_ptr<IGraphicsEngineBase> GetGraphicsEngine();

public:
	// ������ �ʱ�ȭ�Ѵ�.
	virtual bool Init(void* hInst, void* hWnd, int screenWidth, int screenHeight) abstract;

	// ������ �����Ѵ�.
	virtual void Release() abstract;

	// ������Ʈ
	virtual void Update(float deltaTime) abstract;

	// ������ ����
	virtual void BeginRender() abstract;

	// ������ ����
	virtual void PostPerFrameData(std::shared_ptr<PerFrameData> perFrameData) abstract;
	virtual void PostRenderingData(std::shared_ptr<RenderingData> renderingData) abstract;

	// ������
	virtual void Render() abstract;

	// Post Processing
	virtual void PostProcess() abstract;

	// ������ �� (����ۿ��� ȭ������ Blit, �������ø���, Present��...)
	virtual void EndRender() abstract;

public:
	// ������ ���� �� �׽�Ʈ �Լ�
	virtual void Draw_Test() abstract;

	// â ��ȯ ����
	virtual void OnResize(const int& screenWidth, const int& screenHeight) abstract;
	virtual bool IsValidDevice() abstract;
	virtual void SetClientSize(int width, int height) abstract;
	virtual void SetWinMinMax(bool isMinimized, bool isMaxmized) abstract;

	// ���콺 �Է�. ���� �޽��� ���
	virtual void OnMouseDown(int x, int y) abstract;
	virtual void OnMouseUp(int x, int y) abstract;
	virtual void OnMouseMove(int btnState, int x, int y) abstract;

	// ���ҽ� ����
	virtual const uint32 LoadMesh(
		const std::string& meshName
		, bool isDynamic = false
		, bool isLineList = false
	) abstract;

	virtual const uint32 LoadMesh(
		const std::string& meshName
		, const std::vector<GraphicsEngine::VertexStruct::VertexBasic>& vertexVector
		, const std::vector<uint32>& indexVector
		, bool isDynamic = false
		, bool isLineList = false
	) abstract;

	virtual const uint32 LoadMesh(
		const std::string& meshName
		, const std::vector<GraphicsEngine::VertexStruct::VertexBasicSkinned>& vertexVector
		, const std::vector<uint32>& indexVector
		, bool isDynamic = false
		, bool isLineList = false
	) abstract;

	virtual const uint32 LoadTexture(const std::string& path) abstract;

	virtual void ReleaseResourceOnly() abstract;

};
