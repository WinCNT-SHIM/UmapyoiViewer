#include "GameEnginePCH.h"
#include "FactoryGameObject.h"

std::shared_ptr<GameObject> FactoryGameObject::CreateGameObject()
{
	try
	{
		// todo: 사실은 make_shared를 사용하고 싶었지만 잘 안 됐다
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
