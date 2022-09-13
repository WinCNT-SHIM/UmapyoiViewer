#pragma once

#include "../ParserLib/CASEParser.h"
#include "AnimationClip.h"

using namespace DirectX;

class GameObject;
struct PrefabASE;

struct NodeMaterial
{
	XMFLOAT4 _ambient;
	XMFLOAT4 _diffuse;
	XMFLOAT4 _specular;
	XMFLOAT4 _reflect;

	uint32 _diffuseMapID;
	uint32 _normalMapID;
};

struct NodeObjectBase : public std::enable_shared_from_this<NodeObjectBase>
{
	virtual ~NodeObjectBase();

	std::shared_ptr<GameObject> MakeGameObject(
		PrefabASE* prefab
		, std::map<std::shared_ptr<NodeObjectBase>
		, std::shared_ptr<GameObject>>& refGameObjectByNodeMap
	);

	std::string _nodeName = "";
	std::string _nodeParent = "";
	XMFLOAT4X4 _nodeTM;
	XMFLOAT4X4 _localTM;
	Animation* _tmAnimation = nullptr;
	//std::shared_ptr<Animation> _tmAnimation;

	std::weak_ptr<NodeObjectBase> _parentNode;
	std::vector<std::shared_ptr<NodeObjectBase>> _childNodes;

private:
	virtual void SettingGameObject(PrefabASE* prefab, std::shared_ptr<GameObject> gameObject) {}
};

struct NodeGeoObject : NodeObjectBase
{
	~NodeGeoObject() override;

	uint32 _meshID;

	uint32 _materialRef;
	std::vector<std::string> _boneNameList;
	//std::vector<VertexWeight> _wVertexs;

private:
	virtual void SettingGameObject(PrefabASE* prefab, std::shared_ptr<GameObject> gameObject) override;
};

struct NodeHelperObject : NodeObjectBase
{
};

struct NodeShapeObject : NodeObjectBase
{
};

struct PrefabASE
{
	PrefabASE(
		std::vector<std::shared_ptr<NodeObjectBase>> rootNodes,
		std::map<std::string, std::shared_ptr<NodeObjectBase>> nodeMap,
		std::vector<std::shared_ptr<NodeMaterial>> materialList
	)
	{
		_rootNodes = std::move(rootNodes);
		_nodeMap = std::move(nodeMap);
		_materialList = std::move(materialList);
	}

	~PrefabASE();

	std::shared_ptr<GameObject> MakeGameObject();

	// ���� �θ� �����ϴ� �༮��
	// �� ģ������ �ϳ��� ��ƼƼ�� ��� ������� ���� ������Ʈ�� �����Ѵ�.
	std::vector<std::shared_ptr<NodeObjectBase>> _rootNodes;
	std::map<std::string, std::shared_ptr<NodeObjectBase>> _nodeMap;
	std::vector<std::shared_ptr<NodeMaterial>> _materialList;

	// �� �ִϸ��̼����� �� ���� ���� �������� �Űܵ� �Ǵ� ����
	// ������ ���� ������Ʈ�� �̵�, �ǰ� ���� ���� �θ� ������Ʈ�� Ʈ�������� ������ �����̴�.
	void InitAnimTrackList(std::shared_ptr<NodeObjectBase> currentNode, std::vector<std::string> currentNodeHierarchy);

	std::vector<AnimPositionTrack> _animPosTrackList;
	std::vector<AnimRotationTrack> _animRotTrackList;

	// ��Ÿ ����
	std::string _prefabName;
};
