#include "GameEnginePCH.h"
#include "PrefabASE.h"

#include "FactoryGameObject.h"

#include "Transform.h"
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "Armature.h"
#include "Animator.h"

NodeObjectBase::~NodeObjectBase()
{
	delete _tmAnimation;

	for (int i = 0; i < _childNodes.size(); i++)
		_childNodes[i].reset();
}

std::shared_ptr<GameObject> NodeObjectBase::MakeGameObject(PrefabASE* prefab, std::map<std::shared_ptr<NodeObjectBase>, std::shared_ptr<GameObject>>& refGameObjectByNodeMap)
{
	std::unique_ptr<FactoryGameObject> m_FactoryGameObject = std::make_unique<FactoryGameObject>();
	std::shared_ptr<GameObject> gameObject = m_FactoryGameObject->CreateGameObject(_nodeName);


	// 모든 게임 오브젝트는 일단 트랜스폼을 가진다
	gameObject->AddComponent<Transform>();
	//gameObject->SetName(_nodeName);
	gameObject->GetComponent<Transform>()->SetLocalTM(XMLoadFloat4x4(&_localTM));

	// 프리팹의 정보를 바탕으로 게임 오브젝트를 설정한다
	SettingGameObject(prefab, gameObject);

	// 자식 오브젝트도 마찬가지
	for (auto child : _childNodes)
	{
		std::shared_ptr<GameObject> childGameObj = child->MakeGameObject(prefab, refGameObjectByNodeMap);

		gameObject->SetChild(childGameObj);
	}
	refGameObjectByNodeMap.emplace(this->shared_from_this(), gameObject);

	return gameObject;
}

NodeGeoObject::~NodeGeoObject()
{

}

void NodeGeoObject::SettingGameObject(PrefabASE* prefab, std::shared_ptr<GameObject> gameObject)
{
	// Static Mesh Object
	if (_boneNameList.empty())
	{
		const auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

		std::shared_ptr<::RenderingData> renderingData = meshRenderer->m_RenderingData;

		renderingData->m_DataType = RenderingdataType::STATIC_MESH_OBJECT;

		// 메시ID를 설정
		renderingData->m_ObjectInfo->m_MeshID = _meshID;

		// Material 설정
		if (_materialRef < 0 ||
			_materialRef >= prefab->_materialList.size())
		{
			// Material Ref가 유효하지 않은 경우. 그냥 놔두면 된다.
			renderingData->m_ShaderInfo->m_TechName = TEXT("Basic");
		}
		else
		{
			// Material Ref가 유효하면 설정하면 됨
			std::shared_ptr<NodeMaterial> mat = prefab->_materialList[_materialRef];

			renderingData->m_MaterialInfo->m_Ambient = mat->_ambient;
			renderingData->m_MaterialInfo->m_Diffuse = mat->_diffuse;
			renderingData->m_MaterialInfo->m_Specular = mat->_specular;
			renderingData->m_MaterialInfo->m_Reflect = mat->_reflect;

			renderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID = mat->_diffuseMapID;
			renderingData->m_MaterialInfo->m_TextureInfo.m_NormalMapID = mat->_normalMapID;
			
			// 머테리얼 설정은 있지만 텍스처는 없는 경우
			if (renderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID == UINT_MAX)
			{
			}
		}

		//if ((renderingData->m_MaterialInfo->m_DiffuseMapID != UINT_MAX)
		//	&& (renderingData->m_MaterialInfo->m_NormalMapID != UINT_MAX))
		//{
		//	//renderingData->m_ShaderInfo->m_VSName = TEXT("VS_DiffuseNormal");
		//	//renderingData->m_ShaderInfo->m_PSName = TEXT("PS_DiffuseNormal");
		//	renderingData->m_ShaderInfo->m_TechName = TEXT("Basic");	// 일단 기본 테크
		//}
		//else if (renderingData->m_MaterialInfo->m_DiffuseMapID != UINT_MAX)
		//{
		//	//renderingData->m_ShaderInfo->m_VSName = TEXT("VS_Diffuse");
		//	//renderingData->m_ShaderInfo->m_PSName = TEXT("PS_Diffuse");
		//	renderingData->m_ShaderInfo->m_TechName = TEXT("Basic");	// 일단 기본 테크
		//}
	}
	// Skinned Mesh Object
	else
	{
		const auto meshRenderer = gameObject->AddComponent<SkinnedMeshRenderer>();

		std::shared_ptr<::RenderingData> renderingData = meshRenderer->m_RenderingData;

		renderingData->m_DataType = RenderingdataType::SKINNED_MESH_OBJECT;

		renderingData->m_ObjectInfo->m_MeshID = _meshID;

		if (_materialRef < 0 ||
			_materialRef >= prefab->_materialList.size())
		{
			// Material Ref가 유효하지 않은 경우. 그냥 놔두면 된다.
			renderingData->m_ShaderInfo->m_TechName = TEXT("BasicSkinned");
		}
		else
		{
			std::shared_ptr<NodeMaterial> mat = prefab->_materialList[_materialRef];

			renderingData->m_MaterialInfo->m_Ambient = mat->_ambient;
			renderingData->m_MaterialInfo->m_Diffuse = mat->_diffuse;
			renderingData->m_MaterialInfo->m_Specular = mat->_specular;
			renderingData->m_MaterialInfo->m_Reflect = mat->_reflect;

			renderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID = mat->_diffuseMapID;
			renderingData->m_MaterialInfo->m_TextureInfo.m_NormalMapID = mat->_normalMapID;
			renderingData->m_ShaderInfo->m_TechName = TEXT("BasicSkinned");
		}

		if ((renderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID != UINT_MAX)
			&& (renderingData->m_MaterialInfo->m_TextureInfo.m_NormalMapID != UINT_MAX))
		{
			//renderingData->m_ShaderInfo->m_VSName = TEXT("VS_DiffuseNormalSkinning");
			//renderingData->m_ShaderInfo->m_PSName = TEXT("PS_DiffuseNormalSkinning");
			renderingData->m_ShaderInfo->m_TechName = TEXT("BasicSkinned");
		}
		else if (renderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID != UINT_MAX)
		{
			//renderingData->m_ShaderInfo->m_VSName = TEXT("VS_DiffuseSkinning");
			//renderingData->m_ShaderInfo->m_PSName = TEXT("PS_DiffuseSkinning");
			renderingData->m_ShaderInfo->m_TechName = TEXT("BasicSkinned");
		}
	}
}

PrefabASE::~PrefabASE()
{
	for (auto& _rootNode : _rootNodes)
		_rootNode.reset();
	_rootNodes.clear();

	for (auto& iter : _nodeMap)
		iter.second.reset();
	_nodeMap.clear();

	for (auto& material : _materialList)
		material.reset();
	_materialList.clear();
}

std::shared_ptr<GameObject> PrefabASE::MakeGameObject()
{
	std::unique_ptr<FactoryGameObject> m_FactoryGameObject = std::make_unique<FactoryGameObject>();

	// Final Root. 실제로 트랜스폼 등을 붙이거나 하는 게임 오브젝트
	//std::shared_ptr<GameObject> outObject = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> outObject = m_FactoryGameObject->CreateGameObject(_prefabName);

	outObject->AddComponent<Transform>();

	// 프리팹의 노드 정보를 이용해서 하위 게임 오브젝트들을 만들 것이다
	std::map<std::string, std::shared_ptr<GameObject>> gameObjectByNodeNameMap;
	std::map<std::shared_ptr<NodeObjectBase>, std::shared_ptr<GameObject>> gameObjectByNodeMap;

	// 루트 노드들에 대해서 애니메이션 정보를 저장한다.
	for (int i = 0; i < _rootNodes.size(); i++)
	{
		std::shared_ptr<GameObject> rootObject = _rootNodes[i]->MakeGameObject(this, gameObjectByNodeMap);
		outObject->SetChild(rootObject);

		std::vector<std::string> hierarchy{ _rootNodes[i]->_nodeName };

		InitAnimTrackList(_rootNodes[i], hierarchy);
	}

	// 본 생성 시 검색을 위해 맵을 만들자
	for (const auto& pair : gameObjectByNodeMap)
	{
		gameObjectByNodeNameMap.emplace(pair.first->_nodeName, pair.second);
	}

	// 본 스키닝 애니메이션 준비
	for (auto& node : gameObjectByNodeMap)
	{
		const auto nodeGeo = std::dynamic_pointer_cast<NodeGeoObject>(node.first);

		if (nodeGeo == nullptr)
			continue;

		if (nodeGeo->_boneNameList.empty())
			continue;

		// 여기까지 통과했다면 이 노드로부터 형성된 게임 오브젝트는 스킨 메쉬이다.
		std::vector<std::shared_ptr<::Bone>> boneList(nodeGeo->_boneNameList.size());

		for (int i = 0; i < nodeGeo->_boneNameList.size(); i++)
		{
			std::string boneName = nodeGeo->_boneNameList[i];

			const auto goIter = gameObjectByNodeNameMap.find(boneName);

			if (goIter == gameObjectByNodeNameMap.end())
				continue;

			const auto iter = _nodeMap.find(boneName);

			if (iter == _nodeMap.end())
				continue;

			std::shared_ptr<::Bone> newBone = std::make_shared<::Bone>();

			newBone->m_BoneNodeTM = iter->second->_nodeTM;
			newBone->m_Name = boneName;
			newBone->m_BoneObject = goIter->second;
			newBone->m_Index = i;

			boneList[i] = newBone;
		}

		if (boneList.empty())
			continue;

		const auto armature = node.second->AddComponent<Armature>();

		armature->Init(nodeGeo->_nodeTM, boneList);
	}

	// 최종 상단 오브젝트에게 Animator Component를 붙여준다.
	const auto animator = outObject->AddComponent<Animator>();

	std::shared_ptr<AnimationClip> animClip = std::make_shared<AnimationClip>(_animPosTrackList, _animRotTrackList);

	animator->SetAnimClip(animClip);

	return outObject;
}

void PrefabASE::InitAnimTrackList(std::shared_ptr<NodeObjectBase> currentNode, std::vector<std::string> currentNodeHierarchy)
{
	//if (currentNode->_tmAnimation == nullptr)
	//	return;

	// 포지션 애니메이션 초기화
	if (currentNode->_tmAnimation != nullptr
		&& currentNode->_tmAnimation->m_position.empty() == false)
	{
		// 슬슬 파싱한 값을 놓아줄 때가 됐다...값 복사 시작!!
		std::vector<std::pair<unsigned, Vector3>> posTrack;
		for (auto& pos : currentNode->_tmAnimation->m_position)
			posTrack.push_back({ pos->m_time, pos->m_pos });

		_animPosTrackList.push_back({ currentNodeHierarchy, posTrack });
	}

	// 회전 애니메이션 초기화
	if (currentNode->_tmAnimation != nullptr
		&& currentNode->_tmAnimation->m_rotation.empty() == false)
	{
		// 슬슬 파싱한 값을 놓아줄 때가 됐다...값 복사 시작!!
		std::vector<std::pair<unsigned, Quaternion>> rotTrack;
		for (auto& rot : currentNode->_tmAnimation->m_rotation)
			rotTrack.push_back({ rot->m_time, rot->m_rotQT_accumulation });

		_animRotTrackList.push_back({ currentNodeHierarchy, rotTrack });
	}

	// 크기 애니메이션은 없어서 생략함
	//if (currentNode->_tmAnimation->m_scale.empty() == false)
	//{
	//}

	// 본인까지 하이어라키를 꽂아서 애니메이션 트랙 리스트를 형성한다.
	for (const auto& child : currentNode->_childNodes)
	{
		currentNodeHierarchy.push_back(child->_nodeName);
		InitAnimTrackList(child, currentNodeHierarchy);
		currentNodeHierarchy.pop_back();
	}

	//delete currentNode->_tmAnimation;
}
