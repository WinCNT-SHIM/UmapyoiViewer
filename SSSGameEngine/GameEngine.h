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
	// 생성자 및 소멸자
	//////////////////////////////////////////////////////////////////////////
public:
	GameEngine();
	virtual ~GameEngine();

//////////////////////////////////////////////////////////////////////////
// 변수
//////////////////////////////////////////////////////////////////////////
private:
	// 윈도우 관련
	HWND m_hWnd;
	MSG m_Msg;
	
	// 초기 위치
	const int m_WindowPosX = 300;
	const int m_WindowPosY = 300;

	// 게임 일시 정지
	static bool m_IsGamePaused;

	// 윈도우 사이즈
	static int m_ScreenWidth;
	static int m_ScreenHeight;
	static bool m_IsMinimized;
	static bool m_IsMaximized;
	static bool m_IsResizing;

	// 그래픽스 엔진
	//unique_ptr<IGraphicsEngineBase> m_pRenderer;
	//IGraphicsEngineBase* m_GraphicsEngine;
	std::shared_ptr<IGraphicsEngineBase> m_GraphicsEngine;

	// 타이머
	//std::unique_ptr<GameTimer> m_pTimer;
	static std::unique_ptr<GameTimer> m_pTimer;
	ISoundManager* m_SoundManager;

//////////////////////////////////////////////////////////////////////////
// 함수
//////////////////////////////////////////////////////////////////////////
public:
	// todo: 나중에는 new 대신 Create함수를 사용할 수 있도록 하자...
	//IGameEngine* CreateGameEngine() override;
	//void DeleteGameEngine(IGameEngine* pModule) override;

	long Init(void* hInstance, const wchar_t* Caption) override;
	void Loop() override;
	void Release() override;

	inline static int GetScreenWidth() { return m_ScreenWidth; }
	inline static int GetScreenHeight() { return m_ScreenHeight; }
	//static IGraphicsEngineBase* GetGraphicsEngine() { return m_GraphicsEngine; }
private:
	// 메시지 핸들러(윈도우 콜백 함수)
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	// Initialize 때 사용함
	void RecalcWindowSize();

	// Loop의 함수
	void UpdateAll();
	void RenderAll();
};
