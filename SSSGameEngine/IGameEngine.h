#pragma once

#ifdef SSSGameEngine_EXPORTS
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#endif

class DLL_DECLSPEC IGameEngine abstract
{
public:
	IGameEngine() = default;
	virtual ~IGameEngine() = default;

	// ���� �Լ�, �Ҹ� �Լ�
	//virtual IGameEngine * CreateGameEngine() abstract;
	//virtual void DeleteGameEngine(IGameEngine * pModule) abstract;

	// ���� ���� �ʱ�ȭ
	virtual long Initialize(void* hInstance, const wchar_t* Caption) abstract;
	// ���� ���� ����. ���� ������ ����Ǵ� �ݺ�����
	virtual void Loop() abstract;
	// ���� ������ Finalize
	virtual void Finalize() abstract;
};