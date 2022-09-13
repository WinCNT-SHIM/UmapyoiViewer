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

	// ���� �Լ�, �Ҹ� �Լ�
	//virtual IGameEngine * CreateGameEngine() abstract;
	//virtual void DeleteGameEngine(IGameEngine * pModule) abstract;

	// ���� ���� �ʱ�ȭ
	virtual long Init(void* hInstance, const wchar_t* Caption) abstract;
	// ���� ���� ����. ���� ������ ����Ǵ� �ݺ�����
	virtual void Loop() abstract;
	// ���� ������ Finalize
	virtual void Release() abstract;
};