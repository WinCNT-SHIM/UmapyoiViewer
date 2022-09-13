/// <summary>
/// 리소스의 Base Class
/// 아직 하는 일은 없다
/// 나중에 리소스 매니저에서 다형성으로 뭔가를 할 수 있을 지도?
/// 
/// Created : 2022/07/06 심승수
/// Updated : 2022/07/06 심승수
/// </summary>
#pragma once

namespace GraphicsEngine
{
	class ResourceBase_Mk1
	{
	protected:
		unsigned int m_ReferenceCount = 0;

		/// <summary>
		/// 참조 카운터 증가/감소
		/// </summary>
		inline void AddReferenceCount() noexcept { ++m_ReferenceCount; }
		inline void SubReferenceCount() noexcept { --m_ReferenceCount; }

		friend ResourceManager;
	};
}