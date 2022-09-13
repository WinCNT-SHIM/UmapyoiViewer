/// <summary>
/// 우마무스메 캐릭터에 대한 컴포넌트
/// 몸통 + 머리 + 애니메이션에 대한 초기화도 처리함
/// 
/// Created : 2022/08/17 심승수
/// Updated : 2022/08/17 심승수
/// </summary>
#pragma once

class AnimationClip;
class Animator;

class DLL_DECLSPEC UmaMusume
	: public ComponentBase
{
public:
	UmaMusume(std::weak_ptr<GameObject> _GameObj);
	virtual ~UmaMusume();
	virtual void Release() override;

private:
	// 전체 모델링에 대한 최상위 게임 오브젝트
	std::shared_ptr<GameObject> m_UmaMusumeBody;	// 몸통의 최상위 GO
	std::shared_ptr<GameObject> m_UmaMusumeHead;	// 머리의 최상위 GO
	std::shared_ptr<GameObject> m_UmaMusumeEyes;	// 눈동자(...)의 최상위 GO

	// 특정 본에 대한 게임 오브젝트(몸통과 머리의 하위 오브젝트) 캐싱
	std::shared_ptr<GameObject> m_BodyNeckGameObj;	// 몸통의 Neck GO
	std::shared_ptr<GameObject> m_BodyHeadGameObj;	// 몸통의 Head GO
	std::shared_ptr<GameObject> m_HeadNeckGameObj;	// 머리의 Neck GO
	std::shared_ptr<GameObject> m_HeadHeadGameObj;	// 머리의 Head GO
	std::shared_ptr<GameObject> m_EyesNeckGameObj;	// 머리의 Neck GO
	std::shared_ptr<GameObject> m_EyesHeadGameObj;	// 머리의 Head GO

public:
	void CreateUmaMusume(std::string charID, std::string charSubID, std::string liveID, std::string liveSubID);

	std::shared_ptr<GameObject> GetUmaMusumeBody() { return m_UmaMusumeBody != nullptr ? m_UmaMusumeBody : nullptr; }

private:
	void SetResourcePath(std::string charID, std::string charSubID, std::string liveID, std::string liveSubID);

	void SetUmaMusumeResource();

	void AssemblyUmaMusume(
		std::shared_ptr<GameObject> umaMusumeBody
		, std::shared_ptr<GameObject> umaMusumeHead
		, std::shared_ptr<GameObject> umaMusumeEyes
		, std::shared_ptr<GameObject> umaMusumeLiveAnim
	);

	void SetTexture();
	void SetAnimClip(std::shared_ptr<GameObject> umaMusumeLiveAnim);
	void ConvertAnimClipName(std::shared_ptr<AnimationClip> clip);
	void SetStartAnimDelayTime(std::shared_ptr<AnimationClip> animator);

private:
	std::shared_ptr<GameObject> CreateGameObjectFromPrefab(std::string prefabName); // SceneBase와 거의 같은 처리 + 우마무스메 쉐이더 설정
	void SetUmaMusumeShader(std::shared_ptr<GameObject> gameObject);
	void SynchronizeBodyHead();

public:
	virtual void Init();
	void FixedUpdate(float deltaTime) override {}
	void Update(float deltaTime) override {}
	void LateUpdate(float deltaTime) override;
	virtual void Render() {}

// ID와 경로에 대한 변수들(많아서 밑에 기술)
private:
	// ID
	std::string m_CharID;
	std::string m_CharSubID;
	std::string m_LiveID;
	std::string m_LiveSubID;
	// ASE
	std::string m_PathUmaMusumeBody;
	std::string m_PathUmaMusumeHead;
	std::string m_PathUmaMusumeEyes;
	std::string m_PathLiveAnimation;
	// Body 관련
	std::string m_PathBodyDiff;
	std::string m_PathBodyBase;
	std::string m_PathBodyCtrl;
	std::string m_PathBodyShad_c;
	// Hair 관련
	std::string m_PathHairDiff;
	std::string m_PathHairBase;
	std::string m_PathHairCtrl;
	std::string m_PathHairShad_c;
	// Face 관련
	std::string m_PathFaceDiff;
	std::string m_PathFaceBase;
	std::string m_PathFaceCtrl;
	std::string m_PathFaceShad_c;
	// Eyes 관련
	std::string m_PathEyes0;
	std::string m_PathEyesHighlight00;
	std::string m_PathEyesHighlight01;
	std::string m_PathEyesHighlight02;
	// Cheek 관련
	std::string m_PathCheek0;
	// Sound 관련
	std::string m_LiveBGMFile;
	std::string m_PathLiveBGMFile;
	std::string m_CharVocalFile;
	std::string m_PathCharVocalFile;
	std::string m_CharSubVocalFile;
	std::string m_PathCharSubVocalFile;
};

