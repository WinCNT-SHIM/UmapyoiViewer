#include "GraphicsEnginePCH.h"
#include "EffectManager.h"

#include "EffectBase.h"
#include "ConstantBuffer.h"
#include "PosColorEffect.h"
#include "BasicEffect.h"
#include "BasicSkinnedEffect.h"
// UmaMusume
#include "UmaMusumeHairEffect.h"
#include "UmaMusumeFaceEffect.h"
#include "UmaMusumeCheekEffect.h"
#include "UmaMusumeEyesEffect.h"

using namespace GraphicsEngine;

std::shared_ptr<GraphicsEngine::EffectManager> GraphicsEngine::EffectManager::m_Instance = nullptr;

std::shared_ptr<GraphicsEngine::EffectBase> EffectManager::GetEffect(const tstring& name)
{
	return m_Effects.find(name) != m_Effects.end() ? m_Effects[name] : nullptr;
}

void EffectManager::Init(std::shared_ptr<DXDevice> device)
//void EffectManager::Init(ComPtr<ID3D11Device> device)
{
	// todo: 팩토리의 역할 같은데...? 뭐 됐나
	//////////////////////////////////////////////////////////////////////////
	// ConstantBuffer
	//////////////////////////////////////////////////////////////////////////
	// Per Frame 상수 버퍼는 특수한 처리를 한다
	std::shared_ptr<ConstantBuffer> constantBuffer = std::make_shared<ConstantBuffer>(device, "FX/CBPerFrame.cso");
	m_Effects.emplace(constantBuffer->GetName(), constantBuffer);


	//////////////////////////////////////////////////////////////////////////
	// PosColor
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> posColor = std::make_shared<PosColorEffect>(device, "FX/PosColor.cso");
	// Per Frame 상수 버퍼 설정
	posColor->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(posColor->GetName(), posColor);


	//////////////////////////////////////////////////////////////////////////
	// Basic
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> basic = std::make_shared<BasicEffect>(device, "FX/Basic.cso");
	// Per Frame 상수 버퍼 설정
	basic->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(basic->GetName(), basic);


	//////////////////////////////////////////////////////////////////////////
	// BasicSkinned
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> basicSkinned = std::make_shared<BasicSkinnedEffect>(device, "FX/BasicSkinned.cso");
	// Per Frame 상수 버퍼 설정
	basicSkinned->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(basicSkinned->GetName(), basicSkinned);


	//////////////////////////////////////////////////////////////////////////
	// UmaMusumeHair
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> umaMusumeHiar = std::make_shared<UmaMusumeHairEffect>(device, "FX/UmaMusumeHair.cso");
	// Per Frame 상수 버퍼 설정
	umaMusumeHiar->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(umaMusumeHiar->GetName(), umaMusumeHiar);


	//////////////////////////////////////////////////////////////////////////
	// UmaMusumeFace
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> umaMusumeFace = std::make_shared<UmaMusumeFaceEffect>(device, "FX/UmaMusumeFace.cso");
	// Per Frame 상수 버퍼 설정
	umaMusumeFace->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(umaMusumeFace->GetName(), umaMusumeFace);


	//////////////////////////////////////////////////////////////////////////
	// UmaMusumeCheek
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> umaMusumeCheek = std::make_shared<UmaMusumeCheekEffect>(device, "FX/UmaMusumeCheek.cso");
	// Per Frame 상수 버퍼 설정
	umaMusumeCheek->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(umaMusumeCheek->GetName(), umaMusumeCheek);


	//////////////////////////////////////////////////////////////////////////
	// UmaMusumeEyes
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> umaMusumeEyes = std::make_shared<UmaMusumeEyesEffect>(device, "FX/UmaMusumeEyes.cso");
	// Per Frame 상수 버퍼 설정
	umaMusumeEyes->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(umaMusumeEyes->GetName(), umaMusumeEyes);



	// 쉐이더가 늘어날 때마다 추가
	// ...
	// ...
	// ...
	// ...
}

void EffectManager::Release()
{
	for (auto& effect : m_Effects)
		if (effect.second != nullptr)
			effect.second->Release();
	m_Effects.clear();
}
