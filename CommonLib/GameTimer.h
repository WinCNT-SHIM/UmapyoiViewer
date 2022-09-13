#pragma once
#include <windows.h> // performance timer(counter) ���

/// <summary>
/// Windows�� �����ϴ� ���� Ÿ�̸�(Performance Timer)�� ����� �ð� ������ �ϴ� Ŭ����
/// ���� Ÿ�̸Ӵ� �ð��� ����(Count)������ ������ �ð� �����Ѵ�
/// 
/// </summary>
class GameTimer
{
// ������ �� �Ҹ���
public:
	GameTimer();
	virtual ~GameTimer();

// �Լ�
public:
	/// <summary>
	/// ������ ������ ��� �ð�(��Ÿ Ÿ��)�� ��ȯ�ϴ� �Լ�
	/// </summary>
	/// <returns></returns>
	float DeltaTime() const;
	/// <summary>
	/// Reset ������ �ð����� �Ͻ� ������ �ð��� ������ �ð��� ��ȯ�ϴ� �Լ�
	/// </summary>
	/// <returns></returns>
	float TotalGamePlayTime() const;

	// �޽��� ���� ������ ȣ���ϴ� �Լ�
	void Reset();
	// Ÿ�̸� ����, �簳 �� ȣ���ϴ� �Լ�
	void Start();
	/// <summary>
	/// �Ͻ� ���� �� ȣ���ϴ� �Լ�
	/// </summary>
	void Stop();
	/// <summary>
	/// �� �����Ӹ��� ȣ���ϴ� �Լ�
	/// ���� �����Ӱ� ���� ������ ������ ��� �ð�(��Ÿ Ÿ��)�� ���Ѵ�
	/// </summary>
	void Tick();

// ����
private:
	// �� ���� �ð�
	double m_SecondsPerCount;
	// 
	double m_DeltaTime;

	__int64 m_BaseTime;
	__int64 m_PauseTime;
	__int64 m_StopTime;
	__int64 m_PrevTime;
	__int64 m_CurrTime;

	bool m_Stopped;
};

