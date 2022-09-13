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
	// 기존의 버퍼가 있으면 해제한다
	ReleaseCOM(m_IB);

	m_IndexCount = indexCount;

	// ==================== 인덱스 버퍼를 생성한다 ====================
	// 1. 생성할 버퍼를 서술하는 구조체를 채운다
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(D3D11_BUFFER_DESC));
	ibd.ByteWidth = sizeof(UINT) * m_IndexCount;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER; // 인덱스 버퍼 생성 시의 플래그
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	
	// 2. 버퍼의 초기화에 사용할 데이터를 서술하는 D3D11_SUBRESOURCE_DATA 구조체를 채운다
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = pFirstElement;

	// 3. ID3D11Device::CreateBuffer 함수로 버퍼를 생성한다
	HR(device->GetDevice()->CreateBuffer(&ibd, &iinitData, &m_IB));
}
