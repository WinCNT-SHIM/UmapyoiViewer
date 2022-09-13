#pragma once

#include "ComponentBase.h"

class AnimationClip;

class Animator
	: public ComponentBase
{
public:
	Animator(std::weak_ptr<GameObject> gameObj);
	virtual ~Animator();
	virtual void Release() override;

private:
	std::shared_ptr<AnimationClip> _animationClip;
	float playingAnimTick;
	int _fps;
	
	// 애니메이션 시작 지연 체크
	float m_AddedDeltaTimeAnimStart = 0.0f;

	// 애니메이션 재생 끝남 체크
	bool m_IsAnimationOver = false;

public:
	float GetPlayingFrame() const { return playingAnimTick; }
	int GetFPS() const { return _fps; }
	bool IsAnimationOver() { return m_IsAnimationOver; }

	std::shared_ptr<AnimationClip> GetAnimClip() const { return _animationClip; }
	void SetAnimClip(std::shared_ptr<AnimationClip> animClip) { _animationClip = animClip; playingAnimTick = 0; }

public:
	virtual void Init() override {}
	void FixedUpdate(float deltaTime) override {}
	virtual void Update(float deltaTime) override;
	void LateUpdate(float deltaTime) override {}
	virtual void Render() override {}
};

