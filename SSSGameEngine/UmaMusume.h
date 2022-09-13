/// <summary>
/// �츶������ ĳ���Ϳ� ���� ������Ʈ
/// ���� + �Ӹ� + �ִϸ��̼ǿ� ���� �ʱ�ȭ�� ó����
/// 
/// Created : 2022/08/17 �ɽ¼�
/// Updated : 2022/08/17 �ɽ¼�
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
	// ��ü �𵨸��� ���� �ֻ��� ���� ������Ʈ
	std::shared_ptr<GameObject> m_UmaMusumeBody;	// ������ �ֻ��� GO
	std::shared_ptr<GameObject> m_UmaMusumeHead;	// �Ӹ��� �ֻ��� GO
	std::shared_ptr<GameObject> m_UmaMusumeEyes;	// ������(...)�� �ֻ��� GO

	// Ư�� ���� ���� ���� ������Ʈ(����� �Ӹ��� ���� ������Ʈ) ĳ��
	std::shared_ptr<GameObject> m_BodyNeckGameObj;	// ������ Neck GO
	std::shared_ptr<GameObject> m_BodyHeadGameObj;	// ������ Head GO
	std::shared_ptr<GameObject> m_HeadNeckGameObj;	// �Ӹ��� Neck GO
	std::shared_ptr<GameObject> m_HeadHeadGameObj;	// �Ӹ��� Head GO
	std::shared_ptr<GameObject> m_EyesNeckGameObj;	// �Ӹ��� Neck GO
	std::shared_ptr<GameObject> m_EyesHeadGameObj;	// �Ӹ��� Head GO

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
	std::shared_ptr<GameObject> CreateGameObjectFromPrefab(std::string prefabName); // SceneBase�� ���� ���� ó�� + �츶������ ���̴� ����
	void SetUmaMusumeShader(std::shared_ptr<GameObject> gameObject);
	void SynchronizeBodyHead();

public:
	virtual void Init();
	void FixedUpdate(float deltaTime) override {}
	void Update(float deltaTime) override {}
	void LateUpdate(float deltaTime) override;
	virtual void Render() {}

// ID�� ��ο� ���� ������(���Ƽ� �ؿ� ���)
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
	// Body ����
	std::string m_PathBodyDiff;
	std::string m_PathBodyBase;
	std::string m_PathBodyCtrl;
	std::string m_PathBodyShad_c;
	// Hair ����
	std::string m_PathHairDiff;
	std::string m_PathHairBase;
	std::string m_PathHairCtrl;
	std::string m_PathHairShad_c;
	// Face ����
	std::string m_PathFaceDiff;
	std::string m_PathFaceBase;
	std::string m_PathFaceCtrl;
	std::string m_PathFaceShad_c;
	// Eyes ����
	std::string m_PathEyes0;
	std::string m_PathEyesHighlight00;
	std::string m_PathEyesHighlight01;
	std::string m_PathEyesHighlight02;
	// Cheek ����
	std::string m_PathCheek0;
	// Sound ����
	std::string m_LiveBGMFile;
	std::string m_PathLiveBGMFile;
	std::string m_CharVocalFile;
	std::string m_PathCharVocalFile;
	std::string m_CharSubVocalFile;
	std::string m_PathCharSubVocalFile;
};

