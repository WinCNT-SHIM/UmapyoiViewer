/// <summary>
/// 그래픽스 버퍼들의 기본 클래스
/// (지훈이 형 소스 코드를 많이(!) 참조함)
/// 
/// Created : 2022/07/06 심승수
/// Updated : 2022/07/06 심승수
/// </summary>
#pragma once
#include "ResourceBase_Mk1.h"

namespace GraphicsEngine
{
	class BufferBase :
		public ResourceBase_Mk1
	{
	protected:
		enum class BufferType
		{
			NONE,
			CONSTANT,
			VERTEX,
			INDEX
		};

	public:
		BufferBase(BufferType type) : m_Type(type) {}
		virtual ~BufferBase() = default;

	private:
		BufferType m_Type;
	};
}