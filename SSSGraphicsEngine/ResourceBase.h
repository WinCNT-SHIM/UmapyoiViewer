/// <summary>
/// 리소스의 Base Class
/// 대대적인 수정을 하자
/// 어차피 내겐 생각 시간은 이제 없음
/// 
/// Created : 2022/08/05 심승수
/// Updated : 2022/08/05 심승수
/// </summary>
#pragma once

#include "ResourceDefine.h"

#define  _DX11RENDERER
#include "IDefine.h"

namespace GraphicsEngine
{
	class ResourceIDGenerator
	{
		inline static uint32 m_NextID = 0;

	public:
		// 최대 42억개의 오브젝트를 다룰 수 있다
		// (사실 뻥임 절대 못 다룸)
		inline static uint32 Generate() { return m_NextID++; }
	};

	template <ResourceType resourceType>
	class ResourceBase
	{
	public:
		ResourceBase();
		virtual ~ResourceBase();
		virtual void Release() abstract;
		void Reset() { m_RefCount = max(0, m_RefCount--); }

	public:
		inline uint32 GetRefCount() { return m_RefCount; }
		inline ResourceType GetResourceType() { return m_ResourceType; }

		// 레퍼런스 카운트가 0이된 이후 지나간 시간 => 기준을 두어서 특정 시간만큼 흘렀다면 리소스 다운
		inline float GetTimeFromRefZero() { return m_TimeFromRefZero; }
		inline const std::string& GetName() { return m_ResourceName; }
		inline const uint32& GetID() { return m_ResourceID; }

	protected:
		ResourceType m_ResourceType;
		uint32 m_RefCount;
		float m_TimeFromRefZero;
		// Hashing, Searching 등에 사용되는 리소스의 이름
		std::string m_ResourceName;
		// 리소스 생성 시에 발급되는 ID
		uint32 m_ResourceID;
	};

	template <ResourceType resourceType>
	ResourceBase<resourceType>::ResourceBase()
		: m_ResourceType(resourceType)
		, m_RefCount(0)
		, m_TimeFromRefZero(0.f)
	{
		m_ResourceID = ResourceIDGenerator::Generate();
	}

	template <ResourceType resourceType>
	ResourceBase<resourceType>::~ResourceBase()
	{
		
	}
}