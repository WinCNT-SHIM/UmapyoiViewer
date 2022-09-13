/// <summary>
/// ���ؽ� ���� Ŭ����
/// ���ؽ� ���۸� �����ϰ� �����Ѵ�
/// 
/// Created : 2022/07/06 �ɽ¼�
/// Updated : 2022/07/10 �ɽ¼�
/// </summary>
#pragma once
#include "BufferBase.h"

#define  _DX11RENDERER
#include "IDefine.h"

namespace GraphicsEngine
{
    class VertexBuffer :
        public BufferBase
	{
	public:
		VertexBuffer();
		virtual ~VertexBuffer() override;

	public:
		void Init(std::shared_ptr<DXDevice> device, int vertexStructSize, int vertexCount, void* pFirstVertex);

		/// <summary>
		/// ���ؽ� ���۸� �����ϴ� �Լ�
		/// </summary>
		void CreateBuffer(std::shared_ptr<DXDevice> device, int vertexStructSize, int vertexCount, void* pFirstVertex);

		// ������ �ʿ� ���� ���δ�
		//ID3D11Buffer* GetBuffer();
		//UINT GetStride();

	private:
		ComPtr<ID3D11Buffer> m_VB;	// ���ؽ� ����(���� ����)
		UINT m_VertextCount;	// ���ؽ� ����(���� ����)�� ����� ����
		UINT m_Stride;
		UINT m_Offset = 0;
    };
}