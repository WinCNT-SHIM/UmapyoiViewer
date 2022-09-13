#include "GameEnginePCH.h"
#include "FactoryGameObject.h"

std::shared_ptr<GameObject> FactoryGameObject::CreateGameObject()
{
	try
	{
		// todo: ����� make_shared�� ����ϰ� �;����� �� �� �ƴ�
		std::shared_ptr<GameObject> ret(new GameObject());
		return ret;
	}
	catch (...)
	{
		OutputDebugString(_T("Error CreateGameObject\n"));
	}
}

std::shared_ptr<GameObject> FactoryGameObject::CreateGameObject(std::string _GameObjName)
{
	try
	{
		std::shared_ptr<GameObject> ret(new GameObject(_GameObjName));
		return ret;
	}
	catch (...)
	{
		OutputDebugString(_T("Error CreateGameObject\n"));
	}
}
