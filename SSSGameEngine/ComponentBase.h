/// <summary>
/// 컴포넌트 추상화 클래스
/// 
/// 2022 6 20 심승수
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
	// 기본 정보
	std::string m_ComponentName;

	bool m_IsEnable;

	// 컴포넌트를 소유하고 있는 Game Object
	std::weak_ptr<GameObject> m_GameObject;

public:
	//////////////////////////////////////////////////////////////////////////
	// Getter, Setter
	//////////////////////////////////////////////////////////////////////////
	inline std::shared_ptr<GameObject> GetGameObject() { return m_GameObject.lock(); }
	inline bool GetIsEnable() const { return m_IsEnable; }
	inline void SetIsEnable(bool value) { m_IsEnable = value; }

	virtual void Init() abstract;
	// 게임 오브젝트의 모든 컴포넌트에서 수행할 처리들
	virtual void FixedUpdate(float deltaTime) abstract;
	virtual void Update(float deltaTime) abstract;
	virtual void LateUpdate(float deltaTime) abstract;
	virtual void Render() abstract;
};
