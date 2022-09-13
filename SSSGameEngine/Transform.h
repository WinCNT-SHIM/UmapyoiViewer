/// <summary>
/// DX�� Ʈ������ ������Ʈ
/// 
/// Created : 2022/08/02 �ɽ¼�
/// Updated : 2022/08/02 �ɽ¼�
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
	// ����
	XMMATRIX m_LocalTM;
	XMFLOAT3 m_LocalPosition;
	XMFLOAT3 m_LocalRotation;
	XMFLOAT3 m_LocalScale;

	// ����
	XMMATRIX m_WorldTM;
	XMFLOAT3 m_WorldPosition;
	XMFLOAT3 m_WorldRotation;
	XMFLOAT3 m_WorldScale;

	// ������������, ��, �� ������ ����
	XMFLOAT3 m_Look;
	XMFLOAT3 m_Up;
	XMFLOAT3 m_Right;

	// ���ʹϾ� ȸ���� ���� ����
	DirectX::SimpleMath::Quaternion m_WorldRotationQuat;

	// TM ������Ʈ�� ���� ���� ���� �� �Լ�
	bool m_NeedTMUpdate;
	// ���� TM�� ����
	void UpdateLocalTM();
	// WorldTM�� ����
	void UpdateWorldTM();
	// needTM UPdate ���� True�� ����
	void FlagNeedTMUpdate();

public:
	// ������, �����̼�, ������ Setter
	void SetPosition(const XMFLOAT3& _pos, Space _relativeTo = Space::LOCAL);
	void SetPosition(const XMVECTOR& _pos, Space _relativeTo = Space::LOCAL);
	void SetRotation(const XMFLOAT3& _eulerAngle, Space _relativeTo = Space::LOCAL);
	void SetRotation(const XMVECTOR& _eulerAngle, Space _relativeTo = Space::LOCAL);
	void SetRotationLocalQuat(const SimpleMath::Quaternion _quat);
	void SetScale(const XMFLOAT3& _localScale);  /// Scale�� ���� ����
	void SetScale(const XMVECTOR& _localScale);  /// Scale�� ���� ����

	void SetLocalTM(const XMMATRIX& _localTM);
	void SetWorldTM(const XMMATRIX& _worldTM);

	// ������ Private ���� ����.
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

	// ���ʹϾ� => ���Ϸ� �� ��ȯ�� ���� API�� ���� ����...
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

