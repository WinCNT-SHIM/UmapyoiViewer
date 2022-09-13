#include "GameEnginePCH.h"
#include "ResourceManager.h"

#include "../ParserLib/CASEParser.h"
#include "../ParserLib/Animation.h"
#include "PrefabASE.h"

// �׷��Ƚ� ������ Ŀ��Ʈ(...)�ϱ� ���� ���
#ifdef _DX11RENDERER
#include "../SSSGraphicsEngine/IGraphicsEngineBase.h"
#include "../SSSGraphicsEngine/DX11GraphicsEngine.h"
#include "../SSSGraphicsEngine/VertexStruct.h"
#endif

std::shared_ptr<ResourceManager> ResourceManager::m_Instance = nullptr;

const unsigned int ResourceManager::GetMeshID(const std::string& meshPath)
{
	return m_MeshIDs.find(meshPath) != m_MeshIDs.end() ? m_MeshIDs.at(meshPath) : UINT_MAX;
}

const unsigned int ResourceManager::GetTextureID(const std::string& texturePath)
{
	return m_TextureIDs.find(texturePath) != m_TextureIDs.end() ? m_TextureIDs.at(texturePath) : UINT_MAX;
}

const std::shared_ptr<PrefabASE> ResourceManager::GetPrefab(const std::string& prefabName)
{
	return m_PrefabASEs.find(prefabName) != m_PrefabASEs.end() ? m_PrefabASEs[prefabName] : nullptr;
}

void ResourceManager::LoadResource_Texture(const std::string& filePath)
{
	unsigned int textureID = UINT_MAX;
	std::shared_ptr<IGraphicsEngineBase> m_GraphicsEngine = IGraphicsEngineBase::GetGraphicsEngine();

	textureID = m_GraphicsEngine->LoadTexture(filePath);
	m_TextureIDs.emplace(filePath, textureID);
}

void ResourceManager::LoadResource_ASEFile(const std::string& filePath)
{
	// �̹� �ε������� ��ŵ
	if (this->GetPrefab(filePath) != nullptr)
		return;

	unsigned int meshID = UINT_MAX;
	std::shared_ptr<IGraphicsEngineBase> m_GraphicsEngine = IGraphicsEngineBase::GetGraphicsEngine();

	// ASE ���� �ε���(����ȭ����)
	CASEParser* m_pASEParser = LoadASEFile(filePath);

	// �ε��� ASE ���Ϸ� Prefab�� �����Ѵ�
	auto prefabASE = BuildPrefabFromASE(filePath, m_pASEParser);
	m_PrefabASEs.emplace(filePath, prefabASE);
	
	prefabASE->_prefabName = filePath;

	// �ε��� ASE ������ delete�Ѵ�
	delete m_pASEParser;
}

bool ResourceManager::LoadResource_Sound(const std::string& name, std::string filePath, unsigned int mode /*= 0*/)
{
	// FMOD_DEFAULT 0
	// FMOD_LOOP_OFF 1
	// FMOD_LOOP 2
	bool res = GetSoundManager()->LoadSoundFile(name, filePath, mode);
	return res;
}

void ResourceManager::Play_SoundResource(std::string name, IPlayMode mode)
{
	GetSoundManager()->Play(name, mode);
}

void ResourceManager::Stop_SoundResource(std::string name, IPlayMode mode)
{
	GetSoundManager()->Stop(name, mode);
}

void ResourceManager::SetVolume_SoundResource(float volume, IPlayMode mode)
{
	GetSoundManager()->SetSoundVoulume(volume, mode);
}

CASEParser* ResourceManager::LoadASEFile(const std::string& filePath)
{
	// ASE ���� �ε���(����ȭ����)
	CASEParser* m_pASEParser;
	m_pASEParser = new CASEParser();
	m_pASEParser->Init();
	LPSTR tmp = const_cast<char*>(filePath.c_str());
	bool res = m_pASEParser->Load(tmp);
	if (res)
		for (size_t i = 0; i < m_pASEParser->m_MeshList.size(); i++)
			m_pASEParser->Convert_Optimize(m_pASEParser->GetMesh(i));

	return m_pASEParser;
}

std::shared_ptr<PrefabASE> ResourceManager::BuildPrefabFromASE(std::string filePath, CASEParser* aseData)
{
	m_FilePath = filePath;

	std::shared_ptr<IGraphicsEngineBase> m_GraphicsEngine = IGraphicsEngineBase::GetGraphicsEngine();

	std::vector<std::shared_ptr<NodeObjectBase>> rootNodes;
	std::map<std::string, std::shared_ptr<NodeObjectBase>> nodeMap;

	std::vector<uint32> meshIDVector(aseData->m_MeshList.size());
	std::vector<uint32> materialRefVector(aseData->m_MeshList.size());
	std::vector<std::shared_ptr<NodeMaterial>> materialVector;


	// ��� ���͸��� ���� (���� �ݻ絵 �� ����ϴ� ���� ID) �� ����
	for (int i = 0; i < static_cast<int>(aseData->m_MaterialCount); i++)
	{
		std::shared_ptr<NodeMaterial> newMat = std::make_shared<NodeMaterial>();
		std::string diffuseFileName;
		if (aseData->m_MaterialList[i]->m_map_diffuse != nullptr)
		{
			diffuseFileName = aseData->m_MaterialList[i]->m_map_diffuse->m_bitmap;
		}
		else if (aseData->m_MaterialList[i]->m_map_generic != nullptr)
		{
			diffuseFileName = aseData->m_MaterialList[i]->m_map_generic->m_bitmap;
		}

		if (diffuseFileName != ""
			&& std::filesystem::path(diffuseFileName).has_extension())
		{
			std::string tempPath;
			tstring ext = std::filesystem::path(diffuseFileName).extension();

			// �ϴ� dds ���ϸ� �е��� ����
			// tif�� ��?��
			if (ext == _T(".dds")/* || ext == _T(".png") || ext == _T(".tga")*/)
			{
				tempPath = "Resource/Textures/" + diffuseFileName;
			}
			else
			{
				tstring fileName = std::filesystem::path(diffuseFileName).stem();
				std::string tmpName;
				tmpName.assign(fileName.begin(), fileName.end());
				tempPath = "Resource/Textures/" + tmpName + ".dds";
			}
			uint32 diffuseMapID = m_GraphicsEngine->LoadTexture(tempPath);
			newMat->_diffuseMapID = diffuseMapID;
		}
		else
			newMat->_diffuseMapID = UINT_MAX;

		newMat->_ambient = Vector4(aseData->m_MaterialList[i]->m_MaterialAmbient);
		newMat->_ambient.w = 1.f;
		newMat->_diffuse = Vector4(aseData->m_MaterialList[i]->m_MaterialDiffuse);
		newMat->_diffuse.w = 1.f;
		newMat->_specular = Vector4(aseData->m_MaterialList[i]->m_MaterialSpecular);
		newMat->_specular.w = 1.f;
		newMat->_reflect = XMFLOAT4{ 0.0f, 0.0f, 0.0f, 1.f };
		//newMat->_ambient = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		//newMat->_diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
		//newMat->_specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
		//newMat->_reflect = XMFLOAT4{ 0.0f, 0.0f, 0.0f, 1.f };

		materialVector.emplace_back(newMat);
	}


	// Geometry Object => Mesh ����
	for (int geoObjIdx = 0; geoObjIdx < aseData->m_MeshList.size(); geoObjIdx++)
	{
		std::string nodeName = aseData->m_MeshList[geoObjIdx]->m_Nodename;
		std::string nodeParent = aseData->m_MeshList[geoObjIdx]->m_NodeParent;

		Matrix nodeTM = Matrix(
			Vector4(aseData->m_MeshList[geoObjIdx]->m_TMRow0)
			, Vector4(aseData->m_MeshList[geoObjIdx]->m_TMRow1)
			, Vector4(aseData->m_MeshList[geoObjIdx]->m_TMRow2)
			, Vector4(aseData->m_MeshList[geoObjIdx]->m_TMRow3)
		);
		nodeTM._44 = 1.0f;

		const int& materialRef = aseData->m_MeshList[geoObjIdx]->m_MaterialRef;
		const auto& boneList = aseData->m_MeshList[geoObjIdx]->m_BoneList;
		const auto& wVertexs = aseData->m_MeshList[geoObjIdx]->m_WVertexs;

		auto& vertexList = aseData->m_MeshList[geoObjIdx]->m_opt_vertex;
		const auto& faceList = aseData->m_MeshList[geoObjIdx]->m_meshface;
		auto& indexList = aseData->m_MeshList[geoObjIdx]->m_opt_index;

		int faceCount = aseData->m_MeshList[geoObjIdx]->m_mesh_numfaces;
		int indexCount = faceCount * 3;


		// ���ؽ� ����, �ε��� ���� ����
		std::vector<GraphicsEngine::VertexStruct::VertexBasicSkinned> vertexBuffer(vertexList.size());
		std::vector<GraphicsEngine::VertexStruct::VertexBasic> staticVertexVector(vertexList.size());
		std::vector<uint32> indexBuffer(indexCount);

		// TM_ROW(���� ���)�� ����ķ� ���ؽ����� ���÷� ������ �ٲ۴�
		Matrix nodeTMInv = nodeTM.Invert();
		for (auto& vertex : vertexList)
		{
			const auto localPosition = Vector3::Transform(vertex->m_pos, nodeTMInv);
			vertex->m_pos = localPosition;
		}

		// �ε��� ���۸� ����(����)�Ѵ�
		for (int i = 0; i < faceCount; ++i)
		{
			int idx0 = i * 3 + 0;
			int idx1 = i * 3 + 1;
			int idx2 = i * 3 + 2;
			indexBuffer[idx0] = aseData->m_MeshList[geoObjIdx]->m_opt_index[i].index[0];
			indexBuffer[idx1] = aseData->m_MeshList[geoObjIdx]->m_opt_index[i].index[2];
			indexBuffer[idx2] = aseData->m_MeshList[geoObjIdx]->m_opt_index[i].index[1];
			//indexBuffer[idx0] = aseData->m_MeshList[geoObjIdx]->m_opt_index[i].index[0];
			//indexBuffer[idx1] = aseData->m_MeshList[geoObjIdx]->m_opt_index[i].index[1];
			//indexBuffer[idx2] = aseData->m_MeshList[geoObjIdx]->m_opt_index[i].index[2];
		}


		uint32 nodesMeshID = UINT_MAX;

		// ���ؽ� ���۸� �����ؼ� ���ҽ��� �����Ѵ�
		// ���� ���� �������� ���� ���ؽ��� ������ Static���� �ٲ۴�
		if (vertexList.size() != 0)
		{
			if (boneList.empty() == true)
			{
				for (int i = 0; i < vertexList.size(); i++)
				{
					staticVertexVector[i].Pos = vertexList[i]->m_pos;
					staticVertexVector[i].Normal = vertexList[i]->m_normal;
					staticVertexVector[i].Tex = Vector2(vertexList[i]->u, vertexList[i]->v);
					staticVertexVector[i].TangentL = vertexList[i]->m_tangent;
					staticVertexVector[i].Color = { 0.0f, 0.0f, 0.0f, 0.0f };
				}
				nodesMeshID = m_GraphicsEngine->LoadMesh(nodeName, staticVertexVector, indexBuffer);
			}
			else
			{
				for (int i = 0; i < vertexList.size(); i++)
				{
					vertexBuffer[i].Pos = vertexList[i]->m_pos;
					vertexBuffer[i].Normal = vertexList[i]->m_normal;
					vertexBuffer[i].Tex = Vector2(vertexList[i]->u, vertexList[i]->v);
					vertexBuffer[i].TangentL = vertexList[i]->m_tangent;
					vertexBuffer[i].Weights.x = vertexList[i]->m_bw1.second;
					vertexBuffer[i].Weights.y = vertexList[i]->m_bw2.second;
					vertexBuffer[i].Weights.z = vertexList[i]->m_bw3.second;
					vertexBuffer[i].BoneIndices[0] = vertexList[i]->m_bw1.first;
					vertexBuffer[i].BoneIndices[1] = vertexList[i]->m_bw2.first;
					vertexBuffer[i].BoneIndices[2] = vertexList[i]->m_bw3.first;
					vertexBuffer[i].BoneIndices[3] = vertexList[i]->m_bw4.first;
					vertexBuffer[i].Color = { 0.0f, 0.0f, 0.0f, 0.0f };
				}
				nodesMeshID = m_GraphicsEngine->LoadMesh(nodeName, vertexBuffer, indexBuffer);
			}
		}

		meshIDVector[geoObjIdx] = nodesMeshID;
		materialRefVector[geoObjIdx] = materialRef;

		auto newNodeObject = std::make_shared<NodeGeoObject>();
		newNodeObject->_nodeName = nodeName;
		newNodeObject->_nodeParent = nodeParent;
		newNodeObject->_nodeTM = nodeTM;
		newNodeObject->_meshID = nodesMeshID;
		newNodeObject->_tmAnimation = aseData->m_MeshList[geoObjIdx]->m_TMAnimation;
		aseData->m_MeshList[geoObjIdx]->m_TMAnimation = nullptr;	// �������� �̵�
		newNodeObject->_materialRef = materialRef;
		//newNodeObject->_wVertexs = wVertexs;

		for (int i = 0; i < boneList.size(); i++)
			newNodeObject->_boneNameList.emplace_back(boneList[i]->m_BoneName);


		nodeMap.emplace(nodeName, newNodeObject);
		m_MeshIDs.emplace(m_FilePath + "/" + nodeName, nodesMeshID);
	}

	//// Helper Object ����
	//for (int i = 0; i < aseData.helperObjectList.size(); i++)
	//{
	//	const auto& helperObject = aseData.helperObjectList[i];
	//	const tstring& nodeName = StringHelper::ToTString(helperObject.nodeName);
	//	const tstring& nodeParent = StringHelper::ToTString(helperObject.nodeParent);
	//	const ASEParsingData::NodeTM& nodeTM = helperObject.nodeTM;
	//	const ASEParsingData::TMAnimation& tmAnimation = helperObject.tmAnimation;
	//	auto newHelperObject = std::make_shared<NodeHelperObject>();
	//	newHelperObject->_nodeName = nodeName;
	//	newHelperObject->_nodeParent = nodeParent;
	//	newHelperObject->_nodeTM = nodeTM;
	//	newHelperObject->_tmAnimation = tmAnimation;
	//	nodeMap.insert({ nodeName, newHelperObject });
	//}

	//// Shape Object ����
	//for (int i = 0; i < aseData.shapeObjectList.size(); i++)
	//{
	//	const auto& shapeObject = aseData.shapeObjectList[i];
	//	const tstring& nodeName = StringHelper::ToTString(shapeObject.nodeName);
	//	const tstring& nodeParent = StringHelper::ToTString(shapeObject.nodeParent);
	//	const ASEParsingData::NodeTM& nodeTM = shapeObject.nodeTM;
	//	const ASEParsingData::TMAnimation& tmAnimation = shapeObject.tmAnimation;
	//	auto newShapeObject = std::make_shared<NodeShapeObject>();
	//	newShapeObject->_nodeName = nodeName;
	//	newShapeObject->_nodeParent = nodeParent;
	//	newShapeObject->_nodeTM = nodeTM;
	//	newShapeObject->_tmAnimation = tmAnimation;
	//	nodeMap.insert({ nodeName, newShapeObject });
	//}

	// ��尣�� ���� ���踦 ��������
	for (auto& node : nodeMap)
	{
		const std::string parentNodeName = node.second->_nodeParent;

		// �ֻ��� Root Node
		if (parentNodeName.empty())
		{
			rootNodes.emplace_back(node.second);
			node.second->_localTM = node.second->_nodeTM;
			continue;
		}

		// �θ� �ִ�. ã�Ƽ� ����������.
		const auto iter = nodeMap.find(parentNodeName);

		if (iter != nodeMap.end())
		{
			std::shared_ptr<NodeObjectBase> parentNode = iter->second;

			node.second->_parentNode = parentNode;
			parentNode->_childNodes.emplace_back(node.second);

			Matrix tmp = XMLoadFloat4x4(&parentNode->_nodeTM);
			Matrix parentWorldTMInv = tmp.Invert();

			node.second->_localTM = node.second->_nodeTM * parentWorldTMInv;
		}
		else
		{
			// �θ� ������ ã�� �� ��. ������ �� �� �Ǿ��ִ� ����̹Ƿ� ���� �߻�
			assert(false);
			continue;
		}
	}

	std::shared_ptr<PrefabASE> outPrefab = std::make_shared<PrefabASE>(rootNodes, nodeMap, materialVector);
	return outPrefab;
}

void ResourceManager::CreateBasicMesh()
{
	unsigned int meshID = UINT_MAX;
	std::shared_ptr<IGraphicsEngineBase> m_GraphicsEngine = IGraphicsEngineBase::GetGraphicsEngine();

	// Axis �ε�
	meshID = m_GraphicsEngine->LoadMesh("Axis", false, true);
	m_MeshIDs.emplace("Axis", meshID);

	// Grid �ε�
	meshID = m_GraphicsEngine->LoadMesh("Grid", false, true);
	m_MeshIDs.emplace("Grid", meshID);

	// �⺻ ť�� �ε�
	meshID = m_GraphicsEngine->LoadMesh("Cube", false, false);
	m_MeshIDs.emplace("Cube", meshID);
}

void ResourceManager::Init()
{
	// �⺻ �޽� �ε�
	CreateBasicMesh();
}

void ResourceManager::Release()
{
	m_MeshIDs.clear();
	m_TextureIDs.clear();

	for (auto& item : m_PrefabASEs)
	{
		item.second.reset();
	}
	m_PrefabASEs.clear();

	IGraphicsEngineBase::GetGraphicsEngine()->ReleaseResourceOnly();
}
