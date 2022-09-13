#pragma once

#include "ComponentBase.h"

using namespace DirectX::SimpleMath;
class Transform;

struct Bone
{
	~Bone() { m_BoneObject.reset(); }

	uint32 m_Index;
	std::string m_Name;
	Matrix m_BoneNodeTM;
	std::shared_ptr<GameObject> m_BoneObject;
};

class Armature
	: public ComponentBase
{
public:
	Armature(std::weak_ptr<GameObject> gameObj);
	virtual ~Armature();

	void Init(
		const Matrix& skinNodeTM
		, const std::vector<std::shared_ptr<::Bone>>& boneList);
	virtual void Release() override;

private:
	std::shared_ptr<Transform> m_Transform;
	Matrix m_SkinNodeTM;
	std::vector<std::shared_ptr<::Bone>> m_BoneList;
	//std::vector<Matrix> m_BoneMatrixList;
	std::vector<DirectX::XMMATRIX> m_BoneMatrixList;
	uint32 m_BoneCount;

public:
	inline const uint32 GetBoneCount() const { return m_BoneCount; }
	inline const Matrix& GetNodeTM() const { return m_SkinNodeTM; }
	//const Matrix* GetBoneMatrixListPointer() const { return m_BoneMatrixList.data(); }
	//const std::vector<Matrix>& GetBoneMatrixList() const { return m_BoneMatrixList; }
	const DirectX::XMMATRIX* GetBoneMatrixListPointer() const { return m_BoneMatrixList.data(); }
	std::vector<DirectX::XMMATRIX>& GetBoneMatrixList() { return m_BoneMatrixList; }

public:
	virtual void Init() override {}
	void FixedUpdate(float deltaTime) override {}
	virtual void Update(float deltaTime) override {}
	void LateUpdate(float deltaTime) override;
	virtual void Render() override {}
};

