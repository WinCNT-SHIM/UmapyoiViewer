#include "GraphicsEnginePCH.h"

#include "Effects.h"

namespace GraphicsEngine
{
#pragma region Effect
	Effect::Effect(shared_ptr<DXDevice> device, string filename)
		: m_FX(nullptr)
	{
		std::ifstream m_FXFile(filename, std::ios::binary);

		assert(m_FXFile);

		m_FXFile.seekg(0, std::ios_base::end);
		int size = (int)m_FXFile.tellg();
		m_FXFile.seekg(0, std::ios_base::beg);
		std::vector<char> compiledShader(size);

		m_FXFile.read(&compiledShader[0], size);
		m_FXFile.close();

		/// <summary>
		/// �о�� �����ϵ� ����Ʈ ���Ͽ���
		/// D3DX11CreateEffectFromMemory �Լ��� ����Ʈ ��ü�� ����
		/// </summary>
		HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size, 0, device->GetDevice().Get(), &m_FX));
	}

	Effect::~Effect()
	{
		ReleaseCOM(m_FX);
	}
#pragma endregion

#pragma region EffectImpl
#pragma region PosColorEffect
//PosColorEffect::PosColorEffect(shared_ptr<DXDevice> device, string m_FilePath) : Effect(device, m_FilePath)
//	, PosColorTech(nullptr)
//	, WorldViewProj(nullptr)
//{
//	// ����Ʈ���� PosColorTech��� ��ũ�п� ���� �����͸� ȹ��
//	PosColorTech = m_FX->GetTechniqueByName("PosColorTech");
//	// ����Ʈ�� ��� ���ۿ� ���� ������ ȹ��
//	WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
//}
#pragma endregion PosColorEffect

#pragma region WireEffect
	WireEffect::WireEffect(shared_ptr<DXDevice> device, string m_FilePath) : Effect(device, m_FilePath)
	{
		// ��� ����(���)
		WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
		// ��� ����(����)
		WireColor = m_FX->GetVariableByName("gWireColor")->AsVector();
		// ��ũ��
		ColorTech = m_FX->GetTechniqueByName("ColorTech");
	}
#pragma endregion WireEffect
#pragma endregion EffectImpl


#pragma region Effects
	//BasicEffect* Effects::BasicFX = 0;
	//LightTexEffect* Effects::BasicTexFX = 0;
	// ==================== Post Processing �߰� �ڵ� ====================
	//PPEffect* Effects::PPFX = 0;
	// ==================== Collision Detection �߰� �ڵ� ====================
	
	// ==================== static ������ �ʱ�ȭ ====================
	//PosColorEffect* Effects::PosColorFX = nullptr;
	WireEffect* Effects::WireFX = nullptr;
	void Effects::InitAll(shared_ptr<DXDevice> device)
	{
		// ���� ���丮�� Ȯ���ϱ� ���� �Լ�
		//char path[128];
		//GetCurrentDirectoryA(128, path);

		//PosColorFX = new PosColorEffect(device, "FX/PosColor.cso");
		//WireFX = new WireEffect(device, "../Debug/wire.cso");
	}

	void Effects::FinalizeAll()
	{
		//SafeDelete(PosColorFX);
		SafeDelete(WireFX);
	}
#pragma endregion Effects
}