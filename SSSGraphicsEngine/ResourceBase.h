/// <summary>
/// ���ҽ��� Base Class
/// ������� ������ ����
/// ������ ���� ���� �ð��� ���� ����
/// 
/// Created : 2022/08/05 �ɽ¼�
/// Updated : 2022/08/05 �ɽ¼�
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
		// �ִ� 42�ﰳ�� ������Ʈ�� �ٷ� �� �ִ�
		// (��� ���� ���� �� �ٷ�)
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

		// ���۷��� ī��Ʈ�� 0�̵� ���� ������ �ð� => ������ �ξ Ư�� �ð���ŭ �귶�ٸ� ���ҽ� �ٿ�
		inline float GetTimeFromRefZero() { return m_TimeFromRefZero; }
		inline const std::string& GetName() { return m_ResourceName; }
		inline const uint32& GetID() { return m_ResourceID; }

	protected:
		ResourceType m_ResourceType;
		uint32 m_RefCount;
		float m_TimeFromRefZero;
		// Hashing, Searching � ���Ǵ� ���ҽ��� �̸�
		std::string m_ResourceName;
		// ���ҽ� ���� �ÿ� �߱޵Ǵ� ID
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