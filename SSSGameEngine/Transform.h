/// <summary>
/// DX용 트랜스폼 컴포넌트
/// 
/// Created : 2022/08/02 심승수
/// Updated : 2022/08/02 심승수
/// </summary>
#pragma once

using namespace DirectX;	// XMFLOAT3, XMVECTOR. etc.

enum class Space
{
	WORLD,
	LOCAL
};

class DLL_DECLSPEC Transform
	: public ComponentBase
{
public:
	Transform(std::weak_ptr<GameObject> _GameObj);
	virtual ~Transform();
	virtual void Release() override;

private:
	// 로컬
	XMMATRIX m_LocalTM;
	XMFLOAT3 m_LocalPosition;
	XMFLOAT3 m_LocalRotation;
	XMFLOAT3 m_LocalScale;

	// 월드
	XMMATRIX m_WorldTM;
	XMFLOAT3 m_WorldPosition;
	XMFLOAT3 m_WorldRotation;
	XMFLOAT3 m_WorldScale;

	// 중점기준으로, 앞, 위 오른쪽 벡터
	XMFLOAT3 m_Look;
	XMFLOAT3 m_Up;
	XMFLOAT3 m_Right;

	// 쿼터니언 회전에 대한 변수
	DirectX::SimpleMath::Quaternion m_WorldRotationQuat;

	// TM 업데이트를 위한 각종 변수 및 함수
	bool m_NeedTMUpdate;
	// 로컬 TM의 변경
	void UpdateLocalTM();
	// WorldTM의 변경
	void UpdateWorldTM();
	// needTM UPdate 변수 True로 변경
	void FlagNeedTMUpdate();

public:
	// 포지션, 로테이션, 스케일 Setter
	void SetPosition(const XMFLOAT3& _pos, Space _relativeTo = Space::LOCAL);
	void SetPosition(const XMVECTOR& _pos, Space _relativeTo = Space::LOCAL);
	void SetRotation(const XMFLOAT3& _eulerAngle, Space _relativeTo = Space::LOCAL);
	void SetRotation(const XMVECTOR& _eulerAngle, Space _relativeTo = Space::LOCAL);
	void SetRotationLocalQuat(const SimpleMath::Quaternion _quat);
	void SetScale(const XMFLOAT3& _localScale);  /// Scale은 월드 없음
	void SetScale(const XMVECTOR& _localScale);  /// Scale은 월드 없음

	void SetLocalTM(const XMMATRIX& _localTM);
	void SetWorldTM(const XMMATRIX& _worldTM);

	// 각각의 Private 변수 게터.
	inline const XMFLOAT3& GetLocalPosition() { return m_LocalPosition; }
	inline const XMFLOAT3& GetLocalRotation() { return m_LocalRotation; }
	inline const XMFLOAT3& GetLocalScale() { return m_LocalScale; }

	inline const XMFLOAT3& GetWorldPosition() { return m_WorldPosition; }
	inline const XMFLOAT3& GetWorldRotation() { return m_WorldRotation; }
	inline const XMFLOAT3& GetWorldScale() { return m_WorldScale; }

	inline const XMFLOAT3& GetLook() { return m_Look; }
	inline const XMFLOAT3& GetUp() { return m_Up; }
	inline const XMFLOAT3& GetRight() { return m_Right; }

	const XMMATRIX& GetLocalTM() { return m_LocalTM; }
	const XMMATRIX& GetWorldTM();

	// 쿼터니언 => 오일러 각 변환은 따로 API가 없나 보다...
	// https://stackoverflow.com/questions/30540819/is-there-a-way-to-convert-a-quaternion-to-angles
public:
	static XMFLOAT3 QuaternionToEuler(SimpleMath::Quaternion quat);
	static XMFLOAT3 QuaternionToEuler(XMVECTOR& _quat);

public:
	virtual void Init();
	void FixedUpdate(float deltaTime) override {}
	void Update(float deltaTime) override;
	void LateUpdate(float deltaTime) override {}
	virtual void Render();
};

