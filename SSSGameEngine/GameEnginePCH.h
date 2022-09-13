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

#include <windows.h>
#include <tchar.h>

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
#include <stack>
#include <memory>
#include <type_traits>
#include <filesystem>

#include "GameObject.h"
#include "ComponentBase.h"

#ifdef _WINDOWS
#include <DirectXMath.h>
#include "../Inc/SimpleMath.h"
//using namespace DirectX;
#endif

// 사운드 관련
#include "..\SoundLib\InstanceGetter.h"		// 둘 다 구현은 SoundLib.Lib (fmod.dll 종속성 있음)
#include "..\SoundLib\ISoundManager.h"		// 둘 다 구현은 SoundLib.Lib (fmod.dll 종속성 있음)

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

#if defined(UNICODE)
using tstring = std::wstring;
#else
using tstring = std::string;
#endif