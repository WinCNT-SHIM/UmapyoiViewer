#include "GameEnginePCH.h"
#include "Transform.h"

Transform::Transform(std::weak_ptr<GameObject> _GameObj) :
	ComponentBase(_GameObj, "Transform")
	, m_LocalTM{ XMMatrixIdentity() }
	, m_WorldTM{ XMMatrixIdentity() }
	, m_LocalPosition{ 0.0f, 0.0f, 0.0f }
	, m_LocalRotation{ 0.0f, 0.0f, 0.0f }
	, m_LocalScale{ 1.0f, 1.0f, 1.0f }
	, m_WorldPosition{ 0.0f, 0.0f, 0.0f }
	, m_WorldRotation{ 0.0f, 0.0f, 0.0f }
	, m_WorldScale{ 1.0f, 1.0f, 1.0f }
	, m_Look{ 0.0f, 0.0f, 1.0f }
	, m_Up{ 0.0f, 1.0f, 0.0f }
	, m_Right{ 1.0f, 0.0f, 0.0f }
	, m_NeedTMUpdate(true)
{
}

Transform::~Transform()
{
}

void Transform::Release()
{
	__super::Release();
}

void Transform::UpdateLocalTM()
{
	// Local TM�� ����Ѵ�(DX�̹Ƿ� SRT ������ �����)
	m_LocalTM =
		// S
		XMMatrixScaling(
			m_LocalScale.x
			, m_LocalScale.y
			, m_LocalScale.z)
		// R
		* XMMatrixRotationRollPitchYaw(
			m_LocalRotation.x		// Pich - X�� ȸ��
			, m_LocalRotation.y		// Yaw  - Y�� ȸ��
			, m_LocalRotation.z		// Roll - Z�� ȸ��
		)
		// T
		* XMMatrixTranslation(
			m_LocalPosition.x
			, m_LocalPosition.y
			, m_LocalPosition.z
		);

	// �ڽ��� �ֻ��� ��Ʈ��� look up right�� ��������
	std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();
	if (_parent == nullptr)
	{
		XMVECTOR UnitX{ 1.0f, 0.0f, 0.0f };
		XMVECTOR UnitY{ 0.0f, 1.0f, 0.0f };
		XMVECTOR UnitZ{ 0.0f, 0.0f, 1.0f };

		XMStoreFloat3(&m_Look, XMVector3TransformNormal(UnitZ, m_LocalTM));
		XMStoreFloat3(&m_Up, XMVector3TransformNormal(UnitY, m_LocalTM));
		XMStoreFloat3(&m_Right, XMVector3TransformNormal(UnitX, m_LocalTM));
	}

	// ���� TM�� ����Ǿ����Ƿ� ������Ʈ �÷��׸� ����
	FlagNeedTMUpdate();
}

void Transform::UpdateWorldTM()
{
	// �θ��� ����TM * �ڽ��� Local TM => �ڽ��� WorldTM
	// �� �θ��� ���� ��ġ�� �ڽ��� �������� �ΰ�, �ڽ��� ���� TM ��ŭ �Ű��� ���� �ڽ��� World TM�� �ȴ�

	// �θ��� ����TM�� �ڽ��� ������ �����ش�.
	std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();

	if (_parent != nullptr)
	{
		// �θ� �����Ѵٸ� �θ��� ����TM�� �ڽ��� ����TM�� ���Ѵ�
		m_WorldTM = m_LocalTM * (_parent->GetComponent<Transform>()->GetWorldTM());

		// ������ ��Ҹ� �и��ؼ� ������ ������, �����̼�, �������� �����Ѵ�
		XMVECTOR tmpScale;
		XMVECTOR tmpQuat;
		XMVECTOR tmpPosition;
		::XMMatrixDecompose(&tmpScale, &tmpQuat, &tmpPosition, m_WorldTM);

		::XMStoreFloat3(&m_WorldScale, tmpScale);
		m_WorldRotationQuat = tmpQuat;
		::XMStoreFloat3(&m_WorldPosition, tmpPosition);

		// ���ʹϾ����� �� �����̼��� ���Ϸ� ������ �ٲ۴�
		//m_WorldRotation = std::move(Transform::QuaternionToEuler(m_WorldRotationQuat));
		m_WorldRotation = Transform::QuaternionToEuler(m_WorldRotationQuat);
	}
	else
	{
		// �ڱ� �ڽ��� �θ��� ���� ����TM�� �״�� WorldTM�̴�
		m_WorldTM = m_LocalTM;
	}

	// look up right�� �����Ѵ�
	XMVECTOR UnitX{ 1.0f, 0.0f, 0.0f };
	XMVECTOR UnitY{ 0.0f, 1.0f, 0.0f };
	XMVECTOR UnitZ{ 0.0f, 0.0f, 1.0f };

	XMStoreFloat3(&m_Look, XMVector3TransformNormal(UnitZ, m_WorldTM));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(UnitY, m_WorldTM));
	XMStoreFloat3(&m_Right, XMVector3TransformNormal(UnitX, m_WorldTM));

	// WorldTM�� ���� �߱� ������ TM Update�� �÷��׸� off�� ����
	m_NeedTMUpdate = false;
}

void Transform::FlagNeedTMUpdate()
{
	m_NeedTMUpdate = true;

	// ���� ������Ʈ�� ��� �ڽĵ��� TM�� ������Ʈ�ϵ��� �Ѵ�
	// (���� ���� ������Ʈ�� TM�� ����Ǿ����Ƿ� �ڽĵ��� TM�� �����ؾ� �ϱ� ����)
	std::vector<std::shared_ptr<GameObject>>& children = this->GetGameObject()->GetChildren();

	// ��� �ڽ��� Ʈ�������� TM Update�� �ʿ��ϵ���!
	for (auto& child : children)
	{
		child->GetComponent<Transform>()->FlagNeedTMUpdate();
	}
}

void Transform::SetPosition(const XMFLOAT3& _pos, Space _relativeTo /*= Space::LOCAL*/)
{
	if (_relativeTo == Space::LOCAL)
	{
		m_LocalPosition = _pos;
		// Local TM�� �ǽð����� ������
		UpdateLocalTM();
	}
	else
	{
		m_WorldPosition = _pos;

		std::shared_ptr<GameObject> parent = GetGameObject()->GetParent();

		XMFLOAT3 parentPosition{ 0.0f, 0.0f, 0.0f };

		if (parent != nullptr)
			parentPosition = parent->GetComponent<Transform>()->GetWorldPosition();

		XMFLOAT3 newLocalPosition{
			_pos.x - parentPosition.x
			, _pos.y - parentPosition.y
			, _pos.z - parentPosition.z
		};

		SetPosition(newLocalPosition);
	}
}

void Transform::SetPosition(const XMVECTOR& _pos, Space _relativeTo /*= Space::LOCAL*/)
{
	XMFLOAT3 tmp;
	::XMStoreFloat3(&tmp, _pos);
	SetPosition(tmp, _relativeTo);
}

void Transform::SetRotation(const XMFLOAT3& _eulerAngle, Space _relativeTo /*= Space::LOCAL*/)
{
	if (_relativeTo == Space::LOCAL)
	{
		m_LocalRotation = _eulerAngle;
		// Local TM�� �ǽð����� �����Ѵ�
		UpdateLocalTM();
	}
	else
	{
		m_WorldRotation = _eulerAngle;

		std::shared_ptr<GameObject> parent = GetGameObject()->GetParent();

		XMFLOAT3 parentRotation{ 0.0f, 0.0f, 0.0f };

		if (parent != nullptr)
			parentRotation = parent->GetComponent<Transform>()->GetWorldRotation();

		XMFLOAT3 newLocalRotation{
			_eulerAngle.x - parentRotation.x
			, _eulerAngle.y - parentRotation.y
			, _eulerAngle.z - parentRotation.z
		};

		SetRotation(newLocalRotation);
	}
}

void Transform::SetRotation(const XMVECTOR& _eulerAngle, Space _relativeTo /*= Space::LOCAL*/)
{
	XMFLOAT3 tmp;
	::XMStoreFloat3(&tmp, _eulerAngle);
	SetRotation(tmp, _relativeTo);
}

void Transform::SetRotationLocalQuat(const SimpleMath::Quaternion _quat)
{
	XMMATRIX _SclTM = XMMatrixScalingFromVector(XMLoadFloat3(&m_LocalScale));
	XMMATRIX _RotTM = XMMatrixRotationQuaternion(_quat);
	XMMATRIX _TransTM = XMMatrixTranslationFromVector(XMLoadFloat3(&m_LocalPosition));
	m_LocalTM = _SclTM * _RotTM * _TransTM;

	m_LocalRotation = QuaternionToEuler(_quat);

	// �ڽ��� �ֻ��� ��Ʈ��� look up right�� ��������
	std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();
	if (_parent == nullptr)
	{
		XMVECTOR UnitX{ 1.0f, 0.0f, 0.0f };
		XMVECTOR UnitY{ 0.0f, 1.0f, 0.0f };
		XMVECTOR UnitZ{ 0.0f, 0.0f, 1.0f };

		XMStoreFloat3(&m_Look, XMVector3TransformNormal(UnitZ, m_LocalTM));
		XMStoreFloat3(&m_Up, XMVector3TransformNormal(UnitY, m_LocalTM));
		XMStoreFloat3(&m_Right, XMVector3TransformNormal(UnitX, m_LocalTM));
	}

	// ���� TM�� ����Ǿ����Ƿ� ������Ʈ �÷��׸� ����
	FlagNeedTMUpdate();
}

void Transform::SetScale(const XMFLOAT3& _localScale)
{
	m_LocalScale = _localScale;
	UpdateLocalTM();
}

void Transform::SetScale(const XMVECTOR& _localScale)
{
	XMFLOAT3 tmp;
	::XMStoreFloat3(&tmp, _localScale);
	SetRotation(tmp);
}

void Transform::SetLocalTM(const XMMATRIX& _localTM)
{
	m_LocalTM = _localTM;

	XMVECTOR outScale;
	XMVECTOR outQuat;
	XMVECTOR outPosition;
	::XMMatrixDecompose(&outScale, &outQuat, &outPosition, m_LocalTM);

	::XMStoreFloat3(&m_LocalScale, outScale);
	m_LocalRotation = QuaternionToEuler(outQuat);
	::XMStoreFloat3(&m_LocalPosition, outPosition);

	FlagNeedTMUpdate();
}

void Transform::SetWorldTM(const XMMATRIX& _worldTM)
{
	m_WorldTM = _worldTM;

	std::shared_ptr<GameObject> parent = GetGameObject()->GetParent();

	if (parent != nullptr)
	{
		XMMATRIX parentWorld = parent->GetComponent<Transform>()->GetWorldTM();
		m_LocalTM = m_WorldTM * XMMatrixInverse(nullptr, parentWorld);
	}
	else
		m_LocalTM = m_WorldTM;

	// ���� TM�� ��������
	XMVECTOR outWorldScale;
	XMVECTOR outWorldQuat;
	XMVECTOR outWorldPosition;
	::XMMatrixDecompose(&outWorldScale, &outWorldQuat, &outWorldPosition, m_WorldTM);
	
	// ���������� ������ ����
	::XMStoreFloat3(&m_WorldScale, outWorldScale);			// ���� ������
	m_WorldRotation = QuaternionToEuler(outWorldQuat);		// ���� �����̼�
	::XMStoreFloat3(&m_WorldPosition, outWorldPosition);	// ���� ������


	// ���� TM�� ��������
	XMVECTOR outLocalScale;
	XMVECTOR outLocalQuat;
	XMVECTOR outLocalPosition;
	::XMMatrixDecompose(&outLocalScale, &outLocalQuat, &outLocalPosition, m_LocalTM);

	// ���������� ������ ����
	::XMStoreFloat3(&m_LocalScale, outLocalScale);			// ���� ������
	m_LocalRotation = QuaternionToEuler(outLocalQuat);		// ���� �����̼�
	::XMStoreFloat3(&m_LocalPosition, outLocalPosition);	// ���� ������


	FlagNeedTMUpdate();
}

const DirectX::XMMATRIX& Transform::GetWorldTM()
{
	if (m_NeedTMUpdate)
		UpdateWorldTM();
	return m_WorldTM;
}

DirectX::XMFLOAT3 Transform::QuaternionToEuler(SimpleMath::Quaternion quat)
{
	XMVECTOR tmp = quat;
	return Transform::QuaternionToEuler(tmp);
}

DirectX::XMFLOAT3 Transform::QuaternionToEuler(XMVECTOR& _quat)
{
	XMFLOAT4 quat;
	::XMStoreFloat4(&quat, _quat);

	// ���� : Z / X / Y �� ������ ȸ���ϴ� �Ϳ� ���ؼ� �����.
	float x = quat.x;
	float y = quat.y;
	float z = quat.z;
	float w = quat.w;

	//float sqx = x * x;
	//float sqy = y * y;
	//float sqz = z * z;
	//float sqw = w * w;

	// C++���� asin�� �Ű������� -1 ~ 1 ���̷θ� ����
	// (�� �̻��� �� ���� ��Ȯ�� 90�� �Ǵ� -90���� Ư������)
	float exceptCheck = 2.0f * (w * x - y * z);

	float eulerX = 0.f;
	if (abs(exceptCheck) >= 1.f)
		// copysign : _excepCheck�� ��ȣ�� 2���� ����, 90���� ��ȯ�Ѵ�.
		eulerX = copysign(DirectX::XM_PI / 2, exceptCheck);
	else
		eulerX = std::asin(exceptCheck);
		//eulerX = exceptCheck;	// �߸��� �ڵ�. ������ ���� ���ܵд�

	// ����� �� �� ���⿡ ��Ű�ǵ�� �������� ������
	//float eulerY = atan2(2.0f * (x * z + w * y), (-sqx - sqy + sqz + sqw));
	//float eulerZ = atan2(2.0f * (x * y + z * w), (-sqx + sqy - sqz + sqw));
	float eulerY = atan2(2.0f * (x * z + w * y), (1 - 2 * (y * y + x * x)));
	float eulerZ = atan2(2.0f * (x * y + z * w), (1 - 2 * (x * x + z * z)));

	return XMFLOAT3{ eulerX, eulerY, eulerZ };
}

void Transform::Init()
{

}

void Transform::Update(float deltaTime)
{

}

void Transform::Render()
{

}
