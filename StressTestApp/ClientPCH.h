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

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <memory>	// ����Ʈ ������

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
