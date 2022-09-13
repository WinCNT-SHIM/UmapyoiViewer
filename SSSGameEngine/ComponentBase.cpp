#include "GameEnginePCH.h"
#include "ComponentBase.h"

ComponentBase::ComponentBase(std::weak_ptr<GameObject> _GameObj, std::string ComponentName)
	: m_GameObject(_GameObj)
	, m_ComponentName(ComponentName)
	, m_IsEnable(true)
{
}

ComponentBase::~ComponentBase()
{
}

void ComponentBase::Release()
{
	if (!m_GameObject.expired())
	{
		m_GameObject.lock().reset();
	}
}
