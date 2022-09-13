#pragma once

class GameObject;

class FactoryGameObject
{
public:
	FactoryGameObject() {};
	virtual ~FactoryGameObject() {};

	// todo: deleter라는 것도 만들어야 하나...흑흑
	std::shared_ptr<GameObject> CreateGameObject();
	std::shared_ptr<GameObject> CreateGameObject(std::string _GameObjName);
};
