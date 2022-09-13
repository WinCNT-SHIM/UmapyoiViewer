#include "GameEnginePCH.h"
#include "LightBase.h"

LightBase::LightBase(std::weak_ptr<GameObject> _GameObj, std::string ComponentName)
	: ComponentBase(_GameObj, ComponentName)
{
}

LightBase::~LightBase()
{
}

void LightBase::Release()
{
	m_LightInfo.reset();
	__super::Release();
}
