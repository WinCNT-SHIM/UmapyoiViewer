/// <summary>
/// �� �Ŵ��� Ŭ����
/// ������ �߰��ϰ� ��ȯ�ϴ� ������ �Ѵ�
/// 
/// Created : 2022/06/17 �ɽ¼�
/// Updated : 2022/08/04 �ɽ¼�
/// </summary>
#pragma once

#include "..\CommonLib\SingletonDefine.h"

#include <map>

class SceneBase;

class SceneManager
{
	// SceneManager�� �̱������� ������
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
	// �� �߰� �� ��ü
	void AddScene(std::string sceneName, SceneBase* scene);
	void ChangeScene(std::string sceneName);
	SceneBase* GetCurrentScene() const { return m_CurrentScene; }

private:
	// �� ��ü �� ���� �� ����
	void DeleteScene();

public:
	void Init();
	void Update(float deltaTime);
	void Render();
	void Release();
};