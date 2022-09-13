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
	// 초기화 때, 이 컴포넌트가 존재하는 씬에 등록하자
	SceneManager::GetInstance()->GetCurrentScene()->AddRenderer(this->shared_from_this());
}
