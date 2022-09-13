#pragma once

class GameObject;

class FactoryGameObject
{
public:
	FactoryGameObject() {};
	virtual ~FactoryGameObject() {};

	// todo: deleter��� �͵� ������ �ϳ�...����
	std::shared_ptr<GameObject> CreateGameObject();
	std::shared_ptr<GameObject> CreateGameObject(std::string _GameObjName);
};
