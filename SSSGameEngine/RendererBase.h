/// <summary>
/// 렌더러 컴포넌트의 기본 클래스
/// 게임 엔진의 렌더러의 기본 클래스로
/// 그래픽스 엔진에 그리기 처리를 위한 정보를 전하는 클래스를
/// 구현하기 위한 기본 클래스이다
/// 
/// Created : 2022/08/04 심승수
/// Updated : 2022/08/04 심승수
/// </summary>
#pragma once

#include "ComponentBase.h"
#include "../SSSGraphicsEngine/GraphicsRenderingData.h"

class GameObject;
class Transform;

class DLL_DECLSPEC RendererBase
	: public ComponentBase
	, public std::enable_shared_from_this<RendererBase>
{
public:
	RendererBase(std::weak_ptr<GameObject> _GameObj, std::string _CmpName);
	virtual ~RendererBase();
	virtual void Release() override;

protected:
	// Transform 캐싱
	std::weak_ptr<Transform> m_Transform;

public:
	// 이 컴포넌트를 가진 게임 오브젝트를 그릴 때 필요한 정보들
	// (오브젝트, 머테리얼, 쉐이더, 지오메트리, 애니메이션, 라이트, 텍스트 등등등)
	// 게임 엔진에서는 이것을 조작한다
	// 그래픽스 엔진 프로젝트에 있어서 종속성이 신경쓰이긴 하지만
	// 혹시라도 그래픽스 엔진을 교체한다고 하면 해당 헤더를 인터페이스로 빼야할 것이다...는 그럴 일 없겠지
	std::shared_ptr<RenderingData> m_RenderingData;

//protected:
//	// Transform 캐싱
//	std::shared_ptr<Transform> _transform;

public:
	virtual void Init() override;
	void FixedUpdate(float deltaTime) override {}
	virtual void Update(float deltaTime) override {}
	void LateUpdate(float deltaTime) override {}
	virtual void Render() override {}
};

