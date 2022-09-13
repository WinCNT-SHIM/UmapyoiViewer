/// <summary>
/// 리소스 매니저 클래스
/// 리소스들을 로드하고 관리하는 역할을 한다
/// 
/// Created : 2022/07/10 심승수
/// Updated : 2022/07/10 심승수
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
		// ResourceManager를 싱글톤으로 만들자
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



		// 관리하는 리소스들
		// 버텍스 버퍼와 인덱스 버퍼
		map<string, vector<VertexBuffer*>> m_VertexBuffers;
		map<string, vector<IndexBuffer*>>  m_IndexBuffer;

		// 텍스처
		// ...
		// 기타 등등

	private:
		// 처음에는 파서를 멤버 변수로 가지고 있을까 했는데 아직 유의미한 필요성을 못 느껴서 리소스 로드 시에 지역 변수로 생성하기로 함
		//unique_ptr<CASEParser> m_pASEParser;


	public:
		// Mk.2
		std::shared_ptr<Mesh> GetMeshResource(const uint32& resourceID);
		std::shared_ptr<Texture> GetTextureResource(const uint32& resourceID);


		// MK.1
		// 리소스를 반환하는 함수
		void GetResource_ASEFile(std::shared_ptr<DXDevice> device, std::string filePath, std::vector<VertexBuffer*>& outputVB, std::vector<IndexBuffer*>& outputIB);
		void GetResource_ASEFile(std::shared_ptr<DXDevice> device, const char* filePath, std::vector<VertexBuffer*>& outputVB, std::vector<IndexBuffer*>& outputIB);
		void GetResource_ASEFile(std::shared_ptr<DXDevice> device, const LPSTR& filePath, std::vector<VertexBuffer*>& outputVB, std::vector<IndexBuffer*>& outputIB);

	private:
		// 리소스가 로드되어 있는지 확인하는 함수
		bool CheckLoadedResource(string filePath);

		// Axis 생성 함수
		const uint32& CreateAxisMesh();
		const uint32& CreateGridMesh();
		const uint32& CreateCubeMesh();

	public:
		// 리소스를 로드하는 함수
		bool LoadResource_ASEFile(shared_ptr<DXDevice> device, string filePath);
		bool LoadResource_ASEFile(shared_ptr<DXDevice> device, const char* filePath);
		bool LoadResource_ASEFile(shared_ptr<DXDevice> device, const LPSTR& filePath);

		// 메쉬를 로드하는 함수
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


		// 텍스처를 로드하는 함수
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
		// 특정 오브젝트인 경우는 리소스 매니저에서 바로 만든다
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

		// 일반적인 경우는 (ASE에서)파싱한 데이터로 메쉬를 생성한다
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