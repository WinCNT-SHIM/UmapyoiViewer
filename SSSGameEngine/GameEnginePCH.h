#pragma once
/// <summary>
/// �̸� �����ϵ� ���(PCH)
/// 
/// �߰� ���
/// 1. stdafx.h, stdafx.cpp �߰�
/// 2. ������Ʈ - �Ӽ� - C/C++ - �̸� �����ϵ� ��� - �̸� �����ϵ� ��� "���" ����
/// 3. stdafx.cpp - �Ӽ� - C/C++ - �̸� �����ϵ� ��� - �̸� �����ϵ� ��� "�����" ����
/// 4. stdafx.cpp�� #include "stdafx.h" �߰�
/// 5. stdafx.h�� �ʿ��� ������� ���
/// 
/// ������ ���� ����ϰ� ���Ǵ� ����� ����� ��!
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

// ���� ����
#include "..\SoundLib\InstanceGetter.h"		// �� �� ������ SoundLib.Lib (fmod.dll ���Ӽ� ����)
#include "..\SoundLib\ISoundManager.h"		// �� �� ������ SoundLib.Lib (fmod.dll ���Ӽ� ����)

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