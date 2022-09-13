#include "GameEnginePCH.h"
#include "GameEngine.h"

#include "SceneManager.h"
#include "ResourceManager.h"
#include "RenderingManager.h"

// �׷��Ƚ� ���� ����
#ifdef _DX11RENDERER
#include "../SSSGraphicsEngine/IGraphicsEngineBase.h"
#include "../SSSGraphicsEngine/DX11GraphicsEngine.h"
#endif


//////////////////////////////////////////////////////////////////////////
// ���� ����
//////////////////////////////////////////////////////////////////////////
bool GameEngine::m_IsGamePaused = false;

int GameEngine::m_ScreenWidth = 1280;
int GameEngine::m_ScreenHeight = 720;
bool GameEngine::m_IsMinimized = false;
bool GameEngine::m_IsMaximized = false;
bool GameEngine::m_IsResizing = false;

std::unique_ptr<GameTimer> GameEngine::m_pTimer = nullptr;

//////////////////////////////////////////////////////////////////////////
// ������
//////////////////////////////////////////////////////////////////////////
GameEngine::GameEngine() :
	m_hWnd(nullptr)
{
	ZeroMemory(&m_Msg, sizeof(MSG));
}

GameEngine::~GameEngine()
{
}

long GameEngine::Init(void* Instance, const wchar_t* Caption)
{
	HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(Instance);

	/// Win32 ����
	// ���� Ŭ����
	const wchar_t* szAppName = Caption;
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = this->WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = szAppName;
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);


	// ���� Ŭ���� ���
	RegisterClassExW(&wcex);

	// ���� ����
	m_hWnd = CreateWindowW(
		szAppName
		, szAppName
		, WS_OVERLAPPEDWINDOW
		, m_WindowPosX
		, m_WindowPosY
		, m_ScreenWidth + 100
		, m_ScreenHeight + 100
		, nullptr
		, nullptr
		, hInstance
		, NULL
	);

	if (!m_hWnd) return FALSE;


	// �׷��Ƚ� ���� ����
	// �׷��Ƚ� ���� �ʱ�ȭ
#ifdef _DX11RENDERER
	m_GraphicsEngine = IGraphicsEngineBase::CreateGraphicsEngine();
#endif
	m_GraphicsEngine->Init(hInstance, m_hWnd, m_ScreenWidth, m_ScreenHeight);

	//m_pDREngineInstance = m_pRenderer;

	// ������ ������ ȭ�鿡 ǥ��
	ShowWindow(m_hWnd, SW_SHOWNORMAL);
	UpdateWindow(m_hWnd);

	// Ŭ���̾�Ʈ ������ ���ϴ� ���� �ػ󵵰� �ǵ��� ���� ������ ������
	// todo: 21���� ����� �˰� �ʹ�
	RecalcWindowSize();

	// Ÿ�̸� ����
	m_pTimer = std::make_unique<GameTimer>();
	m_pTimer->Reset();

	// ���� �Ŵ��� ����
	m_SoundManager = GetSoundManager();
	m_SoundManager->Initialize();

	// Scene Manager �ʱ�ȭ
	SceneManager::GetInstance()->Init();

	return S_OK;
}

void GameEngine::Loop()
{
	while (true)
	{
		if (PeekMessage(&m_Msg, NULL, 0, 0, PM_REMOVE))
		{
			if (m_Msg.message == WM_QUIT) break;

			DispatchMessage(&m_Msg);
		}
		else
		{
			try
			{
				UpdateAll();
				RenderAll();
			}
			catch (...)
			{
				OutputDebugString(_T("Error Detected\n"));
			}
		}
	}
}

void GameEngine::Release()
{
	SceneManager::GetInstance()->Release();
	ResourceManager::GetInstance()->Release();

	//...
	//...

	// ���� ���̺귯�� ����
	m_SoundManager->Finalize();
	delete m_SoundManager;

	// �׷��Ƚ� ���� ����
	// ==> shared_ptr�� �ٲ�
	// ��� RenderingManager�� ������
	//ReleaseGraphicsEngine(m_GraphicsEngine);
	RenderingManager::GetInstance()->Release();
}

LRESULT CALLBACK GameEngine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;

	switch (message)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  

	case WM_CREATE:
		break;
	case WM_ACTIVATE:
	{
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_IsGamePaused = true;
			//m_pTimer->Stop();
		}
		else
		{
			m_IsGamePaused = false;
			//m_pTimer->Start();
		}
		return 0;
	}

		// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
	{
		//// Save the new client area dimensions.
		m_ScreenWidth = LOWORD(lParam);
		m_ScreenHeight = HIWORD(lParam);

		if (IGraphicsEngineBase::GetGraphicsEngine()->IsValidDevice())
		{
			if (wParam == SIZE_MINIMIZED)
			{
				m_IsGamePaused = true;
				m_IsMinimized = true;
				m_IsMaximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				m_IsGamePaused = false;
				m_IsMinimized = false;
				m_IsMaximized = true;
				IGraphicsEngineBase::GetGraphicsEngine()->OnResize(m_ScreenWidth, m_ScreenHeight);
			}
			else if (wParam == SIZE_RESTORED)
			{
				if (m_IsMinimized)
				{
					m_IsGamePaused = false;
					m_IsMinimized = false;

					IGraphicsEngineBase::GetGraphicsEngine()->OnResize(m_ScreenWidth, m_ScreenHeight);
				}
				else if (m_IsMaximized)
				{
					m_IsGamePaused = false;
					m_IsMaximized = false;

					IGraphicsEngineBase::GetGraphicsEngine()->OnResize(m_ScreenWidth, m_ScreenHeight);
				}
				else if (m_IsResizing)
				{
					// If user is dragging the resize bars, we do not resize 
					// the buffers here because as the user continuously 
					// drags the resize bars, a stream of WM_SIZE messages are
					// sent to the window, and it would be pointless (and slow)
					// to resize for each WM_SIZE message received from dragging
					// the resize bars.  So instead, we reset after the user is 
					// done resizing the window and releases the resize bars, which 
					// sends a WM_EXITSIZEMOVE message.
				}
				else
				{
					IGraphicsEngineBase::GetGraphicsEngine()->OnResize(m_ScreenWidth, m_ScreenHeight);
				}
			}
		}
	}
	return 0;

	// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
	{
		/*
		//mAppPaused = true;
		mResizing = true;
		//mTimer.Stop();
		*/
	}
	return 0;

	// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
	// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
	{
		/*
		//mAppPaused = false;
		mResizing = false;
		//mTimer.Start();
		m_pDREngineInstance->OnResize();
		*/
	}
	return 0;

	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//m_pDREngineInstance->OnMouseDown(/*wParam, */LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		//m_pDREngineInstance->OnMouseUp(/*wParam, */LOWORD(lParam), HIWORD(lParam));
		return 0;

	case WM_MOUSEMOVE:
		//m_pDREngineInstance->OnMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

void GameEngine::RecalcWindowSize()
{
	RECT nowRect;
	DWORD _style = (DWORD)GetWindowLong(m_hWnd, GWL_STYLE);
	DWORD _exstyle = (DWORD)GetWindowLong(m_hWnd, GWL_EXSTYLE);
	GetWindowRect(m_hWnd, &nowRect);

	RECT newRect;
	newRect.left = 0;
	newRect.top = 0;
	newRect.right = m_ScreenWidth;
	newRect.bottom = m_ScreenHeight;

	AdjustWindowRectEx(&newRect, _style, NULL, _exstyle);

	// Ŭ���̾�Ʈ �������� ���� ũ��� �� Ŀ�� �Ѵ�(�ܰ���, Ÿ��Ʋ ��)
	int _newWidth = (newRect.right - newRect.left);
	int _newHeight = (newRect.bottom - newRect.top);

	SetWindowPos(m_hWnd, HWND_NOTOPMOST, nowRect.left, nowRect.top,
		_newWidth, _newHeight, SWP_SHOWWINDOW);
}

void GameEngine::UpdateAll()
{
	//////////////////////////////////////////////////////////////////////////
	// ������ ����
	//////////////////////////////////////////////////////////////////////////
	m_pTimer->Tick();

	// �������� ī��Ʈ�ϱ� ���� ����
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;
	// �������� ī��Ʈ�Ѵ�
	frameCnt++;

	// ���� �ð��� 1�ʸ� �Ѿ��� ���, ������ ī������ �����Ѵ�
	if (m_pTimer->TotalGamePlayTime() >= 1.0f)
	{
		// �ʴ� ������
		float fps = (float)frameCnt;
		// �����Ӵ� �и���
		float mspf = 1000.0f / fps;

		// ���� ������ ������ ���� ������
		frameCnt = 0;
		timeElapsed += 1.0f;
	}

	m_SoundManager->Update(m_pTimer->DeltaTime());

	SceneManager::GetInstance()->Update(m_pTimer->DeltaTime());
}

void GameEngine::RenderAll()
{
	//m_GraphicsEngine->BeginRender();

	// Scene�� ��� ������Ʈ���� Rendering(�� �غ�)�Ѵ�
	SceneManager::GetInstance()->Render();

	// Rendering �غ� �� ������Ʈ���� �׷��Ƚ� ������ ������ �׸���
	RenderingManager::GetInstance()->Render();

	// todo: ���߿� Post Process �����ϸ� ����...
	//m_pRenderer->PostProcess();

	//m_GraphicsEngine->EndRender();
}
