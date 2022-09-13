#pragma once

#include "ClientPCH.h"

#include "UmapyoiScene.h"
#include "HonnoSpeedScene.h"
#include "WinningTheSoulScene.h"
#include "YumeWoKakeruScene.h"
#include "TestScene.h"

//#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)

#pragma region 메모리 누수를 잡기 위한 코드들
#ifdef _DEBUG
// 일반적인 메모리 누수
#include <crtdbg.h>
//#define MAX_LOADSTRING 100

// DX의 메모리 누수
// https://develop-dream.tistory.com/141
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
void MemoryLeakChek_D3DX()
{
	HMODULE dxgidebugdll = GetModuleHandle(L"dxgidebug.dll");
	decltype(&DXGIGetDebugInterface) GetDebugInterface = reinterpret_cast<decltype(&DXGIGetDebugInterface)>(GetProcAddress(dxgidebugdll, "DXGIGetDebugInterface"));

	IDXGIDebug* debug;

	GetDebugInterface(IID_PPV_ARGS(&debug));

	OutputDebugStringW(L"▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽ Direct3DX 오브젝트 ref count 메모리 누수 체크 ▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽▽\r\n");
	debug->ReportLiveObjects(DXGI_DEBUG_D3D11, DXGI_DEBUG_RLO_DETAIL);
	OutputDebugStringW(L"△△△△△△△△△△△△△△△△△△△△ 반환된지 않은 IUnknown 객체들 △△△△△△△△△△△△△△△△△△△△\r\n");

	debug->Release();
}
#endif // _DEBUG
#pragma endregion 메모리 누수를 잡기 위한 코드들

int WINAPI _tWinMain(
	_In_ HINSTANCE hInstance,				// 프로그램의 인스턴스 핸들, 실행 파일(EXE)이 메모리에 로드될 때 OS는 이 값으로 식별함
	_In_opt_ HINSTANCE hPrevInstance,	// 바로 앞에 실행된 현재 프로그램의 인스턴스 핸들(16비트 윈도우의 호환성만을 위해 존재, Win32에선 의미 없음)
	_In_ LPTSTR lpCmdLine,					// 명령행으로 입력된 프로그램 인수
	_In_ int nShowCmd						// 프로그램이 실행될 형태(최소화, 보통 모양 등)
)
{
	// 사용 안 하는 변수들의 경고 해제
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(544);
#endif // _DEBUG

// 콘솔창 띄우기(UI 대용)
#ifdef UNICODE
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#else
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#endif

	// 게임 진행 클래스
	// 내부에서 윈도 생성과 메시지 콜백을 처리한다.
	auto pGameEngine = std::make_unique<GameEngine>();
	//IGameEngineBase* pGameEngine = new GameEngine();

	pGameEngine->Init((void*)hInstance, L"Umapyoi Viewer");

	// UI 대용 입력
	std::cout << "==================== 곡 ID 리스트 ====================" << std::endl;
	std::cout << "1001 - 우마뾰이 전설" << std::endl;
	std::cout << "1027 - 본능 스피드" << std::endl;
	std::cout << "1028 - winning the soul" << std::endl;
	std::cout << "1032 - 꿈을 걸다!" << std::endl;
	std::cout << "==================== 곡 ID 리스트 ====================" << std::endl;

	bool isSelected = false;
	while (!isSelected)
	{
		std::cout << std::endl;
		std::cout << "원하는 곡의 ID를 입력해주세요: ";
		std::string stageID;
		std::cin >> stageID;

		if (stageID == "1001")
		{
			// Scene 생성(해제는 게임 엔진 종료시 함)
			UmapyoiScene* scene = new UmapyoiScene();
			scene->Init();
			isSelected = true;
			break;
		}
		else if(stageID == "1027")
		{
			// Scene 생성(해제는 게임 엔진 종료시 함)
			HonnoSpeedScene* scene = new HonnoSpeedScene();
			scene->Init();
			isSelected = true;
			break;
		}
		else if(stageID == "1028")
		{
			// Scene 생성(해제는 게임 엔진 종료시 함)
			WinningTheSoulScene* scene = new WinningTheSoulScene();
			scene->Init();
			isSelected = true;
			break;
		}
		else if(stageID == "1032")
		{
			// Scene 생성(해제는 게임 엔진 종료시 함)
			YumeWoKakeruScene* scene = new YumeWoKakeruScene();
			scene->Init();
			isSelected = true;
			break;
		}
		else
		{
			std::cout << "잘못된 입력입니다." << std::endl;
			continue;
		}
		std::cout << std::endl;
	}


	//// Scene 생성(해제는 게임 엔진 종료시 함)
	//UmapyoiScene* umapyoiScene = new UmapyoiScene();
	//umapyoiScene->Init();
	//YumeWoKakeruScene* yumeWoKakeruSceneScene = new YumeWoKakeruScene();
	//yumeWoKakeruSceneScene->Init();

	//TestScene* testScene = new TestScene();
	//testScene->Init();
	//char buffer[256] = {};
	//sprintf_s(buffer, 256, "\n\n%d\n\n", sizeof(TestScene));
	//OutputDebugStringA(buffer);


	// 게임 엔진 루프
	pGameEngine->Loop();
	// 게임 엔진 종료
	pGameEngine->Release();

	//delete pGameEngine;

#ifdef _DEBUG
	// Memory Leak Check Dummy Data
	int* a = new int[25];
#endif // _DEBUG

#ifdef _DEBUG
	// D3DX 오브젝트에 의한 메모리 누수 체크
	MemoryLeakChek_D3DX();

	// _CrtDumpMemoryLeaks는 전역 변수 확인이 안되므로 사용 안 함
	// 애초에 프로그램의 시작 지점에 _CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF ); 함수를 실행하는 것으로 대체했으므로
	// 마지막에 실행하면 안 된다
	//_CrtDumpMemoryLeaks();
#endif // _DEBUG


	return 0;
}