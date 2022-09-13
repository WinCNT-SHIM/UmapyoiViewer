/// <summary>
/// �ε��� ���� Ŭ����
/// �ε��� ���۸� �����ϰ� �����Ѵ�
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
    class IndexBuffer :
        public BufferBase
    {
    public:
		IndexBuffer();
		virtual ~IndexBuffer() override;

	public:
		void Init(std::shared_ptr<DXDevice> device, int indexCount, void* pFirstElement);

		/// <summary>
		/// ���ؽ� ���۸� �����ϴ� �Լ�
		/// </summary>
		void CreateBuffer(std::shared_ptr<DXDevice> device, int indexCount, void* pFirstElement);

		// ������ �ʿ� ���� ���δ�
		//ID3D11Buffer* GetBuffer();
		//UINT GetStride();

    private:
		ComPtr<ID3D11Buffer> m_IB;		// �ε��� ����(���� ����)
		UINT m_IndexCount;		// �ε��� ����(���� ����)�� ����� ����
    };
}