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

	// 최종 부모를 형성하는 녀석들
	// 이 친구들을 하나의 엔티티에 담는 방식으로 게임 오브젝트를 구성한다.
	std::vector<std::shared_ptr<NodeObjectBase>> _rootNodes;
	std::map<std::string, std::shared_ptr<NodeObjectBase>> _nodeMap;
	std::vector<std::shared_ptr<NodeMaterial>> _materialList;

	// 본 애니메이션으로 각 본의 로컬 포지션을 옮겨도 되는 이유
	// 어차피 게임 오브젝트의 이동, 피격 등은 최종 부모 오브젝트의 트랜스폼에 따르기 때문이다.
	void InitAnimTrackList(std::shared_ptr<NodeObjectBase> currentNode, std::vector<std::string> currentNodeHierarchy);

	std::vector<AnimPositionTrack> _animPosTrackList;
	std::vector<AnimRotationTrack> _animRotTrackList;

	// 기타 정보
	std::string _prefabName;
};
