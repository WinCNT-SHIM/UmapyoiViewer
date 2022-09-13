/// <summary>
/// 렌더링 패스의 인터페이스 클래스
/// </summary>
#pragma once

#include "Effects.h"

namespace GraphicsEngine
{
	interface RenderingPassBase
	{
	public:
		virtual void Init(const uint32& screenWidth, const uint32& screenHeight);
		virtual void OnResize(const uint32& screenWidth, const uint32& screenHeight);
		virtual void Release();

	protected:
		ComPtr<ID3D11Device> m_Device;
		ComPtr<ID3D11DeviceContext> m_DeviceContext;

		uint32 m_ScreenWidth;
		uint32 m_ScreenHeight;
	};
}