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
		/// 인풋 레이아웃을 생성에 필요한 버텍스 셰이더의 인풋 시그니쳐를 얻는다
		/// 이펙트 객체(ID3DX11Effect 등)에는 하나 이상의 렌더링 패스을 캡술화
		/// (렌더링 패스마다 정점 셰이더가 연관된다)
		/// --> 이펙스 객체에서 패스를 서술하는 구조체(D3DX11_PASS_DESC)을 획득 가능
		/// --> D3DX11_PASS_DESC 구조체에서 버텍스 셰이더의 인풋 시그니처에 접근 가능
		/// </summary>

		// PosColor에 대한 인풋 레이아웃을 생성한다
		D3DX11_PASS_DESC passDesc;
		//Effects::PosColorFX->PosColorTech->GetPassByIndex(0)->GetDesc(&passDesc);
		//HR(m_D3dDevice->GetDevice()->CreateInputLayout(
		//	VertexStruct::VertexPosColor::InputLayout, 2,
		//	passDesc.pIAInputSignature,
		//	passDesc.IAInputSignatureSize,
		//	&VertexPosColor)
		//);

		//// VertexPosTex에 대한 인풋 레이아웃을 생성한다
		//ZeroMemory(&passDesc, sizeof(D3DX11_PASS_DESC));
		//Effects::PosTexFX->PosTexTech->GetPassByIndex(0)->GetDesc(&passDesc);
		//HR(m_D3dDevice->CreateInputLayout(
		//	Vertex::VertexPosTex::InputLayout, 2,
		//	passDesc.pIAInputSignature,
		//	passDesc.IAInputSignatureSize,
		//	&VertexPosTex)
		//);

		//// VertexPosNormalTex에 대한 인풋 레이아웃을 생성한다
		//ZeroMemory(&passDesc, sizeof(D3DX11_PASS_DESC));
		//Effects::PosNormalTexFX->PosNormalTexTech->GetPassByIndex(0)->GetDesc(&passDesc);
		//HR(m_D3dDevice->CreateInputLayout(
		//	Vertex::VertexPosNormalTex::InputLayout, 3,
		//	passDesc.pIAInputSignature,
		//	passDesc.IAInputSignatureSize,
		//	&VertexPosNormalTex)
		//);


		// todo: 이펙트 파일이 늘어나면 추가로 인풋 레이아웃을 생성한다
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