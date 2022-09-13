/// <summary>
/// �޽� ������ ������Ʈ Ŭ����
/// �׸��⸦ ���� ���� �⺻����  ������Ʈ
/// 
/// Created : 2022/08/04 �ɽ¼�
/// Updated : 2022/08/04 �ɽ¼�
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

