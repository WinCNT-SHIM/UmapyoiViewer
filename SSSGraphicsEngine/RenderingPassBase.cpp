#include "GraphicsEnginePCH.h"
#include "RenderingPassBase.h"

using namespace GraphicsEngine;

void RenderingPassBase::Init(const uint32& screenWidth, const uint32& screenHeight)
{
	m_Device = Renderer::GetInstance()->GetDevice();
	m_DeviceContext = Renderer::GetInstance()->GetDeviceContext();

	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;
}

void RenderingPassBase::OnResize(const uint32& screenWidth, const uint32& screenHeight)
{
	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;
}

void RenderingPassBase::Release()
{
	m_Device.Reset();
	m_DeviceContext.Reset();
}
