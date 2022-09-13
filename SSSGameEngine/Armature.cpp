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

	// 본 매트릭스를 업데이트
	for (int i = 0; i < m_BoneList.size(); i++)
	{
		std::shared_ptr<Transform> boneTransform = m_BoneList[i]->m_BoneObject->GetComponent<Transform>();

		boneWorldTM = boneTransform->GetWorldTM();
		boneNodeTM = m_BoneList[i]->m_BoneNodeTM;
		boneOffsetTM = boneNodeTM * m_SkinNodeTM.Invert();

		// 스킨은 본이 이동한 만큼 + 가중치에 따라서 이동한다
		// + 쉐이더에서 월드 행렬 곱해지는 것 막기 위해서 그냥 인버스 곱함
		// 행렬 한 번 더 곱하는 것 정도는 딱히 그래픽 카드에게 무례한 일 시키는 것 아니다.
		// (근데 이 계산은 CPU야 혜성아!! 물론 CPU에게도 딱히 무리한 연산량이 아니라고 봄)
		Matrix worldTM = m_Transform->GetWorldTM();
		Matrix resTM = boneOffsetTM.Invert() * boneWorldTM * worldTM.Invert();

		m_BoneMatrixList[i] = resTM;
		//m_BoneMatrixList.at(i) = resTM;
	}
}
