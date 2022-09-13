#include "GameEnginePCH.h"
#include "GameObject.h"

#include "ComponentBase.h"
#include "Transform.h"

UINT32 GameObject::m_LastID = 0;
GameObject::GameObject()
	: m_ID{ ++m_LastID }
	, m_Name{ "GameObject" }
	, m_Tag{ "Default" }
	, m_Layer{ 0 }
{
}

GameObject::GameObject(std::string _GameObjName)
	: m_ID{ ++m_LastID }
	, m_Name{ _GameObjName }
	, m_Tag{ "" }
	, m_Layer{ 0 }
{

}

GameObject::~GameObject()
{

}

void GameObject::Init()
{
	for (auto& component : m_Components)
		if (component->GetIsEnable() == true)
			component->Init();
}

void GameObject::FixedUpdate(float deltaTime)
{
	for (auto& component : m_Components)
		if (component->GetIsEnable() == true)
			component->FixedUpdate(deltaTime);
}

void GameObject::Update(float deltaTime)
{
	for (auto& component : m_Components)
		if (component->GetIsEnable() == true)
			component->Update(deltaTime);
}

void GameObject::LateUpdate(float deltaTime)
{
	for (auto& component : m_Components)
		if (component->GetIsEnable() == true)
			component->LateUpdate(deltaTime);
}

void GameObject::Render()
{
	for (auto& component : m_Components)
		if (component->GetIsEnable() == true)
			component->Render();
}

void GameObject::Release()
{
	if (!m_Parent.expired())
		m_Parent.lock().reset();

	m_Transform->Release();

	// 부모 객체가 사라지면 자식 객체도 사라진다
	for (auto& child : m_Children)
		child->Release();

	// 부품도 객체 종속적인 것이니 사라진다
	for (auto& component : m_Components)
		component->Release();

	// 벡터도 비워둔다
	m_Children.clear();
	m_Components.clear();
}

void GameObject::SetParent(std::shared_ptr<GameObject> _parent)
{
	// 기존의 부모 객체가 있었다면 바꿔준다.
	this->ResetParent();

	m_Parent = _parent;
}

std::shared_ptr<GameObject> GameObject::GetChild(const std::string& name)
{
	// 이름에 해당하는 자식을 찾아서 반환
	for (const auto& child : m_Children)
		if (child->GetName() == name)
			return child;

	return nullptr;
}

void GameObject::SetChild(std::shared_ptr<GameObject> _child)
{
	// 이미 자식 목록에 본인이 있다면 그냥 Return
	for (auto& child : m_Children)
		if (child == _child)
			return;

	// 없으면 자식으로 세팅함
	m_Children.emplace_back(_child);
	_child->SetParent(shared_from_this());
}

void GameObject::SetChildren(std::vector<std::shared_ptr<GameObject>> children)
{
	for (auto& newChild : children)
	{
		// 이미 자식 목록에 본인이 있다면 그냥 Return
		for (auto& child : m_Children)
			if (child == newChild)
				continue;

		// 없으면 자식으로 세팅함
		m_Children.emplace_back(newChild);
		newChild->SetParent(shared_from_this());
	}
}

std::shared_ptr<GameObject> GameObject::FindChildInAllChildren(const std::string& name)
{
	if (this->GetName() == name)
		return shared_from_this();

	for (auto& child : m_Children)
	{
		std::shared_ptr<GameObject> res = child->FindChildInAllChildren(name);
		if (res != nullptr)
			return res;
	}

	return nullptr;
}

void GameObject::ResetParent()
{
	if (m_Parent.lock() == nullptr)
		return;

	auto& parentsChildren = m_Parent.lock()->m_Children;

	std::shared_ptr<GameObject> sharedThis = shared_from_this();

	for (int i = 0; i < parentsChildren.size(); i++)
	{
		// 부모 자식 목록에서 나를 지워준다.
		if (sharedThis == parentsChildren[i])
		{
			parentsChildren.erase(parentsChildren.begin() + i);
			break;
		}
	}

	// 부모 포인터를 널 포인터로 바꾼다
	m_Parent = std::weak_ptr<GameObject>();
}
