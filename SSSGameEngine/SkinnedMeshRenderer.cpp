#include "GameEnginePCH.h"
#include "SkinnedMeshRenderer.h"

#include "RenderingManager.h"
#include "SceneManager.h"
#include "SceneBase.h"

#include "Transform.h"
#include "Camera.h"
#include "Armature.h"

SkinnedMeshRenderer::SkinnedMeshRenderer(std::weak_ptr<GameObject> _GameObj)
	: RendererBase(_GameObj, "SkinnedMeshRenderer")
{
	m_RenderingData->m_DataType = RenderingdataType::STATIC_MESH_OBJECT;
	m_RenderingData->m_ObjectInfo->m_ObjectID = GetGameObject()->GetObjectID();
}

SkinnedMeshRenderer::~SkinnedMeshRenderer()
{

}

void SkinnedMeshRenderer::Release()
{
	__super::Release();
}

void SkinnedMeshRenderer::Render()
{
	// 데이터들을 정리해서 그래픽스 매니저에게 보낸다.
	auto armature = GetGameObject()->GetComponent<Armature>();
	auto& boneMatrixList = armature->GetBoneMatrixList();
	
	// 본 매트릭스의 데이터를 복사(이동)
	m_RenderingData->m_AnimInfo->m_BoneMatrixList.clear();
	for (auto& item : boneMatrixList)
	{
		m_RenderingData->m_AnimInfo->m_BoneMatrixList.emplace_back(std::move(item));
	}

	// 기하 정보와 카메라 관련 정보를 업데이트해서 그래픽스 매니저에게 데이터를 보낸다
	// 카메라 정보
	std::shared_ptr<Camera> mainCam = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera();

	// 메인 카메라 미설정 시, 예외 발생
	assert(mainCam);

	// 변환 행렬 정보
	SimpleMath::Matrix worldTM = m_Transform.lock()->GetWorldTM();
	m_RenderingData->m_GeoInfo->m_World = worldTM;
	m_RenderingData->m_GeoInfo->m_TexTransform = ::XMMatrixIdentity();
	m_RenderingData->m_GeoInfo->m_View = mainCam->GetViewMatrix();
	m_RenderingData->m_GeoInfo->m_Proj = mainCam->GetProjMatrix();
	m_RenderingData->m_GeoInfo->m_WorldViewProj = worldTM * mainCam->GetViewProjMatrix();
	m_RenderingData->m_GeoInfo->m_WorldInvTranspose = (worldTM.Invert()).Transpose();

	// 렌더링 매니저에게 렌더링 데이터를 보낸다.
	RenderingManager::GetInstance()->PostRenderingData(m_RenderingData);
}
