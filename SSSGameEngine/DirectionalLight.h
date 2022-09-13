#pragma once

#include "LightBase.h"

class DLL_DECLSPEC DirectionalLight
	: public LightBase
{
public:
	DirectionalLight(std::weak_ptr<GameObject> _GameObj);
	virtual ~DirectionalLight();
	virtual void Release() override;

private:
	std::shared_ptr<Transform> m_Transform;
public:
	std::shared_ptr<DirectionalLightInfo> m_DirectionalLightInfo;

public:
	void Init() override {}
	void FixedUpdate(float deltaTime) override {}
	void Update(float deltaTime) override {}
	void LateUpdate(float deltaTime) override {}
	void Render() override;
};

