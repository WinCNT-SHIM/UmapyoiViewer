#pragma once

#include "GameEnginePCH.h"
#include "IGameEngineBase.h"

#include "../SSSGraphicsEngine/IGraphicsEngineBase.h"
#include "../CommonLib/GameTimer.h"
#include "../SoundLib/ISoundManager.h"

class DLL_DECLSPEC GameEngine
	: public IGameEngineBase
{
	//////////////////////////////////////////////////////////////////////////
	// ������ �� �Ҹ���
	//////////////////////////////////////////////////////////////////////////
public:
	GameEngine();
	virtual ~GameEngine();

//////////////////////////////////////////////////////////////////////////
// ����
//////////////////////////////////////////////////////////////////////////
private:
	// ������ ����
	HWND m_hWnd;
	MSG m_Msg;
	
	// �ʱ� ��ġ
	const int m_WindowPosX = 300;
	const int m_WindowPosY = 300;

	// ���� �Ͻ� ����
	static bool m_IsGamePaused;

	// ������ ������
	static int m_ScreenWidth;
	static int m_ScreenHeight;
	static bool m_IsMinimized;
	static bool m_IsMaximized;
	static bool m_IsResizing;

	// �׷��Ƚ� ����
	//unique_ptr<IGraphicsEngineBase> m_pRenderer;
	//IGraphicsEngineBase* m_GraphicsEngine;
	std::shared_ptr<IGraphicsEngineBase> m_GraphicsEngine;

	// Ÿ�̸�
	//std::unique_ptr<GameTimer> m_pTimer;
	static std::unique_ptr<GameTimer> m_pTimer;
	ISoundManager* m_SoundManager;

//////////////////////////////////////////////////////////////////////////
// �Լ�
//////////////////////////////////////////////////////////////////////////
public:
	// todo: ���߿��� new ��� Create�Լ��� ����� �� �ֵ��� ����...
	//IGameEngine* CreateGameEngine() override;
	//void DeleteGameEngine(IGameEngine* pModule) override;

	long Init(void* hInstance, const wchar_t* Caption) override;
	void Loop() override;
	void Release() override;

	inline static int GetScreenWidth() { return m_ScreenWidth; }
	inline static int GetScreenHeight() { return m_ScreenHeight; }
	//static IGraphicsEngineBase* GetGraphicsEngine() { return m_GraphicsEngine; }
private:
	// �޽��� �ڵ鷯(������ �ݹ� �Լ�)
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// Initialize �� �����
	void RecalcWindowSize();

	// Loop�� �Լ�
	void UpdateAll();
	void RenderAll();
};
