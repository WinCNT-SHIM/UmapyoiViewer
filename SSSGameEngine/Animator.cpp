#include "GameEnginePCH.h"
#include "Animator.h"

#include "Transform.h"
#include "AnimationClip.h"

Animator::Animator(std::weak_ptr<GameObject> gameObj)
	: ComponentBase(gameObj, "Animator")
	, _animationClip(nullptr)
	, playingAnimTick(0)
	, _fps(4800)
{

}

Animator::~Animator()
{
}

void Animator::Release()
{
	_animationClip.reset();
	__super::Release();
}

void Animator::Update(float deltaTime)
{
	if (_animationClip == nullptr)
		return;

	if (_animationClip->GetStartAnimDelayTime() > m_AddedDeltaTimeAnimStart)
	{
		m_AddedDeltaTimeAnimStart += deltaTime;
		return;
	}

	playingAnimTick = playingAnimTick + _fps * deltaTime;

	// ���� ��� �������� �ƽ� �������� �Ѿ����� üũ
	//if (playingAnimTick >= _fps * 5)
	if (playingAnimTick >= _animationClip->GetMaxFrame())
		m_IsAnimationOver = true;
	else
		m_IsAnimationOver = false;

	// �ƽ� ������ �Ѿ�� �ٽ� ó������ ����ϰ� ������
	playingAnimTick = static_cast<float>(std::fmod(playingAnimTick, _animationClip->GetMaxFrame()));

	const auto& animPosTrackList = _animationClip->GetPositionTrackList();
	for (const auto& track : animPosTrackList)
	{
		std::shared_ptr<GameObject> targetObject = GetGameObject();

		// �� �ִϸ��̼� Ʈ���� � �༮�� Ÿ�������� �ϴ� Ʈ������ �˰� �����ϱ� ã�´�.
		for (const auto& name : track._targetHierarchy)
			targetObject = targetObject->GetChild(name);

		Vector3 position;

		_animationClip->GetInterpolationByTargetFrame(track._posTrack, static_cast<int>(playingAnimTick), position);

		if (position != Vector3::Zero)
			targetObject->GetComponent<Transform>()->SetPosition(position, Space::LOCAL);
	}

	const auto& animRotTrackList = _animationClip->GetRotationTrackList();
	for (const auto& track : animRotTrackList)
	{
		std::shared_ptr<GameObject> targetObject = GetGameObject();

		for (const auto& name : track._targetHierarchy)
			targetObject = targetObject->GetChild(name);

		Quaternion rotation;

		_animationClip->GetInterpolationByTargetFrame(track._rotTrack, static_cast<int>(playingAnimTick), rotation);

		if (rotation != Quaternion::Identity)
			targetObject->GetComponent<Transform>()->SetRotation(Transform::QuaternionToEuler(rotation), Space::LOCAL);
			//targetObject->GetComponent<Transform>()->SetRotationLocalQuat(rotation); // �̰͵� �ȴ�
	}
}
