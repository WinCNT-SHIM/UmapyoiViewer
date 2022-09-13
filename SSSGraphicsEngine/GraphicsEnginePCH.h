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
#ifndef PCH_H
#define PCH_H

// ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// ���ڿ� ����
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
