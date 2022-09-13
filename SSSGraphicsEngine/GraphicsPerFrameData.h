/// <summary>
/// 그래픽스 엔진에 필요한 정보를 모아둔 헤더
/// 게임 엔진에 노출시키는 부분
/// 따로 빼는 것도 괜찮을 것 같지만 혜성이를 따라서 그래픽스 엔진 폴더에 두자
/// (Thanks, Commet!!)
/// 
/// Created : 2022/08/04 심승수
/// Updated : 2022/08/04 심승수
/// </summary>
#pragma once

#include <random>

#ifdef _WINDOWS
#include <DirectXMath.h>
#include "../Inc/SimpleMath.h"
using namespace DirectX;
#endif

struct CameraInfo
{
	CameraInfo()
		: m_CameraWorldPosition{ 0.0f, 0.0f, 0.0f }
		, m_Pad(0.0f)
		, m_ViewMatrix{}
		, m_ProjMatrix{}
		, m_PrevViewMatrix{}
		, m_PrevProjMatrix{}
	{
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_PrevViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_PrevProjMatrix, XMMatrixIdentity());
	}

	XMFLOAT3 m_CameraWorldPosition;

private:
	float m_Pad;

public:
	XMFLOAT4X4 m_ViewMatrix;
	XMFLOAT4X4 m_ProjMatrix;
	XMFLOAT4X4 m_PrevViewMatrix;
	XMFLOAT4X4 m_PrevProjMatrix;
};

struct LightInfo
{
	LightInfo()
		: m_Ambient{ 0.5f, 0.5f, 0.5f, 1.f }
		, m_Diffuse{ 0.5f, 0.5f, 0.5f, 1.f }
		, m_Specular{ 0.5f, 0.5f, 0.5f, 16.f }
	{}

	XMFLOAT4 m_Ambient;
	XMFLOAT4 m_Diffuse;
	XMFLOAT4 m_Specular;
};

struct DirectionalLightInfo final
	: public LightInfo
{
	DirectionalLightInfo()
		: LightInfo()
		, m_Direction{ 1.0f, 0.0f, 0.0f }
		, m_Pad{ 0.0f }
	{
	}

	//// 복사 생성자
	//DirectionalLightInfo(const DirectionalLightInfo& oth)
	//{
	//	m_Ambient = oth.m_Ambient;
	//	m_Diffuse = oth.m_Diffuse;
	//	m_Specular = oth.m_Specular;
	//	m_Direction = oth.m_Direction;
	//	m_Pad = oth.m_Pad;
	//}

	//// 이동생성자
	//DirectionalLightInfo(DirectionalLightInfo&& oth) noexcept
	//{
	//	m_Ambient = oth.m_Ambient;
	//	m_Diffuse = oth.m_Diffuse;
	//	m_Specular = oth.m_Specular;
	//	m_Direction = oth.m_Direction;
	//	m_Pad = oth.m_Pad;
	//}

	// World Rotation Look Vector
	XMFLOAT3 m_Direction;

private:
	float m_Pad;
};

struct PointLightInfo final
	: public LightInfo
{
	PointLightInfo()
		: LightInfo()
		, m_Position{ 0.0f, 0.0f, 0.0f }
		, m_Range{ 25.0f }
		, m_Attenuation{ 0.f, 0.1f, 0.0f }
		, m_Pad{ 0.0f }
	{}

	XMFLOAT3 m_Position;
	float m_Range;
	XMFLOAT3 m_Attenuation;

private:
	float m_Pad;
};

struct SpotLightInfo final
	: public LightInfo
{
	SpotLightInfo()
		: LightInfo()
		, m_Position{ 0.0f, 0.0f, 0.0f }
		, m_Range{ 25.0f }
		, m_Direction{ 1.f, 0.f, 0.f }
		, m_Spot{ 10.0f }
		, m_Attenuation{ 0.0f, 0.1f, 0.0f }
		, m_Pad(0.0f)
	{

	}

	XMFLOAT3 m_Position;
	float m_Range;
	XMFLOAT3 m_Direction;
	float m_Spot;
	XMFLOAT3 m_Attenuation;

private:
	float m_Pad;
};

struct SSAOInfo final
{
	SSAOInfo()
		: m_OcclusionRadius(0.5f)
		, m_OcclusionFadeStart(0.2f)
		, m_OcclusionFadeEnd(2.f)
		, m_SurfaceEpsilon(0.05f)
		, m_SampleCount(10)
	{
		// 임시 오프셋 벡터
		XMVECTOR offsetVectors[14] =
		{
			// 8 cube corners
			{+1.0f, +1.0f, +1.0f, 0.0f},
			{-1.0f, -1.0f, -1.0f, 0.0f},
			{-1.0f, +1.0f, +1.0f, 0.0f},
			{+1.0f, -1.0f, -1.0f, 0.0f},
			{+1.0f, +1.0f, -1.0f, 0.0f},
			{-1.0f, -1.0f, +1.0f, 0.0f},
			{-1.0f, +1.0f, -1.0f, 0.0f},
			{+1.0f, -1.0f, +1.0f, 0.0f},

			// 6 ters of cube faces
			{-1.0f, 0.0f, 0.0f, 0.0f},
			{+1.0f, 0.0f, 0.0f, 0.0f},
			{0.0f, -1.0f, 0.0f, 0.0f},
			{0.0f, +1.0f, 0.0f, 0.0f},
			{0.0f, 0.0f, -1.0f, 0.0f},
			{0.0f, 0.0f, +1.0f, 0.0f}
		};

		// 난수 생성
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dist(0.25f, 1.0f);

		for (int i = 0; i < 14; ++i)
		{
			// 0.25 ~ 1 사이의 Random Length
			float s = dist(gen);

			auto normalizedVec = XMVector4Normalize(offsetVectors[i]);
			XMStoreFloat4(&m_OffsetVectors[i], XMVectorScale(normalizedVec, s));
		}
	}

	// 최대 14개의 Sampling을 지원함
	XMFLOAT4 m_OffsetVectors[14];

	// Left Top, Right Top, Left Bottom, Right Bottom
	XMFLOAT4 m_CameraFrustumCorner[4];

	float m_OcclusionRadius;
	float m_OcclusionFadeStart;
	float m_OcclusionFadeEnd;
	float m_SurfaceEpsilon;

	unsigned int m_SampleCount;

	XMFLOAT3 m_Pad;
};

constexpr unsigned int LIGHT_INFO_MAX = 100;

struct PerFrameData
{
	PerFrameData()
		: m_CameraInfo{ std::make_shared<CameraInfo>() }
		, m_DirectionalLightInfos{ std::vector<std::shared_ptr<DirectionalLightInfo>>() }
		, m_SpotLightInfos{ std::vector<std::shared_ptr<SpotLightInfo>>() }
		, m_PointLightInfos{ std::vector<std::shared_ptr<PointLightInfo>>() }
		, m_SSAOInfo{ std::make_shared<SSAOInfo>() }
	{
	}

	std::shared_ptr<CameraInfo> m_CameraInfo;
	std::vector<std::shared_ptr<DirectionalLightInfo>> m_DirectionalLightInfos;
	std::vector<std::shared_ptr<SpotLightInfo>> m_SpotLightInfos;
	std::vector<std::shared_ptr<PointLightInfo>> m_PointLightInfos;
	std::shared_ptr<SSAOInfo> m_SSAOInfo;

	void Clear()
	{
		m_DirectionalLightInfos.clear();
		m_SpotLightInfos.clear();
		m_PointLightInfos.clear();
	}
};