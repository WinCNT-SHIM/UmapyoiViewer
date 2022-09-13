/// <summary>
/// 그래픽스 엔진에 필요한 정보를 모아둔 헤더
/// 게임 엔진에 노출시키는 부분
/// 따로 빼는 것도 괜찮을 것 같지만 혜성이를 따라서 그래픽스 엔진 폴더에 두자
/// 
/// Created : 2022/08/04 심승수
/// Updated : 2022/08/04 심승수
/// </summary>
#pragma once

#include <limits.h>

#ifdef _WINDOWS
#include <DirectXMath.h>
#include "../Inc/SimpleMath.h"
using namespace DirectX;
#endif

#define  _DX11RENDERER
#include "IDefine.h"

//#include "Graphics_DX11_typedef.h"
#include "./GraphicsPerFrameData.h"
#include "./GraphicsTextData.h"

struct ObjectInfo
{
	ObjectInfo()
		: m_MeshID(2)
		, m_ObjectID(0)
		, m_UsingLighting(true)
		, m_UsingShadow(false)
	{}

	unsigned int m_MeshID;
	unsigned int m_ObjectID;
	bool m_UsingLighting;
	bool m_UsingShadow;
};

struct TextureInfo 
{
	TextureInfo()
		: m_DiffuseMapID{ UINT_MAX }
		, m_NormalMapID{ UINT_MAX }
		, m_TripleMaskMapID{ UINT_MAX }
		, m_OptionMaskMapID{ UINT_MAX }
		, m_ToonShadowMapID{ UINT_MAX }
		, m_EnvironmentMapID{ UINT_MAX }
	{}

	unsigned int m_DiffuseMapID;
	unsigned int m_NormalMapID;
	unsigned int m_TripleMaskMapID;		// base
	unsigned int m_OptionMaskMapID;		// ctrl
	unsigned int m_ToonShadowMapID;		// shad_c
	unsigned int m_EnvironmentMapID;
	// eyes 관련
	unsigned int m_EyesHighlight00MapID;
	unsigned int m_EyesHighlight01MapID;
	unsigned int m_EyesHighlight02MapID;
};

struct MaterialInfo
{
	MaterialInfo()
		: m_TextureInfo()
		, m_Ambient{ 0.5f, 0.5f, 0.5f, 1.f }
		, m_Diffuse{ 0.5f, 0.5f, 0.5f, 1.f }
		, m_Specular{ 0.5f, 0.5f, 0.5f, 1.f }
		, m_Reflect{ 0.1f, 0.1f, 0.1f, 1.f }
	{}

	XMFLOAT4 m_Ambient;
	XMFLOAT4 m_Diffuse;
	XMFLOAT4 m_Specular;
	XMFLOAT4 m_Reflect;

	TextureInfo m_TextureInfo;
};

struct ShaderInfo
{
	enum class RasterizerState
	{
		SOLID = 0,
		WIREFRAME = 1,
	};

	ShaderInfo()
		: m_VSName{ _T("VS_Diffuse") }
		, m_PSName{ _T("PS_Diffuse") }
		, m_RasterizerState(RasterizerState::SOLID)
		, m_TechName{ _T("Basic") }
	{}

	tstring m_VSName;
	tstring m_PSName;
	RasterizerState m_RasterizerState;
	tstring m_TechName;		// 이펙트 효과 프레임워크용(FX)
};

struct GeometryInfo
{
	GeometryInfo()
		: m_World{ XMMatrixIdentity() }
		, m_WorldInvTranspose{ XMMatrixIdentity() }
		, m_View{ XMMatrixIdentity() }
		, m_Proj{ XMMatrixIdentity() }
		, m_WorldViewProj{ XMMatrixIdentity() }
		, m_TexTransform{ XMMatrixIdentity() }
	{}

	XMMATRIX m_World;
	XMMATRIX m_WorldInvTranspose;
	XMMATRIX m_View;
	XMMATRIX m_Proj;
	XMMATRIX m_WorldViewProj;
	XMMATRIX m_TexTransform;
};

struct AnimationInfo
{
	AnimationInfo()
		: m_BoneMatrixList{}
	{
	}

	std::vector<XMMATRIX> m_BoneMatrixList;
	//XMMATRIX m_BoneMatrixList[125];
	//SimpleMath::Matrix m_BoneMatrixList[125];
};

enum class RenderingdataType
{
	STATIC_MESH_OBJECT		// Static Mesh
	, SKINNED_MESH_OBJECT	// Skinned Mesh
	, SKY_BOX				// Sky Box (Cube Map)
};

struct RenderingData
{
	RenderingData()
		: m_DataType(RenderingdataType::STATIC_MESH_OBJECT)
		, m_ObjectInfo(std::make_shared<ObjectInfo>())
		, m_MaterialInfo(std::make_shared<MaterialInfo>())
		, m_ShaderInfo(std::make_shared<ShaderInfo>())
		, m_GeoInfo(std::make_shared<GeometryInfo>())
		, m_AnimInfo(std::make_shared<AnimationInfo>())
	{

	}

	~RenderingData()
	{
		m_ObjectInfo.reset();
		m_MaterialInfo.reset();
		m_ShaderInfo.reset();
		m_GeoInfo.reset();
		m_AnimInfo.reset();
	}

	RenderingdataType m_DataType;

	std::shared_ptr<ObjectInfo>    m_ObjectInfo;
	std::shared_ptr<MaterialInfo>  m_MaterialInfo;
	std::shared_ptr<ShaderInfo>    m_ShaderInfo;
	std::shared_ptr<GeometryInfo>  m_GeoInfo;
	std::shared_ptr<AnimationInfo> m_AnimInfo;
};