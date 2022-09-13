#include "GameEnginePCH.h"
#include "AnimationClip.h"

AnimationClip::AnimationClip(const std::vector<AnimPositionTrack>& animPosTrackList, const std::vector<AnimRotationTrack>& animRotTrackList, const uint32& standardfps)
	: _positionTrackList(animPosTrackList)
	, _rotationTrackList(animRotTrackList)
	, _standardfps(standardfps)
{
	uint32 positionMaxFrame = 0;
	uint32 rotationMaxFrame = 0;

	for (const auto& track : _positionTrackList)
		positionMaxFrame = std::max<uint32>(positionMaxFrame, track._posTrack.back().first);

	for (const auto& track : _rotationTrackList)
		rotationMaxFrame = std::max<uint32>(rotationMaxFrame, track._rotTrack.back().first);

	_maxFrame = std::max<uint32>(positionMaxFrame, rotationMaxFrame);
}

AnimationClip::~AnimationClip()
{
}