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
	// Local TM을 계산한다(DX이므로 SRT 순으로 계산함)
	m_LocalTM =
		// S
		XMMatrixScaling(
			m_LocalScale.x
			, m_LocalScale.y
			, m_LocalScale.z)
		// R
		* XMMatrixRotationRollPitchYaw(
			m_LocalRotation.x		// Pich - X축 회전
			, m_LocalRotation.y		// Yaw  - Y축 회전
			, m_LocalRotation.z		// Roll - Z축 회전
		)
		// T
		* XMMatrixTranslation(
			m_LocalPosition.x
			, m_LocalPosition.y
			, m_LocalPosition.z
		);

	// 자신이 최상위 루트라면 look up right를 갱신하자
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

	// 로컬 TM이 변경되었으므로 업데이트 플래그를 세움
	FlagNeedTMUpdate();
}

void Transform::UpdateWorldTM()
{
	// 부모의 월드TM * 자신의 Local TM => 자신의 WorldTM
	// 즉 부모의 현재 위치를 자신의 원점으로 두고, 자신의 로컬 TM 만큼 옮겨준 것이 자신의 World TM이 된다

	// 부모의 월드TM에 자신의 로컬을 곱해준다.
	std::shared_ptr<GameObject> _parent = GetGameObject()->GetParent();

	if (_parent != nullptr)
	{
		// 부모가 존재한다면 부모의 월드TM에 자신의 로컬TM을 곱한다
		m_WorldTM = m_LocalTM * (_parent->GetComponent<Transform>()->GetWorldTM());

		// 월드의 요소를 분리해서 각각의 스케일, 로테이션, 포지션을 갱신한다
		XMVECTOR tmpScale;
		XMVECTOR tmpQuat;
		XMVECTOR tmpPosition;
		::XMMatrixDecompose(&tmpScale, &tmpQuat, &tmpPosition, m_WorldTM);

		::XMStoreFloat3(&m_WorldScale, tmpScale);
		m_WorldRotationQuat = tmpQuat;
		::XMStoreFloat3(&m_WorldPosition, tmpPosition);

		// 쿼터니언으로 된 로테이션을 오일러 각으로 바꾼다
		//m_WorldRotation = std::move(Transform::QuaternionToEuler(m_WorldRotationQuat));
		m_WorldRotation = Transform::QuaternionToEuler(m_WorldRotationQuat);
	}
	else
	{
		// 자기 자신이 부모인 경우는 로컬TM이 그대로 WorldTM이다
		m_WorldTM = m_LocalTM;
	}

	// look up right를 수정한다
	XMVECTOR UnitX{ 1.0f, 0.0f, 0.0f };
	XMVECTOR UnitY{ 0.0f, 1.0f, 0.0f };
	XMVECTOR UnitZ{ 0.0f, 0.0f, 1.0f };

	XMStoreFloat3(&m_Look, XMVector3TransformNormal(UnitZ, m_WorldTM));
	XMStoreFloat3(&m_Up, XMVector3TransformNormal(UnitY, m_WorldTM));
	XMStoreFloat3(&m_Right, XMVector3TransformNormal(UnitX, m_WorldTM));

	// WorldTM을 변경 했기 때문에 TM Update의 플래그를 off로 변경
	m_NeedTMUpdate = false;
}

void Transform::FlagNeedTMUpdate()
{
	m_NeedTMUpdate = true;

	// 게임 오브젝트의 모든 자식들의 TM을 업데이트하도록 한다
	// (현재 게임 오브젝트의 TM이 변경되었으므로 자식들의 TM도 변경해야 하기 때문)
	std::vector<std::shared_ptr<GameObject>>& children = this->GetGameObject()->GetChildren();

	// 모든 자식의 트랜스폼에 TM Update가 필요하도록!
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
		// Local TM은 실시간으로 갱신함
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
		// Local TM은 실시간으로 갱신한다
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

	// 자신이 최상위 루트라면 look up right를 갱신하자
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

	// 로컬 TM이 변경되었으므로 업데이트 플래그를 세움
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

	// 월드 TM의 디컴포즈
	XMVECTOR outWorldScale;
	XMVECTOR outWorldQuat;
	XMVECTOR outWorldPosition;
	::XMMatrixDecompose(&outWorldScale, &outWorldQuat, &outWorldPosition, m_WorldTM);
	
	// 디컴포즈한 성분을 저장
	::XMStoreFloat3(&m_WorldScale, outWorldScale);			// 월드 스케일
	m_WorldRotation = QuaternionToEuler(outWorldQuat);		// 월드 로테이션
	::XMStoreFloat3(&m_WorldPosition, outWorldPosition);	// 월드 포지션


	// 로컬 TM의 디컴포즈
	XMVECTOR outLocalScale;
	XMVECTOR outLocalQuat;
	XMVECTOR outLocalPosition;
	::XMMatrixDecompose(&outLocalScale, &outLocalQuat, &outLocalPosition, m_LocalTM);

	// 디컴포즈한 성분을 저장
	::XMStoreFloat3(&m_LocalScale, outLocalScale);			// 로컬 스케일
	m_LocalRotation = QuaternionToEuler(outLocalQuat);		// 로컬 로테이션
	::XMStoreFloat3(&m_LocalPosition, outLocalPosition);	// 로컬 포지션


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

	// 주의 : Z / X / Y 축 순으로 회전하는 것에 대해서 만든다.
	float x = quat.x;
	float y = quat.y;
	float z = quat.z;
	float w = quat.w;

	//float sqx = x * x;
	//float sqy = y * y;
	//float sqz = z * z;
	//float sqw = w * w;

	// C++에서 asin은 매개변수로 -1 ~ 1 사이로만 받음
	// (그 이상이 된 경우는 정확한 90도 또는 -90도인 특이점임)
	float exceptCheck = 2.0f * (w * x - y * z);

	float eulerX = 0.f;
	if (abs(exceptCheck) >= 1.f)
		// copysign : _excepCheck의 부호로 2분의 파이, 90도를 반환한다.
		eulerX = copysign(DirectX::XM_PI / 2, exceptCheck);
	else
		eulerX = std::asin(exceptCheck);
		//eulerX = exceptCheck;	// 잘못된 코드. 교훈을 위해 남겨둔다

	// 결과는 둘 다 같기에 위키피디아 수식으로 수정함
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
