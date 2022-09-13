/// <summary>
/// 버텍스 버퍼 클래스
/// 버텍스 버퍼를 생성하고 보관한다
/// 
/// Created : 2022/07/06 심승수
/// Updated : 2022/07/10 심승수
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
		/// 버텍스 버퍼를 생성하는 함수
		/// </summary>
		void CreateBuffer(std::shared_ptr<DXDevice> device, int vertexStructSize, int vertexCount, void* pFirstVertex);

		// 아직은 필요 없어 보인다
		//ID3D11Buffer* GetBuffer();
		//UINT GetStride();

	private:
		ComPtr<ID3D11Buffer> m_VB;	// 버텍스 버퍼(정점 버퍼)
		UINT m_VertextCount;	// 버텍스 버퍼(색인 버퍼)의 요소의 개수
		UINT m_Stride;
		UINT m_Offset = 0;
    };
}