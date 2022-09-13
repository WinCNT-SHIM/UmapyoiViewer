#pragma once

#include "ResourceBase.h"
#include "VertexStruct.h"
#include "Renderer.h"

namespace GraphicsEngine
{
	class Mesh
		: public ResourceBase<ResourceType::MESH>
	{
	public:
		template <typename TVertex = VertexStruct::VertexBasic>
		void Initialize(
			const std::string& meshName
			, const std::vector<TVertex>& vertexBuffer
			, const std::vector<uint32>& indexBuffer
			, bool isDynamicMesh = false
			, D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		void Render();
		virtual void Release() override;

		ComPtr<ID3D11Buffer> m_VertexBuffer;
		ComPtr<ID3D11Buffer> m_IndexBuffer;
		D3D11_PRIMITIVE_TOPOLOGY m_Topology;

		uint32 m_Stride;
		uint32 m_Offset;
		uint32 m_IndexBufferSize;
	};

	/// <summary>
	/// �޽� ���ҽ��� ����� �Լ�
	/// </summary>
	/// <typeparam name="TVertex">���ؽ� ����</typeparam>
	/// <param name="meshName">�޽� �̸�</param>
	/// <param name="vertexBuffer">���ؽ� ����</param>
	/// <param name="indexBuffer">�ε��� ����</param>
	/// <param name="isDynamicMesh">���� ���� ���� ����</param>
	/// <param name="topology">��������</param>
	template <typename TVertex>
	void Mesh::Initialize(
		const std::string& meshName
		, const std::vector<TVertex>& vertexBuffer
		, const std::vector<uint32>& indexBuffer
		, bool isDynamicMesh /*= false*/
		, D3D11_PRIMITIVE_TOPOLOGY topology /*= D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST*/)
	{
		m_ResourceName = meshName;
		m_Topology = topology;

		// Vertex Buffer ����
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.ByteWidth = sizeof(TVertex) * static_cast<unsigned int>(vertexBuffer.size());

		if (!isDynamicMesh)
		{
			vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			vertexBufferDesc.CPUAccessFlags = 0;
		}
		else
		{
			vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA vertexInitData;
		vertexInitData.pSysMem = vertexBuffer.data();

		Renderer::GetInstance()->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexInitData, &m_VertexBuffer);

		m_Stride = sizeof(TVertex);
		m_Offset = 0;


		// Index Buffer ����
		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.ByteWidth = sizeof(uint32) * static_cast<uint32>(indexBuffer.size());

		if (!isDynamicMesh)
		{
			indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
			indexBufferDesc.CPUAccessFlags = 0;
		}
		else
		{
			indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		}

		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		D3D11_SUBRESOURCE_DATA indexInitData;
		indexInitData.pSysMem = indexBuffer.data();

		Renderer::GetInstance()->GetDevice()->CreateBuffer(&indexBufferDesc, &indexInitData, &m_IndexBuffer);

		m_IndexBufferSize = static_cast<uint32>(indexBuffer.size());
	}
}