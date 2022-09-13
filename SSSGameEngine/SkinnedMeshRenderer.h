#pragma once

#include "RendererBase.h"

class DLL_DECLSPEC SkinnedMeshRenderer
	: public RendererBase
{
public:
	SkinnedMeshRenderer(std::weak_ptr<GameObject> _GameObj);
	virtual ~SkinnedMeshRenderer();
	virtual void Release() override;

public:
	virtual void Init() override { RendererBase::Init(); }
	virtual void Render() override;
};

