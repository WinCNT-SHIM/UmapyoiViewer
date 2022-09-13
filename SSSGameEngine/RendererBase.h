/// <summary>
/// ������ ������Ʈ�� �⺻ Ŭ����
/// ���� ������ �������� �⺻ Ŭ������
/// �׷��Ƚ� ������ �׸��� ó���� ���� ������ ���ϴ� Ŭ������
/// �����ϱ� ���� �⺻ Ŭ�����̴�
/// 
/// Created : 2022/08/04 �ɽ¼�
/// Updated : 2022/08/04 �ɽ¼�
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
	// Transform ĳ��
	std::weak_ptr<Transform> m_Transform;

public:
	// �� ������Ʈ�� ���� ���� ������Ʈ�� �׸� �� �ʿ��� ������
	// (������Ʈ, ���׸���, ���̴�, ������Ʈ��, �ִϸ��̼�, ����Ʈ, �ؽ�Ʈ ����)
	// ���� ���������� �̰��� �����Ѵ�
	// �׷��Ƚ� ���� ������Ʈ�� �־ ���Ӽ��� �Ű澲�̱� ������
	// Ȥ�ö� �׷��Ƚ� ������ ��ü�Ѵٰ� �ϸ� �ش� ����� �������̽��� ������ ���̴�...�� �׷� �� ������
	std::shared_ptr<RenderingData> m_RenderingData;

//protected:
//	// Transform ĳ��
//	std::shared_ptr<Transform> _transform;

public:
	virtual void Init() override;
	void FixedUpdate(float deltaTime) override {}
	virtual void Update(float deltaTime) override {}
	void LateUpdate(float deltaTime) override {}
	virtual void Render() override {}
};

