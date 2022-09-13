/// <summary>
/// 실제 렌더링(그리기)에 대한 관리를 하는 클래스
/// 
/// 컬링을 하거나, 실제로 그릴 오브젝트의 정보를 렌더 큐에 보관하고
/// 그래픽스 엔진에 보내는 등의 처리를 하는 역할을 한다
/// 
/// 그래픽스 엔진과 직접적인 연결이 있는 클래스
/// (물론 리소스 매니저도 직접적인 연결이 있는 상태긴 하다...)
/// 
/// Created : 2022/08/07 심승수
/// Updated : 2022/08/07 심승수
/// </summary>
#pragma once

// 그래픽스 엔진에 커넥트(...)하기 위한 헤더
#ifdef _DX11RENDERER
#include "../SSSGraphicsEngine/IGraphicsEngineBase.h"
#include "../SSSGraphicsEngine/DX11GraphicsEngine.h"
#include "../SSSGraphicsEngine/GraphicsRenderingData.h"
#endif

class RenderingManager
{
	//싱글톤으로 만들자
public:
	RenderingManager() = default;
	RenderingManager(const RenderingManager& ref) = delete;
	RenderingManager& operator=(const RenderingManager& ref) = delete;
	~RenderingManager() { /*Release();*/ }
private:
	static std::shared_ptr<RenderingManager> m_Instance;
public:
	static std::shared_ptr<RenderingManager> GetInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = std::make_shared<RenderingManager>();
			m_Instance->Init();
		}
		return m_Instance;
	}

private:
	// 그래픽스 엔진 캐싱
	std::shared_ptr<IGraphicsEngineBase> m_GraphicsEngine;

	std::shared_ptr<PerFrameData> m_PerframeData;
	std::queue<std::shared_ptr<RenderingData>> m_RenderQueue;
	std::queue<std::shared_ptr<DirectionalLightInfo>> m_DirectionalLightInfoQueue;
	//std::queue<std::shared_ptr<PointLightInfo>> _pointLightInfoQueue;
	//std::queue<std::shared_ptr<SpotLightInfo>> _spotLightInfoQueue;
	//std::queue<std::shared_ptr<TextData>> _textDataQueue;

public:
	void PostRenderingData(std::shared_ptr<RenderingData>& renderingData);
	void PostDirectionalLightInfo(std::shared_ptr<DirectionalLightInfo> dirLightInfo);
	//void PostPointLightInfo(std::shared_ptr<PointLightInfo> pointLightInfo);
	//void PostSpotLightInfo(std::shared_ptr<SpotLightInfo> spotLightInfo);
	//void PostTextData(std::shared_ptr<TextData> textData);
	//const BoundingRect&& MeasureDrawBounds(std::shared_ptr<TextData> textData);

private:
	void DispatchPerFrameData();
	void DispatchRenderingData();
	void DispatchTextData();

public:
	void Init();
	void Update();
	void Render();
	void Release();
};

