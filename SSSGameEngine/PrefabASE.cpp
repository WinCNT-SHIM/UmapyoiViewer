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


	// ��� ���� ������Ʈ�� �ϴ� Ʈ�������� ������
	gameObject->AddComponent<Transform>();
	//gameObject->SetName(_nodeName);
	gameObject->GetComponent<Transform>()->SetLocalTM(XMLoadFloat4x4(&_localTM));

	// �������� ������ �������� ���� ������Ʈ�� �����Ѵ�
	SettingGameObject(prefab, gameObject);

	// �ڽ� ������Ʈ�� ��������
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

		// �޽�ID�� ����
		renderingData->m_ObjectInfo->m_MeshID = _meshID;

		// Material ����
		if (_materialRef < 0 ||
			_materialRef >= prefab->_materialList.size())
		{
			// Material Ref�� ��ȿ���� ���� ���. �׳� ���θ� �ȴ�.
			renderingData->m_ShaderInfo->m_TechName = TEXT("Basic");
		}
		else
		{
			// Material Ref�� ��ȿ�ϸ� �����ϸ� ��
			std::shared_ptr<NodeMaterial> mat = prefab->_materialList[_materialRef];

			renderingData->m_MaterialInfo->m_Ambient = mat->_ambient;
			renderingData->m_MaterialInfo->m_Diffuse = mat->_diffuse;
			renderingData->m_MaterialInfo->m_Specular = mat->_specular;
			renderingData->m_MaterialInfo->m_Reflect = mat->_reflect;

			renderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID = mat->_diffuseMapID;
			renderingData->m_MaterialInfo->m_TextureInfo.m_NormalMapID = mat->_normalMapID;
			
			// ���׸��� ������ ������ �ؽ�ó�� ���� ���
			if (renderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID == UINT_MAX)
			{
			}
		}

		//if ((renderingData->m_MaterialInfo->m_DiffuseMapID != UINT_MAX)
		//	&& (renderingData->m_MaterialInfo->m_NormalMapID != UINT_MAX))
		//{
		//	//renderingData->m_ShaderInfo->m_VSName = TEXT("VS_DiffuseNormal");
		//	//renderingData->m_ShaderInfo->m_PSName = TEXT("PS_DiffuseNormal");
		//	renderingData->m_ShaderInfo->m_TechName = TEXT("Basic");	// �ϴ� �⺻ ��ũ
		//}
		//else if (renderingData->m_MaterialInfo->m_DiffuseMapID != UINT_MAX)
		//{
		//	//renderingData->m_ShaderInfo->m_VSName = TEXT("VS_Diffuse");
		//	//renderingData->m_ShaderInfo->m_PSName = TEXT("PS_Diffuse");
		//	renderingData->m_ShaderInfo->m_TechName = TEXT("Basic");	// �ϴ� �⺻ ��ũ
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
			// Material Ref�� ��ȿ���� ���� ���. �׳� ���θ� �ȴ�.
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

	// Final Root. ������ Ʈ������ ���� ���̰ų� �ϴ� ���� ������Ʈ
	//std::shared_ptr<GameObject> outObject = std::make_shared<GameObject>();
	std::shared_ptr<GameObject> outObject = m_FactoryGameObject->CreateGameObject(_prefabName);

	outObject->AddComponent<Transform>();

	// �������� ��� ������ �̿��ؼ� ���� ���� ������Ʈ���� ���� ���̴�
	std::map<std::string, std::shared_ptr<GameObject>> gameObjectByNodeNameMap;
	std::map<std::shared_ptr<NodeObjectBase>, std::shared_ptr<GameObject>> gameObjectByNodeMap;

	// ��Ʈ ���鿡 ���ؼ� �ִϸ��̼� ������ �����Ѵ�.
	for (int i = 0; i < _rootNodes.size(); i++)
	{
		std::shared_ptr<GameObject> rootObject = _rootNodes[i]->MakeGameObject(this, gameObjectByNodeMap);
		outObject->SetChild(rootObject);

		std::vector<std::string> hierarchy{ _rootNodes[i]->_nodeName };

		InitAnimTrackList(_rootNodes[i], hierarchy);
	}

	// �� ���� �� �˻��� ���� ���� ������
	for (const auto& pair : gameObjectByNodeMap)
	{
		gameObjectByNodeNameMap.emplace(pair.first->_nodeName, pair.second);
	}

	// �� ��Ű�� �ִϸ��̼� �غ�
	for (auto& node : gameObjectByNodeMap)
	{
		const auto nodeGeo = std::dynamic_pointer_cast<NodeGeoObject>(node.first);

		if (nodeGeo == nullptr)
			continue;

		if (nodeGeo->_boneNameList.empty())
			continue;

		// ������� ����ߴٸ� �� ���κ��� ������ ���� ������Ʈ�� ��Ų �޽��̴�.
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

	// ���� ��� ������Ʈ���� Animator Component�� �ٿ��ش�.
	const auto animator = outObject->AddComponent<Animator>();

	std::shared_ptr<AnimationClip> animClip = std::make_shared<AnimationClip>(_animPosTrackList, _animRotTrackList);

	animator->SetAnimClip(animClip);

	return outObject;
}

void PrefabASE::InitAnimTrackList(std::shared_ptr<NodeObjectBase> currentNode, std::vector<std::string> currentNodeHierarchy)
{
	//if (currentNode->_tmAnimation == nullptr)
	//	return;

	// ������ �ִϸ��̼� �ʱ�ȭ
	if (currentNode->_tmAnimation != nullptr
		&& currentNode->_tmAnimation->m_position.empty() == false)
	{
		// ���� �Ľ��� ���� ������ ���� �ƴ�...�� ���� ����!!
		std::vector<std::pair<unsigned, Vector3>> posTrack;
		for (auto& pos : currentNode->_tmAnimation->m_position)
			posTrack.push_back({ pos->m_time, pos->m_pos });

		_animPosTrackList.push_back({ currentNodeHierarchy, posTrack });
	}

	// ȸ�� �ִϸ��̼� �ʱ�ȭ
	if (currentNode->_tmAnimation != nullptr
		&& currentNode->_tmAnimation->m_rotation.empty() == false)
	{
		// ���� �Ľ��� ���� ������ ���� �ƴ�...�� ���� ����!!
		std::vector<std::pair<unsigned, Quaternion>> rotTrack;
		for (auto& rot : currentNode->_tmAnimation->m_rotation)
			rotTrack.push_back({ rot->m_time, rot->m_rotQT_accumulation });

		_animRotTrackList.push_back({ currentNodeHierarchy, rotTrack });
	}

	// ũ�� �ִϸ��̼��� ��� ������
	//if (currentNode->_tmAnimation->m_scale.empty() == false)
	//{
	//}

	// ���α��� ���̾��Ű�� �ȾƼ� �ִϸ��̼� Ʈ�� ����Ʈ�� �����Ѵ�.
	for (const auto& child : currentNode->_childNodes)
	{
		currentNodeHierarchy.push_back(child->_nodeName);
		InitAnimTrackList(child, currentNodeHierarchy);
		currentNodeHierarchy.pop_back();
	}

	//delete currentNode->_tmAnimation;
}
