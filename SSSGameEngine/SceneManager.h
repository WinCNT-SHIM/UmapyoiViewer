/// <summary>
/// 씬 매니저 클래스
/// 씬들을 추가하고 전환하는 역할을 한다
/// 
/// Created : 2022/06/17 심승수
/// Updated : 2022/08/04 심승수
/// </summary>
#pragma once

#include "..\CommonLib\SingletonDefine.h"

#include <map>

class SceneBase;

class SceneManager
{
	// SceneManager를 싱글톤으로 만들자
public:
	SceneManager() = default;
	SceneManager(const SceneManager& ref) = delete;
	SceneManager& operator=(const SceneManager& ref) = delete;
	~SceneManager() { /*Release();*/ }
private:
	static std::shared_ptr<SceneManager> m_Instance;
public:
	static std::shared_ptr<SceneManager> GetInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = std::make_shared<SceneManager>();
			m_Instance->Init();
		}
		return m_Instance;
	}

public:
	std::map<std::string, SceneBase*> SceneList() const { return m_SceneList; }
	void SceneList(std::map<std::string, SceneBase*> val) { m_SceneList = val; }
private:
	std::map<std::string, SceneBase*> m_SceneList;
	SceneBase* m_CurrentScene = nullptr;

public:
	// 씬 추가 및 교체
	void AddScene(std::string sceneName, SceneBase* scene);
	void ChangeScene(std::string sceneName);
	SceneBase* GetCurrentScene() const { return m_CurrentScene; }

private:
	// 씬 교체 시 이전 씬 제거
	void DeleteScene();

public:
	void Init();
	void Update(float deltaTime);
	void Render();
	void Release();
};