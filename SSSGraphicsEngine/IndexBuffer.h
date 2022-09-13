/// <summary>
/// 인덱스 버퍼 클래스
/// 인덱스 버퍼를 생성하고 보관한다
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
    class IndexBuffer :
        public BufferBase
    {
    public:
		IndexBuffer();
		virtual ~IndexBuffer() override;

	public:
		void Init(std::shared_ptr<DXDevice> device, int indexCount, void* pFirstElement);

		/// <summary>
		/// 버텍스 버퍼를 생성하는 함수
		/// </summary>
		void CreateBuffer(std::shared_ptr<DXDevice> device, int indexCount, void* pFirstElement);

		// 아직은 필요 없어 보인다
		//ID3D11Buffer* GetBuffer();
		//UINT GetStride();

    private:
		ComPtr<ID3D11Buffer> m_IB;		// 인덱스 버퍼(색인 버퍼)
		UINT m_IndexCount;		// 인덱스 버퍼(색인 버퍼)의 요소의 개수
    };
}