/// <summary>
/// 메쉬 렌더러 컴포넌트 클래스
/// 그리기를 위한 제일 기본적인  컴포넌트
/// 
/// Created : 2022/08/04 심승수
/// Updated : 2022/08/04 심승수
/// </summary>
#pragma once

#include "RendererBase.h"

class DLL_DECLSPEC MeshRenderer
    : public RendererBase
{
public:
	MeshRenderer(std::weak_ptr<GameObject> _GameObj);
	virtual ~MeshRenderer();
	virtual void Release() override;

public:
	virtual void Init() override { RendererBase::Init(); }
	virtual void Render() override;
};

