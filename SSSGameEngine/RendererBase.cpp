#include "GameEnginePCH.h"
#include "RendererBase.h"

#include "Transform.h"
#include "SceneManager.h"
#include "SceneBase.h"

RendererBase::RendererBase(std::weak_ptr<GameObject> _GameObj, std::string _CmpName)
	: ComponentBase(_GameObj, _CmpName)
{
	m_Transform = GetGameObject()->GetComponent<Transform>();
	m_RenderingData = std::make_shared<RenderingData>();
}

RendererBase::~RendererBase()
{

}

void RendererBase::Release()
{
	//m_Transform.reset();
	m_RenderingData.reset();
	__super::Release();
}

void RendererBase::Init()
{
	// �ʱ�ȭ ��, �� ������Ʈ�� �����ϴ� ���� �������
	SceneManager::GetInstance()->GetCurrentScene()->AddRenderer(this->shared_from_this());
}
