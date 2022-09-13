/// <summary>
/// ���ҽ� �Ŵ��� Ŭ����
/// ���ҽ����� �ε��ϰ� �����ϴ� ������ �Ѵ�
/// 
/// Created : 2022/07/10 �ɽ¼�
/// Updated : 2022/07/10 �ɽ¼�
/// </summary>
#pragma once

#include "Mesh.h"
#include "Texture.h"

using std::map;
using std::string;
using std::vector;
using std::shared_ptr;

namespace GraphicsEngine
{
	class VertexBuffer;
	class IndexBuffer;

	class ResourceManager
	{
		// ResourceManager�� �̱������� ������
	public:
		ResourceManager() = default;
		ResourceManager(const ResourceManager& ref) = delete;
		ResourceManager& operator=(const ResourceManager& ref) = delete;
		~ResourceManager() = default;
	private:
		static std::shared_ptr<ResourceManager> m_Instance;

	public:
		static std::shared_ptr<ResourceManager> GetInstance()
		{
			if (m_Instance == nullptr)
			{
				m_Instance = std::make_shared<GraphicsEngine::ResourceManager>();
				//m_Instance->Init();
			}
			return m_Instance;
		}

	private:
		std::unordered_map<uint32, std::shared_ptr<Mesh>> m_MeshResource;
		std::unordered_map<uint32, std::shared_ptr<Texture>> m_TextureResource;



		// �����ϴ� ���ҽ���
		// ���ؽ� ���ۿ� �ε��� ����
		map<string, vector<VertexBuffer*>> m_VertexBuffers;
		map<string, vector<IndexBuffer*>>  m_IndexBuffer;

		// �ؽ�ó
		// ...
		// ��Ÿ ���

	private:
		// ó������ �ļ��� ��� ������ ������ ������ �ߴµ� ���� ���ǹ��� �ʿ伺�� �� ������ ���ҽ� �ε� �ÿ� ���� ������ �����ϱ�� ��
		//unique_ptr<CASEParser> m_pASEParser;


	public:
		// Mk.2
		std::shared_ptr<Mesh> GetMeshResource(const uint32& resourceID);
		std::shared_ptr<Texture> GetTextureResource(const uint32& resourceID);


		// MK.1
		// ���ҽ��� ��ȯ�ϴ� �Լ�
		void GetResource_ASEFile(std::shared_ptr<DXDevice> device, std::string filePath, std::vector<VertexBuffer*>& outputVB, std::vector<IndexBuffer*>& outputIB);
		void GetResource_ASEFile(std::shared_ptr<DXDevice> device, const char* filePath, std::vector<VertexBuffer*>& outputVB, std::vector<IndexBuffer*>& outputIB);
		void GetResource_ASEFile(std::shared_ptr<DXDevice> device, const LPSTR& filePath, std::vector<VertexBuffer*>& outputVB, std::vector<IndexBuffer*>& outputIB);

	private:
		// ���ҽ��� �ε�Ǿ� �ִ��� Ȯ���ϴ� �Լ�
		bool CheckLoadedResource(string filePath);

		// Axis ���� �Լ�
		const uint32& CreateAxisMesh();
		const uint32& CreateGridMesh();
		const uint32& CreateCubeMesh();

	public:
		// ���ҽ��� �ε��ϴ� �Լ�
		bool LoadResource_ASEFile(shared_ptr<DXDevice> device, string filePath);
		bool LoadResource_ASEFile(shared_ptr<DXDevice> device, const char* filePath);
		bool LoadResource_ASEFile(shared_ptr<DXDevice> device, const LPSTR& filePath);

		// �޽��� �ε��ϴ� �Լ�
		template <typename TVertex = VertexStruct::VertexBasic>
		const uint32& LoadMesh(
			const std::string& meshName
			, bool isDynamic = false
			, bool isLineList = false
		);

		template <typename TVertex = VertexStruct::VertexBasic>
		const uint32& LoadMesh(
			const std::string& meshName
			, const std::vector<TVertex>& vertexVector
			, const std::vector<uint32>& indexVector
			, bool isDynamic = false, bool isLineList = false
		);


		// �ؽ�ó�� �ε��ϴ� �Լ�
		const uint32& LoadTexture(const std::string& path);

	public:
		void Init();
		void Release();
	};

	template <typename TVertex /*= VertexStruct::VertexBasic*/>
	const uint32& GraphicsEngine::ResourceManager::LoadMesh(
		const std::string& meshName
		, bool isDynamic /*= false*/
		, bool isLineList /*= false*/)
	{
		// Ư�� ������Ʈ�� ���� ���ҽ� �Ŵ������� �ٷ� �����
		if (meshName == "Axis")
		{
			return CreateAxisMesh();
		}
		else if (meshName == "Grid")
		{
			return CreateGridMesh();
		}
		else if (meshName == "Cube")
		{
			return CreateCubeMesh();
		}

		// �Ϲ����� ���� (ASE����)�Ľ��� �����ͷ� �޽��� �����Ѵ�
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		if (!isLineList)
		{
			//mesh->Initialize<TVertex>(
			//	meshName
			//	, isDynamic
			//	, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST
			//	);
		}
		else
		{
			//mesh->Initialize<TVertex>(meshName, isDynamic, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		}


		m_MeshResource.emplace(mesh->GetID(), mesh);
		return mesh->GetID();
	}

	template <typename TVertex /*= VertexStruct::VertexBasic*/>
	const uint32& GraphicsEngine::ResourceManager::LoadMesh(
		const std::string& meshName
		, const std::vector<TVertex>& vertexVector
		, const std::vector<uint32>& indexVector
		, bool isDynamic /*= false*/
		, bool isLineList /*= false */
	)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		if (!isLineList)
			mesh->Initialize<TVertex>(meshName, vertexVector, indexVector, isDynamic, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		else
			mesh->Initialize<TVertex>(meshName, vertexVector, indexVector, isDynamic, D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

		m_MeshResource.emplace(mesh->GetID(), mesh);
		return mesh->GetID();
	}

}