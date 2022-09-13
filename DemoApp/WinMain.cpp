#pragma once

#include "ClientPCH.h"

#include "UmapyoiScene.h"
#include "HonnoSpeedScene.h"
#include "WinningTheSoulScene.h"
#include "YumeWoKakeruScene.h"
#include "TestScene.h"

//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#pragma region �޸� ������ ��� ���� �ڵ��
#ifdef _DEBUG
// �Ϲ����� �޸� ����
#include <crtdbg.h>
//#define MAX_LOADSTRING 100

// DX�� �޸� ����
// https://develop-dream.tistory.com/141
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
void MemoryLeakChek_D3DX()
{
	HMODULE dxgidebugdll = GetModuleHandle(L"dxgidebug.dll");
	decltype(&DXGIGetDebugInterface) GetDebugInterface = reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(dxgidebugdll, "DXGIGetDebugInterface"));

	IDXGIDebug* debug;

	GetDebugInterface(IID_PPV_ARGS(&debug));

	OutputDebugStringW(L"����������������� Direct3DX ������Ʈ ref count �޸� ���� üũ �����������������\r\n");
	debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
	OutputDebugStringW(L"��������������������� ��ȯ���� ���� IUnknown ��ü�� ���������������������\r\n");

	debug->Release();
}
#endif // _DEBUG
#pragma endregion �޸� ������ ��� ���� �ڵ��

int WINAPI _tWinMain(
	_In_ HINSTANCE hInstance,				// ���α׷��� �ν��Ͻ� �ڵ�, ���� ����(EXE)�� �޸𸮿� �ε�� �� OS�� �� ������ �ĺ���
	_In_opt_ HINSTANCE hPrevInstance,	// �ٷ� �տ� ����� ���� ���α׷��� �ν��Ͻ� �ڵ�(16��Ʈ �������� ȣȯ������ ���� ����, Win32���� �ǹ� ����)
	_In_ LPTSTR lpCmdLine,					// ��������� �Էµ� ���α׷� �μ�
	_In_ int nShowCmd						// ���α׷��� ����� ����(�ּ�ȭ, ���� ��� ��)
)
{
	// ��� �� �ϴ� �������� ��� ����
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(544);
#endif // _DEBUG

// �ܼ�â ����(UI ���)
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

	// ���� ���� Ŭ����
	// ���ο��� ���� ������ �޽��� �ݹ��� ó���Ѵ�.
	auto pGameEngine = std::make_unique<GameEngine>();
	//IGameEngineBase* pGameEngine = new GameEngine();

	pGameEngine->Init((void*)hInstance, L"Umapyoi Viewer");

	// UI ��� �Է�
	std::cout << "==================== �� ID ����Ʈ ====================" << std::endl;
	std::cout << "1001 - �츶���� ����" << std::endl;
	std::cout << "1027 - ���� ���ǵ�" << std::endl;
	std::cout << "1028 - winning the soul" << std::endl;
	std::cout << "1032 - ���� �ɴ�!" << std::endl;
	std::cout << "==================== �� ID ����Ʈ ====================" << std::endl;

	bool isSelected = false;
	while (!isSelected)
	{
		std::cout << std::endl;
		std::cout << "���ϴ� ���� ID�� �Է����ּ���: ";
		std::string stageID;
		std::cin >> stageID;

		if (stageID == "1001")
		{
			// Scene ����(������ ���� ���� ����� ��)
			UmapyoiScene* scene = new UmapyoiScene();
			scene->Init();
			isSelected = true;
			break;
		}
		else if(stageID == "1027")
		{
			// Scene ����(������ ���� ���� ����� ��)
			HonnoSpeedScene* scene = new HonnoSpeedScene();
			scene->Init();
			isSelected = true;
			break;
		}
		else if(stageID == "1028")
		{
			// Scene ����(������ ���� ���� ����� ��)
			WinningTheSoulScene* scene = new WinningTheSoulScene();
			scene->Init();
			isSelected = true;
			break;
		}
		else if(stageID == "1032")
		{
			// Scene ����(������ ���� ���� ����� ��)
			YumeWoKakeruScene* scene = new YumeWoKakeruScene();
			scene->Init();
			isSelected = true;
			break;
		}
		else
		{
			std::cout << "�߸��� �Է��Դϴ�." << std::endl;
			continue;
		}
		std::cout << std::endl;
	}


	//// Scene ����(������ ���� ���� ����� ��)
	//UmapyoiScene* umapyoiScene = new UmapyoiScene();
	//umapyoiScene->Init();
	//YumeWoKakeruScene* yumeWoKakeruSceneScene = new YumeWoKakeruScene();
	//yumeWoKakeruSceneScene->Init();

	//TestScene* testScene = new TestScene();
	//testScene->Init();
	//char buffer[256] = {};
	//sprintf_s(buffer, 256, "\n\n%d\n\n", sizeof(TestScene));
	//OutputDebugStringA(buffer);


	// ���� ���� ����
	pGameEngine->Loop();
	// ���� ���� ����
	pGameEngine->Release();

	//delete pGameEngine;

#ifdef _DEBUG
	// Memory Leak Check Dummy Data
	int* a = new int[25];
#endif // _DEBUG

#ifdef _DEBUG
	// D3DX ������Ʈ�� ���� �޸� ���� üũ
	MemoryLeakChek_D3DX();

	// _CrtDumpMemoryLeaks�� ���� ���� Ȯ���� �ȵǹǷ� ��� �� ��
	// ���ʿ� ���α׷��� ���� ������ _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); �Լ��� �����ϴ� ������ ��ü�����Ƿ�
	// �������� �����ϸ� �� �ȴ�
	//_CrtDumpMemoryLeaks();
#endif // _DEBUG


	return 0;
}