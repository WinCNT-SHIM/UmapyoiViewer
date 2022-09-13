#pragma once
/// <summary>
/// 미리 컴파일된 헤더(PCH)
/// 
/// 추가 방법
/// 1. stdafx.h, stdafx.cpp 추가
/// 2. 프로젝트 - 속성 - C/C++ - 미리 컴파일된 헤더 - 미리 컴파일된 헤더 "사용" 선택
/// 3. stdafx.cpp - 속성 - C/C++ - 미리 컴파일된 헤더 - 미리 컴파일된 헤더 "만들기" 선택
/// 4. stdafx.cpp에 #include "stdafx.h" 추가
/// 5. stdafx.h에 필요한 헤더들을 기술
/// 
/// 변경이 적고 빈번하게 사용되는 헤더만 기술할 것!
/// </summary>
#ifndef PCH_H
#define PCH_H

// 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// 문자열 관련
#include <tchar.h>
#include <basetsd.h>
#include <xstring>
#include <stringapiset.h>

#include <stdlib.h>

#include <fstream>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <unordered_map>
#include <queue>
#include <stack>
#include <memory>
#include <algorithm>
#include <filesystem>

//#include <d3d11.h>
//#include <wrl.h> // ComPtr
//using namespace Microsoft::WRL;

#include <DirectXMath.h>
using namespace DirectX;

// Core
#include "RenderStruct.h"
#include "DXDevice.h"
#include "SwapChain.h"

// Manager
#include "ResourceManager.h"

// Vertext
#include "VertexStruct.h"

// Parser
#include "../ParserLib/CASEParser.h"

#endif //PCH_H
