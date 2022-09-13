#include "GameEnginePCH.h"
#include "Armature.h"

#include "Transform.h"
#include "MeshRenderer.h"

Armature::Armature(std::weak_ptr<GameObject> gameObj)
	: ComponentBase(gameObj, "Armature")
	, m_SkinNodeTM(Matrix::Identity)
	, m_BoneList(std::vector<std::shared_ptr<Bone>>())
	, m_BoneCount(0)
{
	m_Transform = GetGameObject()->GetComponent<Transform>();
}

Armature::~Armature()
{

}

void Armature::Init(const Matrix& skinNodeTM, const std::vector<std::shared_ptr<Bone>>& boneList)
{
	m_SkinNodeTM = skinNodeTM;
	m_BoneList = boneList;

	m_BoneMatrixList.resize(m_BoneList.size());
	m_BoneCount = static_cast<uint32>(m_BoneList.size());

	for (int i = 0; i < m_BoneList.size(); i++)
	{
		std::shared_ptr<GameObject> boneGO = m_BoneList[i]->m_BoneObject;
		boneGO->GetComponent<MeshRenderer>()->SetIsEnable(false);
	}
}

void Armature::Release()
{
	m_Transform.reset();

	for (auto& bone : m_BoneList)
		bone.reset();

	__super::Release();
}

void Armature::LateUpdate(float deltaTime)
{
	Matrix boneWorldTM;
	Matrix boneOffsetTM;
	Matrix boneNodeTM;
	Matrix finalBoneTM;

	// �� ��Ʈ������ ������Ʈ
	for (int i = 0; i < m_BoneList.size(); i++)
	{
		std::shared_ptr<Transform> boneTransform = m_BoneList[i]->m_BoneObject->GetComponent<Transform>();

		boneWorldTM = boneTransform->GetWorldTM();
		boneNodeTM = m_BoneList[i]->m_BoneNodeTM;
		boneOffsetTM = boneNodeTM * m_SkinNodeTM.Invert();

		// ��Ų�� ���� �̵��� ��ŭ + ����ġ�� ���� �̵��Ѵ�
		// + ���̴����� ���� ��� �������� �� ���� ���ؼ� �׳� �ι��� ����
		// ��� �� �� �� ���ϴ� �� ������ ���� �׷��� ī�忡�� ������ �� ��Ű�� �� �ƴϴ�.
		// (�ٵ� �� ����� CPU�� ������!! ���� CPU���Ե� ���� ������ ���귮�� �ƴ϶�� ��)
		Matrix worldTM = m_Transform->GetWorldTM();
		Matrix resTM = boneOffsetTM.Invert() * boneWorldTM * worldTM.Invert();

		m_BoneMatrixList[i] = resTM;
		//m_BoneMatrixList.at(i) = resTM;
	}
}
