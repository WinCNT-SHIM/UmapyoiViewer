#include "GameEnginePCH.h"
#include "RenderingManager.h"

#include "ResourceManager.h"
#include "SceneManager.h"
#include "SceneBase.h"

#include "Camera.h"

std::shared_ptr<RenderingManager> RenderingManager::m_Instance = nullptr;

void RenderingManager::PostRenderingData(std::shared_ptr<RenderingData>& renderingData)
{
	m_RenderQueue.emplace(renderingData);
}

void RenderingManager::PostDirectionalLightInfo(std::shared_ptr<DirectionalLightInfo> dirLightInfo)
{
	m_DirectionalLightInfoQueue.emplace(dirLightInfo);
}

void RenderingManager::DispatchPerFrameData()
{
	std::shared_ptr<Camera> _mainCamera = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera();

	if (_mainCamera != nullptr)
	{
		memcpy(
			m_PerframeData->m_CameraInfo.get()
			, _mainCamera->m_CameraInfo.get()
			, sizeof(CameraInfo)
		);

		//std::shared_ptr<BoundingFrustum> _viewFrustum = _mainCamera->GetViewFrustum();
		//memcpy(_perframeData->m_SSAOInfo->m_CameraFrustumCorner, _viewFrustum->_frustumCorner, sizeof(SimpleMath::Vector4) * 4);
	}

	while (!m_DirectionalLightInfoQueue.empty())
	{
		std::shared_ptr<DirectionalLightInfo> dirInfo = m_DirectionalLightInfoQueue.front();
		m_DirectionalLightInfoQueue.pop();

		m_PerframeData->m_DirectionalLightInfos.emplace_back(dirInfo);
	}

	//while (!_pointLightInfoQueue.empty())
	//{
	//	std::shared_ptr<PointLightInfo> pointLightInfo = _pointLightInfoQueue.front(); _pointLightInfoQueue.pop();

	//	_perframeData->_pointLightInfos.push_back(pointLightInfo);
	//}

	//while (!_spotLightInfoQueue.empty())
	//{
	//	std::shared_ptr<SpotLightInfo> spotLightInfo = _spotLightInfoQueue.front(); _spotLightInfoQueue.pop();

	//	_perframeData->_spotLightInfos.push_back(spotLightInfo);
	//}

	m_GraphicsEngine->PostPerFrameData(m_PerframeData);
}

void RenderingManager::DispatchRenderingData()
{
	std::shared_ptr<Camera> mainCamera = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera();

	while (!m_RenderQueue.empty())
	{
		std::shared_ptr<RenderingData>& renderingData = m_RenderQueue.front();
		m_RenderQueue.pop();

		m_GraphicsEngine->PostRenderingData(renderingData);

		// �ø��� ���� ����...
		//if (mainCamera->ViewFrustumCulling(boundingCubeInfo, renderingData->_geoInfo->_world))
		//{
		//	_graphicsEngine->PostRenderingData(renderingData);
		//}
	}
}

void RenderingManager::Init()
{
	// �׷��Ƚ� ������ ���� �����͸� �����صΰ� ����
	m_GraphicsEngine = IGraphicsEngineBase::GetGraphicsEngine();

	// ���� �ʱ�ȭ
	// �����Ӹ��� �ʿ��� �������̹Ƿ� ��Ȱ���� ������
	m_PerframeData = std::make_shared<PerFrameData>();
}

void RenderingManager::Update()
{

}

void RenderingManager::Render()
{
	DispatchPerFrameData();			// �����Ӹ��� �ʿ��� ������ ����
	DispatchRenderingData();		// ������ ������ ����
	m_GraphicsEngine->Render();		// ���� Draw Call�� �Ƿ�
}

void RenderingManager::Release()
{
	m_GraphicsEngine->Release();
}
