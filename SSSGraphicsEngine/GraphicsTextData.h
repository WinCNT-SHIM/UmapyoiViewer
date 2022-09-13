#pragma once

#include <string>

#ifdef _WINDOWS
#include <DirectXMath.h>
#include "../Inc/SimpleMath.h"
using namespace DirectX;
#endif

struct TextData
{
	TextData()
		: m_FontID{ 7 }
		, m_StartCoord{ 10.f, 10.f }
		, m_Color{ 0.3f, 0.8f, 0.8f, 1.f }
		, m_OutText{ "TextData" }
	{

	}

	unsigned int m_FontID;
	XMFLOAT2 m_StartCoord;
	XMFLOAT4 m_Color;
	std::string m_OutText;
};