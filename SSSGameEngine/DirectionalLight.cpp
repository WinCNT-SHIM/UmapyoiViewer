#include "GameEnginePCH.h"
#include "DirectionalLight.h"

#include "RenderingManager.h"

#include "Transform.h"

DirectionalLight::DirectionalLight(std::weak_ptr<GameObject> _GameObj)
	: LightBase(_GameObj, "DirectionalLight")
	, m_DirectionalLightInfo{ std::make_shared<DirectionalLightInfo>() }
	, m_Transform{ GetGameObject()->GetComponent<Transform>() }
{
	m_LightInfo = m_DirectionalLightInfo;
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::Release()
{
	m_Transform.reset();
	m_DirectionalLightInfo.reset();
	__super::Release();
}

void DirectionalLight::Render()
{
	m_DirectionalLightInfo->m_Direction = m_Transform->GetLook();
	RenderingManager::GetInstance()->PostDirectionalLightInfo(m_DirectionalLightInfo);
}
