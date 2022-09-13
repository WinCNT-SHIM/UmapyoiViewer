/// <summary>
/// ���� ������Ʈ
/// ������Ʈ�� ���̴� ������Ʈ
/// 
/// 2022. 6. 20 �ɽ¼�
/// </summary>
#pragma once

#include "FactoryGameObject.h"

#ifdef _SSSEngine_EXPORTS
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#endif

class ComponentBase;
class Transform;
class Camera;

class GameObject final
	: public std::enable_shared_from_this<GameObject>
{
	//////////////////////////////////////////////////////////////////////////
	// ������ �� �Ҹ���
	//////////////////////////////////////////////////////////////////////////
private:
	// FactoryGameObject �ܿ��� new�� �� �� ������ private�� ��
	GameObject();
	GameObject(std::string _GameObjName);
	GameObject(GameObject const&) = delete;
	GameObject(GameObject&&) = delete;
	GameObject& operator=(GameObject const&) = delete;
	GameObject& operator=(GameObject&&) = delete;
	friend FactoryGameObject;
public:
	virtual ~GameObject();
	virtual void Release();

	//////////////////////////////////////////////////////////////////////////
	// ����
	//////////////////////////////////////////////////////////////////////////
private:
	// ��ü ���� ������Ʈ�� ���̵�
	// (�ϴ� ��ȯ ���� �� ���� ����! �Ѱ���� �þ��!)
	static UINT32 m_LastID;

	// ������Ʈ�� ID
	UINT32 m_ID;
	// ������Ʈ�� �̸�
	std::string m_Name;
	// �±�
	std::string m_Tag;
	// ���̾�
	UINT m_Layer;

	// ������Ʈ ����
	std::shared_ptr<Transform> m_Transform;	// Ʈ�������� �⺻���� ������ ����
	std::vector<std::shared_ptr<ComponentBase>> m_Components;

	// ����(Hierarchy) ����
	std::weak_ptr<GameObject> m_Parent;	// �θ�
	std::vector<std::shared_ptr<GameObject>> m_Children;	// �ڽ�

	//////////////////////////////////////////////////////////////////////////
	// �Լ�
	//////////////////////////////////////////////////////////////////////////
public:
	virtual void Init();
	virtual void FixedUpdate(float deltaTime);
	virtual void Update(float deltaTime);
	virtual void LateUpdate(float deltaTime);
	virtual void Render();

	std::shared_ptr<Transform> GetTransform() const { return m_Transform; }

	//////////////////////////////////////////////////////////////////////////
	// Getter, Setter
	//////////////////////////////////////////////////////////////////////////
	// �Ϲ�
	const UINT32& GetObjectID() const { return m_ID; }

	const std::string& GetName() const { return m_Name; }
	void SetName(const std::string& _name) { m_Name = _name; }

	const std::string& GetTag() const { return m_Tag; }
	void SetTag(const std::string& _tag) { m_Tag = _tag; }

	const UINT& GetLayer() const { return m_Layer; }
	void SetLayer(const UINT& _layer) { m_Layer = _layer; }


	// ������Ʈ ����
	template<class TComponent>
	std::shared_ptr<TComponent> AddComponent();

	template<class TComponent>
	std::shared_ptr<TComponent> GetComponent();

	inline std::shared_ptr<Transform> GetTransform() { return m_Transform; }

	// ����(Hierarchy) ����
	std::shared_ptr<GameObject> GetParent() { return m_Parent.lock(); }
	void SetParent(std::shared_ptr<GameObject> _parent);

	std::shared_ptr<GameObject> GetChild(const std::string& name);
	void SetChild(std::shared_ptr<GameObject> _child);
	std::vector<std::shared_ptr<GameObject>>& GetChildren() { return m_Children; }
	void SetChildren(std::vector<std::shared_ptr<GameObject>> children);

	// ��� ���� ������ �ڽĵ��� �˻��ؼ� �ش��ϴ� �ڽ� ���� ������Ʈ ã��
	std::shared_ptr<GameObject> FindChildInAllChildren(const std::string& name);

	// ���� �ʱ�ȭ
	void ResetParent();
};

template<class TComponent>
std::shared_ptr<TComponent> GameObject::AddComponent()
{
	// ���ø��� Ÿ���� ComponentBase�� ��ӹ��� Ÿ������ Ȯ��
	static_assert(std::is_base_of<ComponentBase, TComponent>::value, "TComponent must inherit from Component");

	// if constexpr: ������ Ÿ�ӿ� ���ǥ������ boolean(true/false)�� ����
	if constexpr (std::is_same<TComponent, Transform>::value)
	{
		// Ʈ������ ������Ʈ�� ��쿡�� m_TransForm�� �־��ش�.
		m_Transform = std::make_shared<Transform>(this->shared_from_this());
		return m_Transform;
	}

	std::shared_ptr<TComponent> _component = std::make_shared<TComponent>(this->shared_from_this());
	// ������Ʈ ���Ϳ� �������
	m_Components.emplace_back(_component);

	// �� ���� ������ ����� �� �ֵ��� weak_ptr�� ��ȯ
	// todo: ���丮 ���� �������� ����ȭ �Ƿ���?
	return _component;
}

template<class TComponent>
std::shared_ptr<TComponent> GameObject::GetComponent()
{
	// ���ø��� Ÿ���� ComponentBase�� ��ӹ��� Ÿ������ Ȯ��
	static_assert(std::is_base_of<ComponentBase, TComponent>::value, "TComponent must inherit from Component");

	// Ʈ�������� ��쿡�� ���� ��ȯ��
	if constexpr (std::is_same<TComponent, Transform>::value)
		return m_Transform;

	for (auto& component : m_Components)
	{
		std::shared_ptr<TComponent> dcComponent = std::dynamic_pointer_cast<TComponent>(component);

		if (dcComponent != nullptr)
			return dcComponent;
	}
	return nullptr;
}

