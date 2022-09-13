// ==================== 입력 레이아웃 인터페이스(Input Layout) ====================
// 메모리에 배치된 버텍스 데이터를 그래픽 파이프라인의
// 입력 어셈블러 단계로 공급하는 방법에 대한 정의를 보유
//	커스텀 버텍스의 각 성분이 어떤 용도인지를 Direct3D에 알려주는 수단
#pragma once

namespace GraphicsEngine
{
	class InputLayouts
	{
	public:
		//enum class VertexType
		//{
		//	VertexPos,
		//	VertexPosColor,
		//	VertexPosTex,
		//	VertexPosNormalColor,
		//	VertexPosNormalTex,
		//	VertexPosNormalTangentTex,
		//	VertexBasic
		//};
		static void InitAll(shared_ptr<DXDevice> m_D3dDevice);
		static void FinalizeAll();

		static ID3D11InputLayout* VertexPos;
		static ID3D11InputLayout* VertexPosColor;
		static ID3D11InputLayout* VertexPosTex;
		static ID3D11InputLayout* VertexPosNormalColor;
		static ID3D11InputLayout* VertexPosNormalTex;
		static ID3D11InputLayout* VertexPosNormalTangentTex;
		static ID3D11InputLayout* VertexBasic;
	};
}