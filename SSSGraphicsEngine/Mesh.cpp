#include "GraphicsEnginePCH.h"
#include "Mesh.h"

//#include "Renderer.h"

namespace GraphicsEngine
{
	void Mesh::Render()
	{
		ComPtr<ID3D11DeviceContext> deviceContext = Renderer::GetInstance()->GetDeviceContext();

		deviceContext->IASetVertexBuffers(0, 1, m_VertexBuffer.GetAddressOf(), &m_Stride, &m_Offset);
		deviceContext->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		deviceContext->IASetPrimitiveTopology(m_Topology);
		deviceContext->DrawIndexed(m_IndexBufferSize, 0, 0);
	}

	void Mesh::Release()
	{
		m_VertexBuffer.ReleaseAndGetAddressOf();
		m_IndexBuffer.ReleaseAndGetAddressOf();
	}
}