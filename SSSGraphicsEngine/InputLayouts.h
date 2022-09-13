// ==================== �Է� ���̾ƿ� �������̽�(Input Layout) ====================
// �޸𸮿� ��ġ�� ���ؽ� �����͸� �׷��� ������������
// �Է� ����� �ܰ�� �����ϴ� ����� ���� ���Ǹ� ����
//	Ŀ���� ���ؽ��� �� ������ � �뵵������ Direct3D�� �˷��ִ� ����
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