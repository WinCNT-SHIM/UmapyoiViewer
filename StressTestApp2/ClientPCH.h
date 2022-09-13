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

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <memory>	// 스마트 포인터

#include "IGameEngineBase.h"
#include "GameEngine.h"

// Game Object
#include "GameObject.h"

// Manager
#include "ResourceManager.h"

// Component
#include "ComponentBase.h"
#include "Transform.h"
#include "Camera.h"
#include "MeshRenderer.h"
#include "DirectionalLight.h"
