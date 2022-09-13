#pragma once

#ifdef _SSSEngine_EXPORTS
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#endif

class DLL_DECLSPEC IGameEngineBase abstract
{
public:
	IGameEngineBase() = default;
	virtual ~IGameEngineBase() = default;

	// 생성 함수, 소멸 함수
	//virtual IGameEngine * CreateGameEngine() abstract;
	//virtual void DeleteGameEngine(IGameEngine * pModule) abstract;

	// 게임 엔진 초기화
	virtual long Init(void* hInstance, const wchar_t* Caption) abstract;
	// 메인 게임 루프. 실제 게임이 진행되는 반복구문
	virtual void Loop() abstract;
	// 게임 엔진의 Finalize
	virtual void Release() abstract;
};