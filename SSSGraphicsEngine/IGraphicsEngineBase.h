/// <summary>
/// 그래픽스 엔진의 인터페이스
/// 다양한 환경의 그래픽스 엔진을 교체할 수 있도록 추상화...을 노렸으나
/// 이제 잘 모르겠다
/// 
/// Created: 2022. 6. 27 심승수
/// Updated: 2022. 6. 30 심승수
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


// ==> 메모리 관리가 힘들어서 shared_ptr로 변경...
///// <summary>
///// 그래픽스 엔진의 객체 생성, 해제하는 함수
///// (현재는 DX11로만 생성한다)
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
	// 엔진을 초기화한다.
	virtual bool Init(void* hInst, void* hWnd, int screenWidth, int screenHeight) abstract;

	// 엔진을 종료한다.
	virtual void Release() abstract;

	// 업데이트
	virtual void Update(float deltaTime) abstract;

	// 랜더링 시작
	virtual void BeginRender() abstract;

	// 렌더링 관련
	virtual void PostPerFrameData(std::shared_ptr<PerFrameData> perFrameData) abstract;
	virtual void PostRenderingData(std::shared_ptr<RenderingData> renderingData) abstract;

	// 랜더링
	virtual void Render() abstract;

	// Post Processing
	virtual void PostProcess() abstract;

	// 랜더링 끝 (백버퍼에서 화면으로 Blit, 페이지플립핑, Present등...)
	virtual void EndRender() abstract;

public:
	// 랜더러 개발 중 테스트 함수
	virtual void Draw_Test() abstract;

	// 창 변환 관련
	virtual void OnResize(const int& screenWidth, const int& screenHeight) abstract;
	virtual bool IsValidDevice() abstract;
	virtual void SetClientSize(int width, int height) abstract;
	virtual void SetWinMinMax(bool isMinimized, bool isMaxmized) abstract;

	// 마우스 입력. 윈도 메시지 기반
	virtual void OnMouseDown(int x, int y) abstract;
	virtual void OnMouseUp(int x, int y) abstract;
	virtual void OnMouseMove(int btnState, int x, int y) abstract;

	// 리소스 관련
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
