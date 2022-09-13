#pragma once

#include "ComponentBase.h"

// 그래픽스 엔진에 커넥트(...)하기 위한 헤더
#ifdef _DX11RENDERER
#include "../SSSGraphicsEngine/IGraphicsEngineBase.h"
#include "../SSSGraphicsEngine/DX11GraphicsEngine.h"
#include "../SSSGraphicsEngine/GraphicsRenderingData.h"
#endif

class DLL_DECLSPEC LightBase
	: public ComponentBase
{
public:
    LightBase(std::weak_ptr<GameObject> _GameObj, std::string ComponentName);
	virtual ~LightBase();
	virtual void Release() override;

protected:
	std::shared_ptr<LightInfo> m_LightInfo;

public:
	inline XMFLOAT4 GetAmbient() { return m_LightInfo->m_Ambient; }
	inline XMFLOAT4 GetDiffuse() { return m_LightInfo->m_Diffuse; }
	inline XMFLOAT4 GetSpecular() { return m_LightInfo->m_Specular; }

	void SetAmbient(XMFLOAT4 ambient) { m_LightInfo->m_Ambient = ambient; }
	void SetDiffuse(XMFLOAT4 diffuse) { m_LightInfo->m_Diffuse = diffuse; }
	void SetSpecular(XMFLOAT4 specular) { m_LightInfo->m_Specular = specular; }

public:
	void Init() override {}
	void FixedUpdate(float deltaTime) override {}
	void Update(float deltaTime) override {}
	void LateUpdate(float deltaTime) override {}
	void Render() override {}
};
