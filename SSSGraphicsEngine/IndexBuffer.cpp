#include "GraphicsEnginePCH.h"
#include "IndexBuffer.h"

using namespace GraphicsEngine;

IndexBuffer::IndexBuffer()
	: BufferBase{ BufferType::INDEX }
	, m_IB{ nullptr }
	, m_IndexCount{ 0 }
{

}

IndexBuffer::~IndexBuffer()
{

}

void IndexBuffer::Init(std::shared_ptr<DXDevice> device, int indexCount, void* pFirstElement)
{
	CreateBuffer(device, indexCount, pFirstElement);
}

void IndexBuffer::CreateBuffer(std::shared_ptr<DXDevice> device, int indexCount, void* pFirstElement)
{
	// ������ ���۰� ������ �����Ѵ�
	ReleaseCOM(m_IB);

	m_IndexCount = indexCount;

	// ==================== �ε��� ���۸� �����Ѵ� ====================
	// 1. ������ ���۸� �����ϴ� ����ü�� ä���
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(D3D11_BUFFER_DESC));
	ibd.ByteWidth = sizeof(UINT) * m_IndexCount;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // �ε��� ���� ���� ���� �÷���
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	
	// 2. ������ �ʱ�ȭ�� ����� �����͸� �����ϴ� D3D11_SUBRESOURCE_DATA ����ü�� ä���
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = pFirstElement;

	// 3. ID3D11Device::CreateBuffer �Լ��� ���۸� �����Ѵ�
	HR(device->GetDevice()->CreateBuffer(&ibd, &iinitData, &m_IB));
}
