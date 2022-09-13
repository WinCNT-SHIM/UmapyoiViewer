/// <summary>
/// ���� ������(�׸���)�� ���� ������ �ϴ� Ŭ����
/// 
/// �ø��� �ϰų�, ������ �׸� ������Ʈ�� ������ ���� ť�� �����ϰ�
/// �׷��Ƚ� ������ ������ ���� ó���� �ϴ� ������ �Ѵ�
/// 
/// �׷��Ƚ� ������ �������� ������ �ִ� Ŭ����
/// (���� ���ҽ� �Ŵ����� �������� ������ �ִ� ���±� �ϴ�...)
/// 
/// Created : 2022/08/07 �ɽ¼�
/// Updated : 2022/08/07 �ɽ¼�
/// </summary>
#pragma once

// �׷��Ƚ� ������ Ŀ��Ʈ(...)�ϱ� ���� ���
#ifdef _DX11RENDERER
#include "../SSSGraphicsEngine/IGraphicsEngineBase.h"
#include "../SSSGraphicsEngine/DX11GraphicsEngine.h"
#include "../SSSGraphicsEngine/GraphicsRenderingData.h"
#endif

class RenderingManager
{
	//�̱������� ������
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
	// �׷��Ƚ� ���� ĳ��
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

