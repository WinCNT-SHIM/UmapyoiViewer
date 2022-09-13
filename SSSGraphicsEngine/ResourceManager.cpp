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
	// ��� ���ҽ����� ��������
	
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
	// �޽�
	for (auto& mesh : m_MeshResource)
		if (mesh.second != nullptr)
			mesh.second->Release();
	m_MeshResource.clear();
	
	// �ؽ�ó
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

	// �̹� �ε�� ���ҽ��� ������ �װ��� ��ȯ�Ѵ�
	if (CheckLoadedResource(filePath))
	{
		outputVB = m_VertexBuffers[filePath];
		outputIB = m_IndexBuffer[filePath];
	}
	// ���� ��쿡�� �ε��Ѵ�
	else
	{
		_res = LoadResource_ASEFile(device, filePath);
	}

	// ���ҽ� �ε忡 ���������� ���ҽ��� �����ش�
	if (_res)
	{
		outputVB = m_VertexBuffers[filePath];
		// ���۷��� ī���� ����
		for (auto& elem : outputVB)
			elem->AddReferenceCount();

		outputIB = m_IndexBuffer[filePath];
		// ���۷��� ī���� ����
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

	// �ļ��� ������
	// ó������ �ļ��� ��� ������ ������ ������ �ߴµ� ���� ���ǹ��� �ʿ伺�� �� ������ ���ҽ� �ε� �ÿ� ���� ������ �����ϱ�� ��
	unique_ptr<CASEParser> m_pASEParser;
	m_pASEParser = make_unique<CASEParser>();
	m_pASEParser->Init();
	res = m_pASEParser->Load(filePath);

	if (res == false) return res;

	// �Ľ��� �����Ϳ� ���� ó��
	for (const auto& _mesh : m_pASEParser->m_MeshList)
	{

		// �Ľ��� �����ͷ�  ���ؽ� ������ ������ ���ͷ� ����
		// todo: ����ȭ�� ������ �ִ� �κ� --> �Ľ��� �����Ϳ� ������ ������ ��ġ���� �ʾƼ� �� ���縦 �ϰ� �ִ�
		UINT _vertexCount = _mesh->m_opt_vertex.size();

		// ���ؽ��� ������ ���� ������Ʈ�� ������ ó���� ��ŵ��
		if (_vertexCount == 0)
			continue;

		// ���ؽ� ������ �ִ� ���� ���ؽ� ���۸� ������
		std::vector<VertexStruct::VertexBasicSkinned> _Vertices(_vertexCount);


#pragma region �Ľ��� �����ͷ� ���ؽ� ���۸� ������
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
#pragma endregion �Ľ��� �����ͷ� ���ؽ� ���۸� ������

#pragma region �Ľ��� �����ͷ� �ε��� ���۸� ������
		// face�� 3���(== �ε����� ��)��ŭ�� vector�� ������
		UINT _IndexCount = _mesh->m_mesh_numfaces * 3;
		std::vector<UINT> _Indices(_IndexCount);

		// ���̽���ŭ�� 
		for (UINT i = 0; i < _mesh->m_mesh_numfaces; ++i)
		{
			// ��� ���� �÷ο� ���� ��� ���ϱ� ����
			UINT _i0 = i * 3 + 0;
			UINT _i1 = i * 3 + 1;
			UINT _i2 = i * 3 + 2;

			// todo: �ļ��� �Բ� ������ ������ �ִ� �κ�
			// ������ �ļ����� �ε����� ������ ��������� �� ��?
			_Indices[_i0] = _mesh->m_opt_index[i].index[0];
			_Indices[_i1] = _mesh->m_opt_index[i].index[2];
			_Indices[_i2] = _mesh->m_opt_index[i].index[1];
		}

		m_IndexBuffer[filePath].emplace_back(new IndexBuffer());
		IndexBuffer* _tmpIB = m_IndexBuffer[filePath].back();
		_tmpIB->Init(device, _IndexCount, _Indices.data());
#pragma endregion �Ľ��� �����ͷ� �ε��� ���۸� ������
	}

	return res;
}

const uint32& ResourceManager::CreateAxisMesh()
{
	// ���ؽ� ����
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


	// �ε��� ����
	std::vector<unsigned int> indices(6);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 4;
	indices[5] = 5;

	// �޽� ����
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
	// GeometryGenerator���� �׸��� ������ ����
	GeometryGenerator geometryGenerator;
	float width = 100.f; float depth = 100.f;	// �׸��� ��ü�� ���� ���� ����
	UINT m = 100; UINT n = 100;				// �׸����� ���� ���� ���� ��
	GeometryGenerator::MeshData meshData;
	geometryGenerator.CreateGrid(width, depth, m, n, meshData);


	// ���ؽ� ����
	std::vector<VertexStruct::VertexPosColor> vertices(meshData.Vertices.size());
	Vector4 _color = Vector4(1.f, 1.f, 1.f, 1.f);
	
	for (int i = 0; i < meshData.Vertices.size(); i++)
	{
		vertices[i].Pos = std::move(meshData.Vertices[i].Position);
		vertices[i].Color = Vector4(_color.x, _color.y, _color.z, _color.w);	// �÷ζ� ȭ��Ʈ?
	}

	// �ε��� ����
	std::vector<unsigned int> indices(meshData.Indices.size());

	for (int i = 0; i < meshData.Indices.size(); i++)
	{
		indices[i] = meshData.Indices[i];
	}
	// �߸��� �׵θ� ����(...)���� ����
	indices.emplace_back(0);
	indices.emplace_back(m * n - n);
	indices.emplace_back(m * n - n);
	indices.emplace_back(m * n - 1);


	// �޽� ����
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
	// GeometryGenerator���� �׸��� ������ ����
	GeometryGenerator geometryGenerator;
	GeometryGenerator::MeshData meshData;
	geometryGenerator.CreateBox(1.f, 1.f, 1.f, meshData);

	// ���ؽ� ����
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

	// �ε��� ����
	std::vector<unsigned int> indices(meshData.Indices.size());

	for (int i = 0; i < meshData.Indices.size(); i++)
	{
		indices[i] = meshData.Indices[i];
	}


	// �޽� ����
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
