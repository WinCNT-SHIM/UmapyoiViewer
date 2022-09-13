/// <summary>
/// ���� �߻�ȭ Ŭ����
/// 
/// 2022. 6. 17 �ɽ¼�
/// </summary>
#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

#ifdef _SSSEngine_EXPORTS
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#endif

class FactoryGameObject;
class GameObject;
class Camera;

class RendererBase;

class DLL_DECLSPEC SceneBase
{
//////////////////////////////////////////////////////////////////////////
// ������ �� �Ҹ���
//////////////////////////////////////////////////////////////////////////
public:
	SceneBase(std::string sceneName);
	virtual ~SceneBase();

//////////////////////////////////////////////////////////////////////////
// ����
//////////////////////////////////////////////////////////////////////////
protected:
	std::string m_Name;
	std::vector<std::shared_ptr<GameObject>> m_GameObjectsInScene;
	static std::unique_ptr<FactoryGameObject> m_FactoryGameObject;

	// ������ ����
	std::vector<std::shared_ptr<RendererBase>> m_RendererInScene;

	// ���� ī�޶�
	std::shared_ptr<Camera> m_MainCamera;

	// todo: Light ������Ʈ�� �߰��� ����

//////////////////////////////////////////////////////////////////////////
// �� ������ �ʿ��� �Լ���
//////////////////////////////////////////////////////////////////////////
public:
	virtual void BuildScene() abstract;
	void ClearScene();

public:
	std::shared_ptr<GameObject> CreateGameObject();
	std::shared_ptr<GameObject> CreateGameObject(std::string gameObjName);
	std::shared_ptr<GameObject> CreateGameObjectFromPrefab(std::string prefabName);
	
	std::shared_ptr<GameObject> CreateUmaMusume(
		std::string charID
		, std::string charSubID
		, std::string liveID
		, std::string liveSubID
	);

	void ReleaseGameObject(std::shared_ptr<GameObject> gameObject);

	//const vector<GameObject*>& GetGameObjects() {};
	//const std::vector<Camera*>& GetCameras();
	//void SceneChange(std::string sceneName);

	//////////////////////////////////////////////////////////////////////////
	// Getter, Setter
	//////////////////////////////////////////////////////////////////////////
	// ���� ī�޶�
	inline std::shared_ptr<Camera> GetMainCamera() { return m_MainCamera; }
	inline void SetMainCamera(std::shared_ptr<Camera> camera) { m_MainCamera = camera; };

	// ������ ���� ����
	void AddRenderer(std::shared_ptr<RendererBase> renderer);

private:
	void AddGameObject(std::shared_ptr<GameObject> gameObject);

private:
	void AddGameObjectToScene(std::shared_ptr<GameObject> gameObject);

public:
	virtual void Init();
	virtual void FixedUpdate(float deltaTime);
	virtual void Update(float deltaTime);
	virtual void LateUpdate(float deltaTime);
	virtual void Render();
	virtual void Release();
};
