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
	// 기존의 버퍼가 있으면 해제한다
	ReleaseCOM(m_VB);

	m_VertextCount = vertexCount;
	m_Stride = vertexStructSize;

	// ==================== 버텍스 버퍼를 생성한다 ====================
	// GPU가 버텍스 배열에 접근하려면 배열의 버텍스을 버퍼라는 특별한 리소스에 저장해야 한다
	// Direct3D에서는 ID3D11Buffer 인터페이스가 대표적이다

	// 1. 생성할 버퍼를 서술하는 구조체를 채운다
	/// <summary>
	/// D3D11_BUFFER_DESC 구조체
	/// </summary>
	/// <param name="ByteWidth">생성할 버텍스 버퍼의 바이트 단위의 크기</param>
	/// <param name="Usage">버퍼가 사용되는 방식(D3D11_USAGE 열거형)</param>
	/// <param name="BindFlags">버퍼가 파이프라인에 바인딩되는 방법을 설정</param>
	/// <param name="CPUAccessFlags">CPU가 버퍼에 접근하는 방식에 대한 플래그 지정</param>
	/// <param name="MiscFlags">기타 플래그(버텍스 버퍼에는 설정할 필요 없음)</param>
	/// <param name="StructureByteStride">구조적 버퍼에 저장된 원소 하나의 크기(바이트)
	///		구조적 버퍼: 같은 크기의 원소을 담는 버퍼/// </param>
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = vertexStructSize * vertexCount;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;

	// 2. 버퍼의 초기화에 사용할 데이터를 서술하는 D3D11_SUBRESOURCE_DATA 구조체를 채운다
	/// <summary>
	/// D3D11_SUBRESOURCE_DATA 구조체
	/// </summary>
	/// <param name="pSysMem">버텍스 버퍼를 초기화할 데이터를 담은 시스템 메모리 배열에 대한 포인터</param>
	/// <param name="SysMemPitch">버텍스 버퍼에는 불필요</param>
	/// <param name="SysMemSlicePitch">버텍스 버퍼에는 불필요</param>
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = pFirstVertex;

	// 3. ID3D11Device::CreateBuffer 함수로 버퍼를 생성한다
	HR(device->GetDevice()->CreateBuffer(&vbd, &vinitData, &m_VB));
}
