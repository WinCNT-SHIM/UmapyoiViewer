#pragma once

using namespace DirectX::SimpleMath;

struct AnimPositionTrack
{
	std::vector<std::string> _targetHierarchy;
	std::vector<std::pair<unsigned, Vector3>> _posTrack;
};

struct AnimRotationTrack
{
	std::vector<std::string> _targetHierarchy;
	std::vector<std::pair<unsigned, Quaternion>> _rotTrack;
};

class AnimationClip
{
public:
	AnimationClip(
		const std::vector<AnimPositionTrack>& animPosTrackList
		, const std::vector<AnimRotationTrack>& animRotTrackList
		, const uint32& standardfps = 6000
	);
	~AnimationClip();

private:
	tstring _animClipName;
	uint32 _maxFrame;
	
	//// ���� ���� ������ .. �ִϸ��̼� Ŭ���� ���� �� �����ٰ� ���⿡ ���� ������ �����Ǿ�� ü����, ���� FPS ���� �ǹ̰� �ִ�.
	uint32 _standardfps;

	std::vector<AnimPositionTrack> _positionTrackList;
	std::vector<AnimRotationTrack> _rotationTrackList;

	// �ִϸ��̼� ���� ������ ����
	float m_DelayTimeAnimStartTime = 0.0f;

public:
	template <typename TVector>
	void GetInterpolationByTargetFrame(
		const std::vector<std::pair<unsigned, TVector>>& track
		, int targetFrame, TVector& outVec
	);

	auto& GetPositionTrackList() { return _positionTrackList; }
	auto& GetRotationTrackList() { return _rotationTrackList; }

	const auto& GetMaxFrame() const { return _maxFrame; }

	// �ִϸ��̼� ���� ������ ����
	const float& GetStartAnimDelayTime() const {  return m_DelayTimeAnimStartTime; }
	void SetStartAnimDelayTime(float delayTime) { m_DelayTimeAnimStartTime = delayTime; }
};

template <typename TVector>
void AnimationClip::GetInterpolationByTargetFrame(const std::vector<std::pair<unsigned, TVector>>& track, int targetFrame, TVector& outVec)
{
	if (track.size() <= 1)
		return;

	// Ÿ�� �����Ӱ� ���ų� �ٷ� ���� Ű�������� ã�´�.
	auto vecKeyFrameEnd = std::lower_bound(track.begin(), track.end(), targetFrame,
		[](const std::pair<unsigned, TVector>& keyFrame, float frame)
		{
			return static_cast<float>(keyFrame.first) <= frame;
		}
	);

	decltype(vecKeyFrameEnd) vecKeyFrameStart;

	// ������� �� �ȵ� ..!
	if (vecKeyFrameEnd == track.begin())
	{
		vecKeyFrameStart = track.end() - 1;
	}
	// Ʈ������ ������ �����Ӱ� Max Frame ���̿� �ִ� ���
	else if (vecKeyFrameEnd == track.end())
	{
		vecKeyFrameEnd = track.begin();

		vecKeyFrameStart = track.end() - 1;
	}
	else
	{
		vecKeyFrameStart = vecKeyFrameEnd - 1;
	}

	const int keyFrameStart = vecKeyFrameStart->first;
	const int keyFrameEnd = vecKeyFrameEnd->first;

	int frameCountFromStartToEnd;

	int frameCountFromStartToTarget;

	// ���� �� ���.
	if (keyFrameEnd < keyFrameStart)
	{
		frameCountFromStartToEnd = (_maxFrame - keyFrameStart) + keyFrameEnd;

		if (targetFrame < keyFrameStart)
			frameCountFromStartToTarget = (_maxFrame - keyFrameStart) + targetFrame;
		else
			frameCountFromStartToTarget = targetFrame - keyFrameStart;
	}
	else
	{
		frameCountFromStartToEnd = keyFrameEnd - keyFrameStart;

		frameCountFromStartToTarget = targetFrame - keyFrameStart;
	}

	const float interCoefficient = static_cast<float>(frameCountFromStartToTarget) / static_cast<float>(frameCountFromStartToEnd);

	if constexpr (std::is_same<TVector, Quaternion>::value)
	{
		outVec = Quaternion::Slerp(vecKeyFrameStart->second, vecKeyFrameEnd->second, interCoefficient);

		return;
	}

	if constexpr (std::is_same<TVector, Vector3>::value)
	{
		outVec = Vector3::Lerp(vecKeyFrameStart->second, vecKeyFrameEnd->second, interCoefficient);

		return;
	}
}