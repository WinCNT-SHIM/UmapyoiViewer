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
	// todo: ���丮�� ���� ������...? �� �Ƴ�
	//////////////////////////////////////////////////////////////////////////
	// ConstantBuffer
	//////////////////////////////////////////////////////////////////////////
	// Per Frame ��� ���۴� Ư���� ó���� �Ѵ�
	std::shared_ptr<ConstantBuffer> constantBuffer = std::make_shared<ConstantBuffer>(device, "FX/CBPerFrame.cso");
	m_Effects.emplace(constantBuffer->GetName(), constantBuffer);


	//////////////////////////////////////////////////////////////////////////
	// PosColor
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> posColor = std::make_shared<PosColorEffect>(device, "FX/PosColor.cso");
	// Per Frame ��� ���� ����
	posColor->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(posColor->GetName(), posColor);


	//////////////////////////////////////////////////////////////////////////
	// Basic
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> basic = std::make_shared<BasicEffect>(device, "FX/Basic.cso");
	// Per Frame ��� ���� ����
	basic->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(basic->GetName(), basic);


	//////////////////////////////////////////////////////////////////////////
	// BasicSkinned
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> basicSkinned = std::make_shared<BasicSkinnedEffect>(device, "FX/BasicSkinned.cso");
	// Per Frame ��� ���� ����
	basicSkinned->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(basicSkinned->GetName(), basicSkinned);


	//////////////////////////////////////////////////////////////////////////
	// UmaMusumeHair
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> umaMusumeHiar = std::make_shared<UmaMusumeHairEffect>(device, "FX/UmaMusumeHair.cso");
	// Per Frame ��� ���� ����
	umaMusumeHiar->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(umaMusumeHiar->GetName(), umaMusumeHiar);


	//////////////////////////////////////////////////////////////////////////
	// UmaMusumeFace
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> umaMusumeFace = std::make_shared<UmaMusumeFaceEffect>(device, "FX/UmaMusumeFace.cso");
	// Per Frame ��� ���� ����
	umaMusumeFace->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(umaMusumeFace->GetName(), umaMusumeFace);


	//////////////////////////////////////////////////////////////////////////
	// UmaMusumeCheek
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> umaMusumeCheek = std::make_shared<UmaMusumeCheekEffect>(device, "FX/UmaMusumeCheek.cso");
	// Per Frame ��� ���� ����
	umaMusumeCheek->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(umaMusumeCheek->GetName(), umaMusumeCheek);


	//////////////////////////////////////////////////////////////////////////
	// UmaMusumeEyes
	//////////////////////////////////////////////////////////////////////////
	std::shared_ptr<EffectBase> umaMusumeEyes = std::make_shared<UmaMusumeEyesEffect>(device, "FX/UmaMusumeEyes.cso");
	// Per Frame ��� ���� ����
	umaMusumeEyes->SetCBPerFrame(constantBuffer->GetCBPerFrame());
	m_Effects.emplace(umaMusumeEyes->GetName(), umaMusumeEyes);



	// ���̴��� �þ ������ �߰�
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
