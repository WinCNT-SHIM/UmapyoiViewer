#include "GraphicsEnginePCH.h"
#include "ResourceManager.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"

#include "GeometryGenerator.h"

using namespace GraphicsEngine;

std::shared_ptr<GraphicsEngine::ResourceManager> GraphicsEngine::ResourceManager::m_Instance = nullptr;

std::shared_ptr<GraphicsEngine::Mesh> ResourceManager::GetMeshResource(const uint32& resourceID)
{
	return m_MeshResource.find(resourceID) != m_MeshResource.end() ? m_MeshResource.at(resourceID) : nullptr;
}

std::shared_ptr<GraphicsEngine::Texture> ResourceManager::GetTextureResource(const uint32& resourceID)
{
	return m_TextureResource.find(resourceID) != m_TextureResource.end() ? m_TextureResource.at(resourceID) : nullptr;
}

void ResourceManager::Init()
{

}

void ResourceManager::Release()
{
	// 모든 리소스들을 해제하자
	
	// Mk.1
	// VertexBuffer
	for (auto& _vector : m_VertexBuffers)
	{
		for (auto& _item : _vector.second)
		{
			delete _item;
		}
		_vector.second.clear();
	}
	m_VertexBuffers.clear();

	// IndexBuffer
	for (auto& _vector : m_IndexBuffer)
	{
		for (auto& _item : _vector.second)
		{
			delete _item;
		}
		_vector.second.clear();
	}
	m_IndexBuffer.clear();


	// Mk.2
	// 메쉬
	for (auto& mesh : m_MeshResource)
		if (mesh.second != nullptr)
			mesh.second->Release();
	m_MeshResource.clear();
	
	// 텍스처
	for (auto& texture : m_TextureResource)
		if (texture.second != nullptr)
			texture.second->Release();
	m_TextureResource.clear();
}

void ResourceManager::GetResource_ASEFile(std::shared_ptr<DXDevice> device, std::string filePath, std::vector<VertexBuffer*>& outputVB, std::vector<IndexBuffer*>& outputIB)
{
	LPSTR tmp = const_cast<char*>(filePath.c_str());
	GetResource_ASEFile(device, tmp, outputVB, outputIB);
}

void ResourceManager::GetResource_ASEFile(std::shared_ptr<DXDevice> device, const char* filePath, std::vector<VertexBuffer*>& outputVB, std::vector<IndexBuffer*>& outputIB)
{
	GetResource_ASEFile(device, (LPSTR)filePath, outputVB, outputIB);
}

void ResourceManager::GetResource_ASEFile(std::shared_ptr<DXDevice> device, const LPSTR& filePath, std::vector<VertexBuffer*>& outputVB, std::vector<IndexBuffer*>& outputIB)
{
	bool _res = true;

	// 이미 로드된 리소스가 있으면 그것을 반환한다
	if (CheckLoadedResource(filePath))
	{
		outputVB = m_VertexBuffers[filePath];
		outputIB = m_IndexBuffer[filePath];
	}
	// 없는 경우에는 로드한다
	else
	{
		_res = LoadResource_ASEFile(device, filePath);
	}

	// 리소스 로드에 성공했으면 리소스를 돌려준다
	if (_res)
	{
		outputVB = m_VertexBuffers[filePath];
		// 리퍼런스 카운터 증가
		for (auto& elem : outputVB)
			elem->AddReferenceCount();

		outputIB = m_IndexBuffer[filePath];
		// 리퍼런스 카운터 증가
		for (auto& elem : outputIB)
			elem->AddReferenceCount();
	}
}

bool ResourceManager::CheckLoadedResource(std::string filePath)
{
	if (m_VertexBuffers[filePath].size() == 0)
		return false;

	if (m_IndexBuffer[filePath].size() == 0)
		return false;

	return true;
}

bool ResourceManager::LoadResource_ASEFile(std::shared_ptr<DXDevice> device, std::string filePath)
{
	bool res = true;
	LPSTR tmp = const_cast<char*>(filePath.c_str());
	res = LoadResource_ASEFile(device, tmp);
	return res;
}

bool ResourceManager::LoadResource_ASEFile(std::shared_ptr<DXDevice> device, const char* filePath)
{
	bool res = true;
	LoadResource_ASEFile(device, (LPSTR)filePath);
	return res;
}

bool ResourceManager::LoadResource_ASEFile(std::shared_ptr<DXDevice> device, const LPSTR& filePath)
{
	bool res = true;

	// 파서를 생성함
	// 처음에는 파서를 멤버 변수로 가지고 있을까 했는데 아직 유의미한 필요성을 못 느껴서 리소스 로드 시에 지역 변수로 생성하기로 함
	unique_ptr<CASEParser> m_pASEParser;
	m_pASEParser = make_unique<CASEParser>();
	m_pASEParser->Init();
	res = m_pASEParser->Load(filePath);

	if (res == false) return res;

	// 파싱한 데이터에 대한 처리
	for (const auto& _mesh : m_pASEParser->m_MeshList)
	{

		// 파싱한 데이터로  버텍스 정보를 가지고 벡터로 만듦
		// todo: 최적화할 여지가 있는 부분 --> 파싱한 데이터와 엔진의 구조가 일치하지 않아서 값 복사를 하고 있다
		UINT _vertexCount = _mesh->m_opt_vertex.size();

		// 버텍스의 정보가 없는 오브젝트는 이후의 처리를 스킵함
		if (_vertexCount == 0)
			continue;

		// 버텍스 정보가 있는 경우는 버텍스 버퍼를 만들자
		std::vector<VertexStruct::VertexBasicSkinned> _Vertices(_vertexCount);


#pragma region 파싱한 데이터로 버텍스 버퍼를 만들자
		for (int i = 0; i < _vertexCount; i++)
		{
			// Position
			_Vertices[i].Pos = _mesh->m_opt_vertex[i]->m_pos;

			// Normal
			_Vertices[i].Normal.x = _mesh->m_opt_vertex[i]->m_normal.x;
			_Vertices[i].Normal.y = _mesh->m_opt_vertex[i]->m_normal.y;
			_Vertices[i].Normal.z = _mesh->m_opt_vertex[i]->m_normal.z;

			// Texture
			_Vertices[i].Tex.x = _mesh->m_opt_vertex[i]->u;
			_Vertices[i].Tex.y = _mesh->m_opt_vertex[i]->v;

			// Tangent Space
			_Vertices[i].TangentL = _mesh->m_opt_vertex[i]->m_tangent;

			// Skinning
			_Vertices[i].Weights.x = _mesh->m_opt_vertex[i]->m_bw1.second;
			_Vertices[i].Weights.y = _mesh->m_opt_vertex[i]->m_bw2.second;
			_Vertices[i].Weights.z = _mesh->m_opt_vertex[i]->m_bw3.second;
			_Vertices[i].BoneIndices[0] = _mesh->m_opt_vertex[i]->m_bw1.first;
			_Vertices[i].BoneIndices[1] = _mesh->m_opt_vertex[i]->m_bw2.first;
			_Vertices[i].BoneIndices[2] = _mesh->m_opt_vertex[i]->m_bw3.first;
			_Vertices[i].BoneIndices[3] = _mesh->m_opt_vertex[i]->m_bw4.first;
		}

		m_VertexBuffers[filePath].emplace_back(new VertexBuffer());
		VertexBuffer* _tmpVB = m_VertexBuffers[filePath].back();
		_tmpVB->Init(device, sizeof(_Vertices[0]), _vertexCount, _Vertices.data());
#pragma endregion 파싱한 데이터로 버텍스 버퍼를 만들자

#pragma region 파싱한 데이터로 인덱스 버퍼를 만들자
		// face의 3배수(== 인덱스의 수)만큼의 vector를 만들자
		UINT _IndexCount = _mesh->m_mesh_numfaces * 3;
		std::vector<UINT> _Indices(_IndexCount);

		// 페이스만큼의 
		for (UINT i = 0; i < _mesh->m_mesh_numfaces; ++i)
		{
			// 산술 오버 플로에 대한 경고를 피하기 위해
			UINT _i0 = i * 3 + 0;
			UINT _i1 = i * 3 + 1;
			UINT _i2 = i * 3 + 2;

			// todo: 파서와 함께 개선의 여지가 있는 부분
			// 원래는 파서에서 인덱스의 순서를 뒤집어줘야 할 듯?
			_Indices[_i0] = _mesh->m_opt_index[i].index[0];
			_Indices[_i1] = _mesh->m_opt_index[i].index[2];
			_Indices[_i2] = _mesh->m_opt_index[i].index[1];
		}

		m_IndexBuffer[filePath].emplace_back(new IndexBuffer());
		IndexBuffer* _tmpIB = m_IndexBuffer[filePath].back();
		_tmpIB->Init(device, _IndexCount, _Indices.data());
#pragma endregion 파싱한 데이터로 인덱스 버퍼를 만들자
	}

	return res;
}

const uint32& ResourceManager::CreateAxisMesh()
{
	// 버텍스 버퍼
	std::vector<VertexStruct::VertexPosColor> vertices(6);

	vertices[0].Pos = Vector3(0.f, 0.f, 0.f);
	vertices[0].Color = Vector4(1, 0, 0, 1);

	vertices[1].Pos = Vector3(1000.f, 0.f, 0.f);
	vertices[1].Color = Vector4(1, 0, 0, 1);

	vertices[2].Pos = Vector3(0.f, 0.f, 0.f);
	vertices[2].Color = Vector4(0, 1, 0, 1);

	vertices[3].Pos = Vector3(0.f, 1000.f, 0.f);
	vertices[3].Color = Vector4(0, 1, 0, 1);

	vertices[4].Pos = Vector3(0.f, 0.f, 0.f);
	vertices[4].Color = Vector4(0, 0, 1, 1);

	vertices[5].Pos = Vector3(0.f, 0.f, 1000.f);
	vertices[5].Color = Vector4(0, 0, 1, 1);


	// 인덱스 버퍼
	std::vector<unsigned int> indices(6);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	// 메쉬 생성
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	mesh->Initialize<VertexStruct::VertexPosColor>(
		"Axis"
		, vertices
		, indices
		, false
		, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	m_MeshResource.emplace(mesh->GetID(), mesh);
	return mesh->GetID();
}

const uint32& ResourceManager::CreateGridMesh()
{
	// GeometryGenerator에서 그리드 정보를 생성
	GeometryGenerator geometryGenerator;
	float width = 100.f; float depth = 100.f;	// 그리드 전체의 가로 세로 길이
	UINT m = 100; UINT n = 100;				// 그리드의 가로 세로 격자 수
	GeometryGenerator::MeshData meshData;
	geometryGenerator.CreateGrid(width, depth, m, n, meshData);


	// 버텍스 버퍼
	std::vector<VertexStruct::VertexPosColor> vertices(meshData.Vertices.size());
	Vector4 _color = Vector4(1.f, 1.f, 1.f, 1.f);
	
	for (int i = 0; i < meshData.Vertices.size(); i++)
	{
		vertices[i].Pos = std::move(meshData.Vertices[i].Position);
		vertices[i].Color = Vector4(_color.x, _color.y, _color.z, _color.w);	// 플로랄 화이트?
	}

	// 인덱스 버퍼
	std::vector<unsigned int> indices(meshData.Indices.size());

	for (int i = 0; i < meshData.Indices.size(); i++)
	{
		indices[i] = meshData.Indices[i];
	}
	// 잘리는 테두리 수동(...)으로 보완
	indices.emplace_back(0);
	indices.emplace_back(m * n - n);
	indices.emplace_back(m * n - n);
	indices.emplace_back(m * n - 1);


	// 메쉬 생성
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	mesh->Initialize<VertexStruct::VertexPosColor>(
		"Grid"
		, vertices
		, indices
		, false
		, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	m_MeshResource.emplace(mesh->GetID(), mesh);
	return mesh->GetID();
}

const uint32& ResourceManager::CreateCubeMesh()
{
	// GeometryGenerator에서 그리드 정보를 생성
	GeometryGenerator geometryGenerator;
	GeometryGenerator::MeshData meshData;
	geometryGenerator.CreateBox(1.f, 1.f, 1.f, meshData);

	// 버텍스 버퍼
	std::vector<VertexStruct::VertexBasic> vertices(meshData.Vertices.size());
	Vector4 _color = Vector4(1.f, 1.f, 1.f, 1.f);

	for (int i = 0; i < meshData.Vertices.size(); i++)
	{
		vertices[i].Pos = std::move(meshData.Vertices[i].Position);
		vertices[i].Normal = std::move(meshData.Vertices[i].Normal);
		vertices[i].Tex = std::move(meshData.Vertices[i].TexC);
		vertices[i].TangentL = std::move(meshData.Vertices[i].TangentU);
		vertices[i].Color = Vector4{ 1.f, 1.f, 1.f, 1.f };
	}

	// 인덱스 버퍼
	std::vector<unsigned int> indices(meshData.Indices.size());

	for (int i = 0; i < meshData.Indices.size(); i++)
	{
		indices[i] = meshData.Indices[i];
	}


	// 메쉬 생성
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

	mesh->Initialize<VertexStruct::VertexBasic>(
		"Cube"
		, vertices
		, indices
		, false
		, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_MeshResource.emplace(mesh->GetID(), mesh);
	return mesh->GetID();
}

const uint32& ResourceManager::LoadTexture(const std::string& path)
{
	std::shared_ptr<Texture> newTex = std::make_shared<Texture>();
	newTex->Initialize(path);
	m_TextureResource.emplace(newTex->GetID(), newTex);
	return newTex->GetID();
}
