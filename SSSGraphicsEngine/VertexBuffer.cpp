#include "GraphicsEnginePCH.h"
#include "VertexBuffer.h"

using namespace GraphicsEngine;

GraphicsEngine::VertexBuffer::VertexBuffer()
	: BufferBase{ BufferType::VERTEX }
	, m_VB{ nullptr }
	, m_VertextCount{ 0 }
	, m_Stride{ 0 }
{
}

GraphicsEngine::VertexBuffer::~VertexBuffer()
{

}

void VertexBuffer::Init(std::shared_ptr<DXDevice> device, int vertexStructSize, int vertexCount, void* pFirstVertex)
{
	CreateBuffer(device, vertexStructSize, vertexCount, pFirstVertex);
}

void VertexBuffer::CreateBuffer(std::shared_ptr<DXDevice> device, int vertexStructSize, int vertexCount, void* pFirstVertex)

{
	// ������ ���۰� ������ �����Ѵ�
	ReleaseCOM(m_VB);

	m_VertextCount = vertexCount;
	m_Stride = vertexStructSize;

	// ==================== ���ؽ� ���۸� �����Ѵ� ====================
	// GPU�� ���ؽ� �迭�� �����Ϸ��� �迭�� ���ؽ��� ���۶�� Ư���� ���ҽ��� �����ؾ� �Ѵ�
	// Direct3D������ ID3D11Buffer �������̽��� ��ǥ���̴�

	// 1. ������ ���۸� �����ϴ� ����ü�� ä���
	/// <summary>
	/// D3D11_BUFFER_DESC ����ü
	/// </summary>
	/// <param name="ByteWidth">������ ���ؽ� ������ ����Ʈ ������ ũ��</param>
	/// <param name="Usage">���۰� ���Ǵ� ���(D3D11_USAGE ������)</param>
	/// <param name="BindFlags">���۰� ���������ο� ���ε��Ǵ� ����� ����</param>
	/// <param name="CPUAccessFlags">CPU�� ���ۿ� �����ϴ� ��Ŀ� ���� �÷��� ����</param>
	/// <param name="MiscFlags">��Ÿ �÷���(���ؽ� ���ۿ��� ������ �ʿ� ����)</param>
	/// <param name="StructureByteStride">������ ���ۿ� ����� ���� �ϳ��� ũ��(����Ʈ)
	///		������ ����: ���� ũ���� ������ ��� ����/// </param>
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = vertexStructSize * vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	// 2. ������ �ʱ�ȭ�� ����� �����͸� �����ϴ� D3D11_SUBRESOURCE_DATA ����ü�� ä���
	/// <summary>
	/// D3D11_SUBRESOURCE_DATA ����ü
	/// </summary>
	/// <param name="pSysMem">���ؽ� ���۸� �ʱ�ȭ�� �����͸� ���� �ý��� �޸� �迭�� ���� ������</param>
	/// <param name="SysMemPitch">���ؽ� ���ۿ��� ���ʿ�</param>
	/// <param name="SysMemSlicePitch">���ؽ� ���ۿ��� ���ʿ�</param>
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = pFirstVertex;

	// 3. ID3D11Device::CreateBuffer �Լ��� ���۸� �����Ѵ�
	HR(device->GetDevice()->CreateBuffer(&vbd, &vinitData, &m_VB));
}
