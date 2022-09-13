/// <summary>
/// �׷��Ƚ� ���۵��� �⺻ Ŭ����
/// (������ �� �ҽ� �ڵ带 ����(!) ������)
/// 
/// Created : 2022/07/06 �ɽ¼�
/// Updated : 2022/07/06 �ɽ¼�
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