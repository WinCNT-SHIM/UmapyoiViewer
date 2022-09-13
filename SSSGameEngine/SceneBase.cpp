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
	// Scene�� new�� ��� �ٷ� Scene Manager�� �߰���
	m_Name = sceneName;
	SceneManager::GetInstance()->AddScene(sceneName, this);
}

SceneBase::~SceneBase()
{

}

void SceneBase::ClearScene()
{
	// ���� ���� ������Ʈ���� �����Ѵ�
	for (auto& gameObject : m_GameObjectsInScene)
	{
		gameObject->Release();
	}
	// Ȥ�� �𸣴� ������ ���͵� clear
	// (������ ��ü�� ������ ���� ������Ʈ ���� �� �Ͼ)
	m_RendererInScene.clear();
	// ���ҽ��鵵 ������
	ResourceManager::GetInstance()->Release();
}

std::unique_ptr<FactoryGameObject> SceneBase::m_FactoryGameObject = std::make_unique<FactoryGameObject>();
std::shared_ptr<GameObject> SceneBase::CreateGameObject()
{
	std::shared_ptr<GameObject> newObject = m_FactoryGameObject->CreateGameObject();

	// � ���� ������Ʈ�� Transform�� �ְ� �ϱ� ���� ������ �߰��Ѵ�
	newObject->AddComponent<Transform>();

	AddGameObjectToScene(newObject);

	return newObject;
}

std::shared_ptr<GameObject> SceneBase::CreateGameObject(std::string gameObjName)
{
	std::shared_ptr<GameObject> newObject = m_FactoryGameObject->CreateGameObject(gameObjName);

	// � ���� ������Ʈ�� Transform�� �ְ� �ϱ� ���� ������ �߰��Ѵ�
	newObject->AddComponent<Transform>();

	AddGameObjectToScene(newObject);

	return newObject;
}

std::shared_ptr<GameObject> SceneBase::CreateGameObjectFromPrefab(std::string prefabName)
{
	std::shared_ptr<PrefabASE> prefabASE = ResourceManager::GetInstance()->GetPrefab(prefabName);

	assert(prefabASE);

	// �������� ���ؼ� ���� ������Ʈ�� ������
	std::shared_ptr<GameObject> newObject = prefabASE->MakeGameObject();

	// ���� ���� ������Ʈ�� ���� ����
	AddGameObject(newObject);

	// �ֻ��� ��Ʈ�� ���� ������Ʈ�� ��ȯ�Ѵ�
	return newObject;
}

std::shared_ptr<GameObject> SceneBase::CreateUmaMusume(std::string charID, std::string charSubID, std::string liveID, std::string liveSubID)
{
	// ��ȯ�� �ֻ��� ���� ������Ʈ
	std::shared_ptr<GameObject> newUmaMusume = m_FactoryGameObject->CreateGameObject(charID);

	// � ���� ������Ʈ�� Transform�� �ְ� �ϱ� ���� ������ �߰��Ѵ�
	newUmaMusume->AddComponent<Transform>();

	// �츶�����޿� ���� ������Ʈ ����
	const auto const comp = newUmaMusume->AddComponent<UmaMusume>();
	comp->CreateUmaMusume(charID, charSubID, liveID, liveSubID);

	// ���� ���� ������Ʈ�� ���� ����
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
	// todo: ������Ʈ �Ŵ��� �����(?)
	// Scene�� �ִ� ������Ʈ���� ������Ʈ�Ѵ�
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
		// ���� ������Ʈ�� ���� ��� ������Ʈ�� ������
		//for (const auto iterComp : GameObj->m_Components)
		//	iterComp->Render();
		// todo: ������Ʈ�� ������ ������Ʈ�� Render�ϵ��� ����
		GameObj->Render();
	}
}

void SceneBase::Release()
{

}
