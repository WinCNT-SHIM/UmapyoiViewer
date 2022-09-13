/// <summary>
/// DX�� ī�޶� ������Ʈ
/// 
/// Created : 2022/08/04 �ɽ¼�
/// Updated : 2022/08/04 �ɽ¼�
/// </summary>
#pragma once

// �׷��Ƚ� ������ Ŀ��Ʈ(...)�ϱ� ���� ���
#ifdef _DX11RENDERER
#include "../SSSGraphicsEngine/IGraphicsEngineBase.h"
#include "../SSSGraphicsEngine/DX11GraphicsEngine.h"
#include "../SSSGraphicsEngine/GraphicsRenderingData.h"
#endif

using namespace DirectX;	// XMFLOAT3, XMVECTOR. etc.

class Transform;

enum class ProjectType
{
	PERSPECTIVE,
	ORTHOGRAPHIC
};

class DLL_DECLSPEC Camera
	: public ComponentBase
{
public:
	Camera(std::weak_ptr<GameObject> _GameObj);
	virtual ~Camera();
	virtual void Release() override;

private:
	std::shared_ptr<Transform> m_Transform;

	//// ��(View), ����(Projection) ���
	//XMFLOAT4X4 m_ViewMatrix;	// �� ���
	//XMFLOAT4X4 m_ProjMatrix;	// ���� ���

	// ���� Ÿ��
	ProjectType m_ProjectType;

	// ����ü ���� ����
	float m_NearZ;
	float m_FarZ;
	float m_FovY;
	float m_Aspect;

	float m_NearWindowHeight;
	float m_FarWindowHeight;

	std::shared_ptr<CameraInfo> m_CameraInfo;

public:
	// ==================== Getter, Setter ====================
	// ���� ��ǥ�迡���� ī�޶� ��ġ�� ȹ��/�����Ѵ�
	//XMVECTOR GetPositionXM() const;
	//XMFLOAT3 GetPosition() const;
	//void SetPosition(float x, float y, float z);
	//void SetPosition(const XMFLOAT3& v);

	// ����ü ������ ȹ��
	inline const float& GetNearZ() const { return m_NearZ; }
	inline const float& GetFarZ() const { return m_FarZ; }
	inline const float& GetAspect() const { return m_Aspect; }
	inline const float& GetFovY() const { return m_FovY; }
	inline const float& GetFovX() const { return 2.0f * atan(0.5f * GetNearWindowWidth() / m_NearZ); }

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth() const { return m_Aspect * m_NearWindowHeight; }
	float GetNearWindowHeight() const { return m_NearWindowHeight; }
	float GetFarWindowWidth() const { return m_Aspect * m_FarWindowHeight; }
	float GetFarWindowHeight() const { return m_FarWindowHeight; }

	// ����ü ������ ����
	void SetNearZ(float nearZ) { m_NearZ = nearZ; UpdateProjMatrix(); }
	void SetFarZ(float farZ) { m_FarZ = farZ; UpdateProjMatrix(); }
	void SetAspect(float ratio) { m_Aspect = ratio; UpdateProjMatrix(); }
	void SetFov(float fovY) { m_FovY = fovY; UpdateProjMatrix(); }

	// ����ü �� ���� ����� �����Ѵ�
	void SetLens(float fovY, float aspect, float zn, float zf);

	// View/Proj ����� ȹ���Ѵ�
	//const XMMATRIX& GetViewMatrix() { /*UpdateViewMatrix();*/ XMMATRIX tmp = ::XMLoadFloat4x4(&m_ViewMatrix); return tmp; }
	const XMMATRIX& GetViewMatrix() { XMMATRIX tmp = ::XMLoadFloat4x4(&m_CameraInfo->m_ViewMatrix); return tmp; }
	//const XMMATRIX& GetProjMatrix() { /*UpdateProjMatrix();*/ XMMATRIX tmp = ::XMLoadFloat4x4(&m_ProjMatrix); return tmp; }
	const XMMATRIX& GetProjMatrix() { XMMATRIX tmp = ::XMLoadFloat4x4(&m_CameraInfo->m_ProjMatrix); return tmp; }
	//const XMMATRIX& GetViewProjMatrix() { XMMATRIX tmp = ::XMMatrixMultiply(::XMLoadFloat4x4(&m_ViewMatrix), ::XMLoadFloat4x4(&m_ProjMatrix)); return tmp; }
	const XMMATRIX& GetViewProjMatrix() { XMMATRIX tmp = ::XMMatrixMultiply(::XMLoadFloat4x4(&m_CameraInfo->m_ViewMatrix), ::XMLoadFloat4x4(&m_CameraInfo->m_ProjMatrix)); return tmp; }

	//// LookAt �Լ��� ī�޶� ������ �����Ѵ�
	//void LookAt(FXMVECTOR Pos, FXMVECTOR target, FXMVECTOR worldUp);
	//void LookAt(const XMFLOAT3& Pos, const XMFLOAT3& target, const XMFLOAT3& up);
	//
	//// Define transform space via LookAt parameters.
	//void LookAt(FXMVECTOR target, FXMVECTOR pos);
	//void LookAt(const XMFLOAT3& target, const XMFLOAT3& pos);
	//void LookAt(const XMFLOAT3& target);
	//void LookAt(GameObject* target);

	//// Strafe/Walk the camera a distance d.
	//void Strafe(float d);
	//void Walk(float d);
	//void WorldUpDown(float d);

	//// Rotate the transform.
	//void Pitch(float angle);
	//void RotateY(float angle);

	//// ==================== ī�޶��� ��ȯ ���� ====================
	//// ī�޶��� �����¿�յ� �̵�
	//void Strafe(float d);		// ī�޶��� �¿� �̵�
	//void Walk(float d);			// ī�޶��� �յ� �̵�
	//void WorldUpDown(float d);	// ī�޶��� ���� �̵�
	//
	//// ī�޶��� ȸ��
	//void Pitch(float angle);
	//void Yaw(float angle);
	//void Roll(float angle);

private:
	// After modifying camera position/orientation, call to rebuild the view matrix.
	void UpdateViewMatrix();
	void UpdateProjMatrix();

public:
	void Init() override;
	void FixedUpdate(float deltaTime) override {}
	void Update(float deltaTime) override;
	void LateUpdate(float deltaTime) override;
	void Render() override;

	friend class RenderingManager;	// ���ϰ� ����

	// todo: ī�޶� �̵� �ӽ� ����
private:
	SimpleMath::Vector2 m_PrevMousePos;
};
