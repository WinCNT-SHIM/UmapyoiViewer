/// <summary>
/// ������Ʈ �߻�ȭ Ŭ����
/// 
/// 2022 6 20 �ɽ¼�
/// </summary>
#pragma once

#ifdef _SSSEngine_EXPORTS
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#endif

class GameObject;

class DLL_DECLSPEC ComponentBase abstract
{
public:
	ComponentBase(std::weak_ptr<GameObject> _GameObj, std::string ComponentName);
	virtual ~ComponentBase();
	virtual void Release();

protected:
	// �⺻ ����
	std::string m_ComponentName;

	bool m_IsEnable;

	// ������Ʈ�� �����ϰ� �ִ� Game Object
	std::weak_ptr<GameObject> m_GameObject;

public:
	//////////////////////////////////////////////////////////////////////////
	// Getter, Setter
	//////////////////////////////////////////////////////////////////////////
	inline std::shared_ptr<GameObject> GetGameObject() { return m_GameObject.lock(); }
	inline bool GetIsEnable() const { return m_IsEnable; }
	inline void SetIsEnable(bool value) { m_IsEnable = value; }

	virtual void Init() abstract;
	// ���� ������Ʈ�� ��� ������Ʈ���� ������ ó����
	virtual void FixedUpdate(float deltaTime) abstract;
	virtual void Update(float deltaTime) abstract;
	virtual void LateUpdate(float deltaTime) abstract;
	virtual void Render() abstract;
};
