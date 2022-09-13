/// <summary>
/// 게임 오브젝트
/// 컴포넌트를 붙이는 오브젝트
/// 
/// 2022. 6. 20 심승수
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
	// 생성자 및 소멸자
	//////////////////////////////////////////////////////////////////////////
private:
	// FactoryGameObject 외에는 new를 할 수 없도록 private로 둠
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
	// 변수
	//////////////////////////////////////////////////////////////////////////
private:
	// 전체 게임 오브젝트의 아이디
	// (일단 반환 같은 거 없이 구현! 한계까지 늘어난다!)
	static UINT32 m_LastID;

	// 오브젝트의 ID
	UINT32 m_ID;
	// 오브젝트의 이름
	std::string m_Name;
	// 태그
	std::string m_Tag;
	// 레이어
	UINT m_Layer;

	// 컴포넌트 관련
	std::shared_ptr<Transform> m_Transform;	// 트랜스폼은 기본으로 가지고 있음
	std::vector<std::shared_ptr<ComponentBase>> m_Components;

	// 계층(Hierarchy) 관련
	std::weak_ptr<GameObject> m_Parent;	// 부모
	std::vector<std::shared_ptr<GameObject>> m_Children;	// 자식

	//////////////////////////////////////////////////////////////////////////
	// 함수
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
	// 일반
	const UINT32& GetObjectID() const { return m_ID; }

	const std::string& GetName() const { return m_Name; }
	void SetName(const std::string& _name) { m_Name = _name; }

	const std::string& GetTag() const { return m_Tag; }
	void SetTag(const std::string& _tag) { m_Tag = _tag; }

	const UINT& GetLayer() const { return m_Layer; }
	void SetLayer(const UINT& _layer) { m_Layer = _layer; }


	// 컴포넌트 관련
	template<class TComponent>
	std::shared_ptr<TComponent> AddComponent();

	template<class TComponent>
	std::shared_ptr<TComponent> GetComponent();

	inline std::shared_ptr<Transform> GetTransform() { return m_Transform; }

	// 계층(Hierarchy) 관련
	std::shared_ptr<GameObject> GetParent() { return m_Parent.lock(); }
	void SetParent(std::shared_ptr<GameObject> _parent);

	std::shared_ptr<GameObject> GetChild(const std::string& name);
	void SetChild(std::shared_ptr<GameObject> _child);
	std::vector<std::shared_ptr<GameObject>>& GetChildren() { return m_Children; }
	void SetChildren(std::vector<std::shared_ptr<GameObject>> children);

	// 모든 하위 계층의 자식들을 검색해서 해당하는 자식 게임 오브젝트 찾기
	std::shared_ptr<GameObject> FindChildInAllChildren(const std::string& name);

	// 계층 초기화
	void ResetParent();
};

template<class TComponent>
std::shared_ptr<TComponent> GameObject::AddComponent()
{
	// 템플릿의 타입이 ComponentBase를 상속받은 타입인지 확인
	static_assert(std::is_base_of<ComponentBase, TComponent>::value, "TComponent must inherit from Component");

	// if constexpr: 컴파일 타임에 상수표현식의 boolean(true/false)를 평가함
	if constexpr (std::is_same<TComponent, Transform>::value)
	{
		// 트랜스폼 컴포넌트인 경우에는 m_TransForm에 넣어준다.
		m_Transform = std::make_shared<Transform>(this->shared_from_this());
		return m_Transform;
	}

	std::shared_ptr<TComponent> _component = std::make_shared<TComponent>(this->shared_from_this());
	// 컴포넌트 벡터에 집어넣자
	m_Components.emplace_back(_component);

	// 그 외의 설정에 사용할 수 있도록 weak_ptr를 반환
	// todo: 팩토리 빌드 패턴으로 간략화 되려나?
	return _component;
}

template<class TComponent>
std::shared_ptr<TComponent> GameObject::GetComponent()
{
	// 템플릿의 타입이 ComponentBase를 상속받은 타입인지 확인
	static_assert(std::is_base_of<ComponentBase, TComponent>::value, "TComponent must inherit from Component");

	// 트랜스폼인 경우에는 따로 반환함
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

