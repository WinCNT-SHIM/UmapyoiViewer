#include "GameEnginePCH.h"
#include "Camera.h"

#include "Transform.h"
#include "GameEngine.h"

Camera::Camera(std::weak_ptr<GameObject> _GameObj)
	: ComponentBase(_GameObj, "Camera")
	, m_Transform{ GetGameObject()->GetComponent<Transform>() }
	, m_ProjectType{ ProjectType::PERSPECTIVE }
	, m_NearZ{ 1.0f }
	, m_FarZ{ 60.0f }
	, m_Aspect{ static_cast<float>(GameEngine::GetScreenWidth()) / static_cast<float>(GameEngine::GetScreenHeight()) }
	, m_FovY(DirectX::XM_PI / 4)
	, m_NearWindowHeight{ 2.0f * m_NearZ * tanf(0.5f * m_FovY) }
	, m_FarWindowHeight{ 2.0f * m_FarZ * tanf(0.5f * m_FovY) }
	, m_CameraInfo{ std::make_shared<CameraInfo>() }
{
	UpdateViewMatrix();
	UpdateProjMatrix();
}

Camera::~Camera()
{
}

void Camera::Release()
{
	m_Transform.reset();
	__super::Release();
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
	// cache properties
	m_FovY = fovY;
	m_Aspect = aspect;
	m_NearZ = zn;
	m_FarZ = zf;

	m_NearWindowHeight = 2.0f * m_NearZ * tanf(0.5f * m_FovY);
	m_FarWindowHeight = 2.0f * m_FarZ * tanf(0.5f * m_FovY);
	
	UpdateProjMatrix();
}

void Camera::UpdateViewMatrix()
{
	m_CameraInfo->m_PrevViewMatrix = m_CameraInfo->m_ViewMatrix;
	::XMStoreFloat4x4(&m_CameraInfo->m_ViewMatrix, ::XMMatrixInverse(nullptr, m_Transform->GetWorldTM()));
}

void Camera::UpdateProjMatrix()
{
	::XMStoreFloat4x4(&m_CameraInfo->m_ProjMatrix, ::XMMatrixPerspectiveFovLH(m_FovY, m_Aspect, m_NearZ, m_FarZ));

	// ���� ���� ����� �����ϴ� ������...��� ����
	m_CameraInfo->m_PrevProjMatrix = m_CameraInfo->m_ProjMatrix;

	// �������� �ø��� ���� ������ ���� ����
	//UpdateBoundingFrustum();
}

void Camera::Init()
{
}

void Camera::Update(float deltaTime)
{
	m_CameraInfo->m_CameraWorldPosition = m_Transform->GetWorldPosition();
	UpdateViewMatrix();
}

void Camera::LateUpdate(float deltaTime)
{
	// todo: ī�޶� �̵� �ӽ� ����
	float moveSpeed = 10.f;
	if (GetAsyncKeyState(VK_SHIFT) & 0x8001)
		moveSpeed = 150.0f;
	else
		moveSpeed = 10.f;
	moveSpeed *= deltaTime;

	if (GetAsyncKeyState('W') & 0x8000)
	{
		XMVECTOR tmp =
			::XMLoadFloat3(&m_Transform->GetWorldPosition())
			+ ::XMVectorScale(
				XMLoadFloat3(&m_Transform->GetLook()), moveSpeed
			);
		m_Transform->SetPosition(tmp, Space::WORLD);
	}

	if (GetAsyncKeyState('S') & 0x8000)
	{
		XMVECTOR tmp =  
			::XMLoadFloat3(&m_Transform->GetWorldPosition())
			- ::XMVectorScale(
				XMLoadFloat3(&m_Transform->GetLook()), moveSpeed
			);
		m_Transform->SetPosition(tmp, Space::WORLD);
	}

	if (GetAsyncKeyState('A') & 0x8000)
	{
		XMVECTOR tmp =
			::XMLoadFloat3(&m_Transform->GetWorldPosition())
			- ::XMVectorScale(
				XMLoadFloat3(&m_Transform->GetRight()), moveSpeed
			);
		m_Transform->SetPosition(tmp, Space::WORLD);
	}

	if (GetAsyncKeyState('D') & 0x8000)
	{
		XMVECTOR tmp =
			::XMLoadFloat3(&m_Transform->GetWorldPosition())
			+ ::XMVectorScale(
				XMLoadFloat3(&m_Transform->GetRight()), moveSpeed
			);
		m_Transform->SetPosition(tmp, Space::WORLD);
	}

	if (GetAsyncKeyState('Q') & 0x8000)
	{
		XMVECTOR tmp =
			::XMLoadFloat3(&m_Transform->GetWorldPosition())
			- ::XMVectorScale(
				XMLoadFloat3(&m_Transform->GetUp()), moveSpeed
			);
		m_Transform->SetPosition(tmp, Space::WORLD);
	}

	if (GetAsyncKeyState('E') & 0x8000)
	{
		XMVECTOR tmp =
			::XMLoadFloat3(&m_Transform->GetWorldPosition())
			+ ::XMVectorScale(
				XMLoadFloat3(&m_Transform->GetUp()), moveSpeed
			);
		m_Transform->SetPosition(tmp, Space::WORLD);
	}

	// ���콺 �̵��� ���� ����
	POINT tmpCurrentMousePos;
	GetCursorPos(&tmpCurrentMousePos);

	SimpleMath::Vector2 currentMousePos;
	currentMousePos.x = tmpCurrentMousePos.x;
	currentMousePos.y = tmpCurrentMousePos.y;

	if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
	{
		// 1�� �ȼ� �� 4���� 1 �� => �ϰ� �ٽ� Radian���� Convert
		SimpleMath::Vector2 deltaMouseMove
			= 0.15f * (m_PrevMousePos - currentMousePos) * XM_PI / 180;

		// World Right, Up Vector �������� ������.
		SimpleMath::Vector3 worldXY
			= SimpleMath::Vector3(deltaMouseMove.y, deltaMouseMove.x, 0.f);

		// ���� ������ Look, Up ���͸� ȸ����Ű���. �׷� ��� ���͵��� �� ȸ������ŭ �߰��� ȸ���ϸ� �Ǵ°� �ƴѰ��� ?
		SimpleMath::Vector3 rot = m_Transform->GetWorldRotation();
		m_Transform->SetRotation(rot - worldXY, Space::WORLD);
	}

	m_PrevMousePos = currentMousePos;
}

void Camera::Render()
{
}
