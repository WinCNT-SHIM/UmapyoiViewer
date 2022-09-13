#include "GameEnginePCH.h"
#include "SceneBase.h"

#include "SceneManager.h"
#include "FactoryGameObject.h"

#include "Transform.h"
#include "ResourceManager.h"
#include "PrefabASE.h"

#include "UmaMusume.h"

SceneBase::SceneBase(std::string sceneName)
{
	// Scene을 new할 경우 바로 Scene Manager에 추가함
	m_Name = sceneName;
	SceneManager::GetInstance()->AddScene(sceneName, this);
}

SceneBase::~SceneBase()
{

}

void SceneBase::ClearScene()
{
	// 씬의 게임 오브젝트들을 해제한다
	for (auto& gameObject : m_GameObjectsInScene)
	{
		gameObject->Release();
	}
	// 혹시 모르니 렌더러 벡터도 clear
	// (렌더러 자체의 해제는 게임 오브젝트 해제 시 일어남)
	m_RendererInScene.clear();
	// 리소스들도 내린다
	ResourceManager::GetInstance()->Release();
}

std::unique_ptr<FactoryGameObject> SceneBase::m_FactoryGameObject = std::make_unique<FactoryGameObject>();
std::shared_ptr<GameObject> SceneBase::CreateGameObject()
{
	std::shared_ptr<GameObject> newObject = m_FactoryGameObject->CreateGameObject();

	// 어떤 게임 오브젝트라도 Transform이 있게 하기 위해 생성시 추가한다
	newObject->AddComponent<Transform>();

	AddGameObjectToScene(newObject);

	return newObject;
}

std::shared_ptr<GameObject> SceneBase::CreateGameObject(std::string gameObjName)
{
	std::shared_ptr<GameObject> newObject = m_FactoryGameObject->CreateGameObject(gameObjName);

	// 어떤 게임 오브젝트라도 Transform이 있게 하기 위해 생성시 추가한다
	newObject->AddComponent<Transform>();

	AddGameObjectToScene(newObject);

	return newObject;
}

std::shared_ptr<GameObject> SceneBase::CreateGameObjectFromPrefab(std::string prefabName)
{
	std::shared_ptr<PrefabASE> prefabASE = ResourceManager::GetInstance()->GetPrefab(prefabName);

	assert(prefabASE);

	// 프리팹을 통해서 게임 오브젝트를 만들자
	std::shared_ptr<GameObject> newObject = prefabASE->MakeGameObject();

	// 만든 게임 오브젝트를 씬에 넣자
	AddGameObject(newObject);

	// 최상위 루트의 게임 오브젝트를 반환한다
	return newObject;
}

std::shared_ptr<GameObject> SceneBase::CreateUmaMusume(std::string charID, std::string charSubID, std::string liveID, std::string liveSubID)
{
	// 반환할 최상위 게임 오브젝트
	std::shared_ptr<GameObject> newUmaMusume = m_FactoryGameObject->CreateGameObject(charID);

	// 어떤 게임 오브젝트라도 Transform이 있게 하기 위해 생성시 추가한다
	newUmaMusume->AddComponent<Transform>();

	// 우마무스메에 대한 컴포넌트 부착
	const auto const comp = newUmaMusume->AddComponent<UmaMusume>();
	comp->CreateUmaMusume(charID, charSubID, liveID, liveSubID);

	// 만든 게임 오브젝트를 씬에 넣자
	AddGameObjectToScene(newUmaMusume);

	return newUmaMusume;
}

void SceneBase::ReleaseGameObject(std::shared_ptr<GameObject> gameObject)
{
	gameObject->Release();
}

void SceneBase::AddRenderer(std::shared_ptr<RendererBase> renderer)
{
	m_RendererInScene.emplace_back(renderer);
}

void SceneBase::AddGameObject(std::shared_ptr<GameObject> gameObject)
{
	m_GameObjectsInScene.emplace_back(gameObject);

	const std::vector<std::shared_ptr<GameObject>>& children = gameObject->GetChildren();
	for (auto& child : children)
		this->AddGameObject(child);
}

void SceneBase::AddGameObjectToScene(std::shared_ptr<GameObject> gameObject)
{
	m_GameObjectsInScene.emplace_back(gameObject);

	const std::vector<std::shared_ptr<GameObject>>& children = gameObject->GetChildren();

	for (auto& child : children)
		this->AddGameObjectToScene(child);
}

void SceneBase::Init()
{
	for (auto& gameObject : m_GameObjectsInScene)
		gameObject->Init();
}

void SceneBase::FixedUpdate(float deltaTime)
{
	for (auto& iter : m_GameObjectsInScene)
	{
		iter->FixedUpdate(deltaTime);
	}
}

void SceneBase::Update(float deltaTime)
{
	// todo: 오브젝트 매니저 만들기(?)
	// Scene에 있는 오브젝트들을 업데이트한다
	for (auto& iter : m_GameObjectsInScene)
	{
		iter->Update(deltaTime);
	}
}

void SceneBase::LateUpdate(float deltaTime)
{
	for (auto& iter : m_GameObjectsInScene)
	{
		iter->LateUpdate(deltaTime);
	}
}

void SceneBase::Render()
{
	for (const auto& GameObj : m_GameObjectsInScene)
	{
		// 게임 오브젝트가 가진 모든 컴포넌트를 렌더링
		//for (const auto iterComp : GameObj->m_Components)
		//	iterComp->Render();
		// todo: 오브젝트가 스스로 컴포넌트를 Render하도록 하자
		GameObj->Render();
	}
}

void SceneBase::Release()
{

}
