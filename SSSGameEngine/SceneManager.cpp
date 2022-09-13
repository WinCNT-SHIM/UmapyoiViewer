#include "GameEnginePCH.h"
#include "SceneManager.h"

#include "SceneBase.h"
#include "RendererBase.h"

// 전역 변수
std::shared_ptr<SceneManager> SceneManager::m_Instance = nullptr;

void SceneManager::Init()
{
	try
	{
		if (m_SceneList.empty() == true)
			throw _T("There is no scene\n");

		if (m_CurrentScene == nullptr)
			ChangeScene(m_SceneList.begin()->first);
	}
	catch (const TCHAR* str)
	{
		OutputDebugString(str);
	}
}

void SceneManager::Update(float deltaTime)
{
	if (m_CurrentScene != nullptr)
	{
		m_CurrentScene->FixedUpdate(deltaTime);
		m_CurrentScene->Update(deltaTime);
		m_CurrentScene->LateUpdate(deltaTime);
	}
}

void SceneManager::Render()
{
	if (m_CurrentScene != nullptr)
		m_CurrentScene->Render();
}

void SceneManager::Release()
{
	for (auto iter : m_SceneList)
	{
		if (iter.second != nullptr)
		{
			iter.second->Release();
			delete iter.second;
		}
	}
	m_SceneList.clear();
}

void SceneManager::AddScene(std::string sceneName, SceneBase* scene)
{
	auto iter = m_SceneList.find(sceneName);

	//if (iter == m_SceneList.end())
	//	m_SceneList[sceneName] = scene;
	//else
	//	throw _T("Add Scene Failed\n");

	m_CurrentScene = scene;
	m_SceneList[sceneName] = scene;
}

void SceneManager::ChangeScene(std::string sceneName)
{

}

void SceneManager::DeleteScene()
{

}
