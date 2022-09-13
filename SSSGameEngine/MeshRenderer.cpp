#include "GameEnginePCH.h"
#include "MeshRenderer.h"

#include "RenderingManager.h"
#include "SceneManager.h"
#include "SceneBase.h"

#include "Transform.h"
#include "Camera.h"

MeshRenderer::MeshRenderer(std::weak_ptr<GameObject> _GameObj)
	: RendererBase(_GameObj, "MeshRenderer")
{
	m_RenderingData->m_DataType = RenderingdataType::STATIC_MESH_OBJECT;
	m_RenderingData->m_ObjectInfo->m_ObjectID = GetGameObject()->GetObjectID();
}

MeshRenderer::~MeshRenderer()
{

}

void MeshRenderer::Release()
{
	__super::Release();
}

void MeshRenderer::Render()
{
	// ���� ������ ī�޶� ���� ������ ������Ʈ�ؼ� �׷��Ƚ� �Ŵ������� �����͸� ������
	// ī�޶� ����
	std::shared_ptr<Camera> mainCam = SceneManager::GetInstance()->GetCurrentScene()->GetMainCamera();

	// ���� ī�޶� �̼��� ��, ���� �߻�
	assert(mainCam);

	// ��ȯ ��� ����
	SimpleMath::Matrix worldTM = m_Transform.lock()->GetWorldTM();
	m_RenderingData->m_GeoInfo->m_World = worldTM;
	m_RenderingData->m_GeoInfo->m_TexTransform = ::XMMatrixIdentity();
	m_RenderingData->m_GeoInfo->m_View = mainCam->GetViewMatrix();
	m_RenderingData->m_GeoInfo->m_Proj = mainCam->GetProjMatrix();
	m_RenderingData->m_GeoInfo->m_WorldViewProj = worldTM * mainCam->GetViewProjMatrix();
	m_RenderingData->m_GeoInfo->m_WorldInvTranspose = (worldTM.Invert()).Transpose();

	// ������ �Ŵ������� ������ �����͸� ������.
	RenderingManager::GetInstance()->PostRenderingData(m_RenderingData);
}
