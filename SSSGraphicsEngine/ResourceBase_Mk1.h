/// <summary>
/// ���ҽ��� Base Class
/// ���� �ϴ� ���� ����
/// ���߿� ���ҽ� �Ŵ������� ���������� ������ �� �� ���� ����?
/// 
/// Created : 2022/07/06 �ɽ¼�
/// Updated : 2022/07/06 �ɽ¼�
/// </summary>
#pragma once

namespace GraphicsEngine
{
	class ResourceBase_Mk1
	{
	protected:
		unsigned int m_ReferenceCount = 0;

		/// <summary>
		/// ���� ī���� ����/����
		/// </summary>
		inline void AddReferenceCount() noexcept { ++m_ReferenceCount; }
		inline void SubReferenceCount() noexcept { --m_ReferenceCount; }

		friend ResourceManager;
	};
}