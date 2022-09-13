#include "GraphicsEnginePCH.h"
#include "InputLayouts.h"

#include "Effects.h"
#include "VertexStruct.h"

namespace GraphicsEngine
{
	ID3D11InputLayout* InputLayouts::VertexPos = nullptr;
	ID3D11InputLayout* InputLayouts::VertexPosColor = nullptr;
	ID3D11InputLayout* InputLayouts::VertexPosTex = nullptr;
	ID3D11InputLayout* InputLayouts::VertexPosNormalColor = nullptr;
	ID3D11InputLayout* InputLayouts::VertexPosNormalTex = nullptr;
	ID3D11InputLayout* InputLayouts::VertexPosNormalTangentTex = nullptr;
	ID3D11InputLayout* InputLayouts::VertexBasic = nullptr;

	void InputLayouts::InitAll(shared_ptr<DXDevice> m_D3dDevice)
	{
		/// <summary>
		/// ��ǲ ���̾ƿ��� ������ �ʿ��� ���ؽ� ���̴��� ��ǲ �ñ״��ĸ� ��´�
		/// ����Ʈ ��ü(ID3DX11Effect ��)���� �ϳ� �̻��� ������ �н��� ĸ��ȭ
		/// (������ �н����� ���� ���̴��� �����ȴ�)
		/// --> ���彺 ��ü���� �н��� �����ϴ� ����ü(D3DX11_PASS_DESC)�� ȹ�� ����
		/// --> D3DX11_PASS_DESC ����ü���� ���ؽ� ���̴��� ��ǲ �ñ״�ó�� ���� ����
		/// </summary>

		// PosColor�� ���� ��ǲ ���̾ƿ��� �����Ѵ�
		D3DX11_PASS_DESC passDesc;
		//Effects::PosColorFX->PosColorTech->GetPassByIndex(0)->GetDesc(&passDesc);
		//HR(m_D3dDevice->GetDevice()->CreateInputLayout(
		//	VertexStruct::VertexPosColor::InputLayout, 2,
		//	passDesc.pIAInputSignature,
		//	passDesc.IAInputSignatureSize,
		//	&VertexPosColor)
		//);

		//// VertexPosTex�� ���� ��ǲ ���̾ƿ��� �����Ѵ�
		//ZeroMemory(&passDesc, sizeof(D3DX11_PASS_DESC));
		//Effects::PosTexFX->PosTexTech->GetPassByIndex(0)->GetDesc(&passDesc);
		//HR(m_D3dDevice->CreateInputLayout(
		//	Vertex::VertexPosTex::InputLayout, 2,
		//	passDesc.pIAInputSignature,
		//	passDesc.IAInputSignatureSize,
		//	&VertexPosTex)
		//);

		//// VertexPosNormalTex�� ���� ��ǲ ���̾ƿ��� �����Ѵ�
		//ZeroMemory(&passDesc, sizeof(D3DX11_PASS_DESC));
		//Effects::PosNormalTexFX->PosNormalTexTech->GetPassByIndex(0)->GetDesc(&passDesc);
		//HR(m_D3dDevice->CreateInputLayout(
		//	Vertex::VertexPosNormalTex::InputLayout, 3,
		//	passDesc.pIAInputSignature,
		//	passDesc.IAInputSignatureSize,
		//	&VertexPosNormalTex)
		//);


		// todo: ����Ʈ ������ �þ�� �߰��� ��ǲ ���̾ƿ��� �����Ѵ�
		// ...
		ZeroMemory(&passDesc, sizeof(D3DX11_PASS_DESC));
	}

	void InputLayouts::FinalizeAll()
	{
		ReleaseCOM(VertexPos);
		ReleaseCOM(VertexPosColor);
		ReleaseCOM(VertexPosTex);
		ReleaseCOM(VertexPosNormalColor);
		ReleaseCOM(VertexPosNormalTex);
		ReleaseCOM(VertexPosNormalTangentTex);
		ReleaseCOM(VertexBasic);
	}
}