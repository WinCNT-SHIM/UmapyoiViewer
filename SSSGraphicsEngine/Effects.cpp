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
		/// 읽어온 컴파일된 이펙트 파일에서
		/// D3DX11CreateEffectFromMemory 함수로 이펙트 객체를 생성
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
//	// 이펙트에서 PosColorTech라는 테크닉에 대한 포인터를 획득
//	PosColorTech = m_FX->GetTechniqueByName("PosColorTech");
//	// 이펙트의 상수 버퍼에 대한 포인터 획득
//	WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
//}
#pragma endregion PosColorEffect

#pragma region WireEffect
	WireEffect::WireEffect(shared_ptr<DXDevice> device, string m_FilePath) : Effect(device, m_FilePath)
	{
		// 상수 버퍼(행렬)
		WorldViewProj = m_FX->GetVariableByName("gWorldViewProj")->AsMatrix();
		// 상수 버퍼(벡터)
		WireColor = m_FX->GetVariableByName("gWireColor")->AsVector();
		// 테크닉
		ColorTech = m_FX->GetTechniqueByName("ColorTech");
	}
#pragma endregion WireEffect
#pragma endregion EffectImpl


#pragma region Effects
	//BasicEffect* Effects::BasicFX = 0;
	//LightTexEffect* Effects::BasicTexFX = 0;
	// ==================== Post Processing 추가 코드 ====================
	//PPEffect* Effects::PPFX = 0;
	// ==================== Collision Detection 추가 코드 ====================
	
	// ==================== static 변수의 초기화 ====================
	//PosColorEffect* Effects::PosColorFX = nullptr;
	WireEffect* Effects::WireFX = nullptr;
	void Effects::InitAll(shared_ptr<DXDevice> device)
	{
		// 현재 디렉토리를 확인하기 위한 함수
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