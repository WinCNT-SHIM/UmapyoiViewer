#include "GameTimer.h"

GameTimer::GameTimer() :
	m_SecondsPerCount(0.0),
	m_DeltaTime(-1.0),
	m_BaseTime(0),
	m_PauseTime(0),
	m_PrevTime(0),
	m_CurrTime(0),
	m_StopTime(false)
{
	// �����ڿ����� ���� ī������ ���ļ�(�ʴ� ����)�� ��ȸ�ؼ�, ������ �� ���� �����Ѵ�
	// ���� ī������ ���ļ�(�ʴ� ����)�� ������ �����Ѵ�
	__int64 _CountsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*) &_CountsPerSecond);
	// �ʴ� ������ ������ "������ ��"�� ���Ѵ�
	m_SecondsPerCount = 1.0 / (double)_CountsPerSecond;
}

GameTimer::~GameTimer()
{
}

float GameTimer::DeltaTime() const
{
	return (float)m_DeltaTime;
}

float GameTimer::TotalGamePlayTime() const
{
	// ���� ���� ���¶�� �ߴܵ� ���� �ð��� ������� �ʴ´�
	// ���� ������ �Ͻ� ���õ� �ð��� ������ �� �ð��� �����Ѵ�
	if (m_Stopped)
	{
		// (�Ͻ� ���õ� �ð� - ���� �Ͻ� ������ �ð�) - ���� ���� �ð�
		return (float)(((m_StopTime - m_PauseTime) - m_BaseTime) * m_SecondsPerCount);
	}
	// ���� �ð����� ���� ���� �ð��� �����ϰ�,
	// �Ͻ� �����Ǿ��� �ð��� �����Ѵ�
	else
	{
		return (float)(((m_CurrTime - m_PauseTime) - m_BaseTime) * m_SecondsPerCount);
	}
}

void GameTimer::Reset()
{
	__int64 _CurrTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&_CurrTime);

	m_BaseTime = _CurrTime;
	// �ִϸ��̼��� ù �����ӿ��� ���� �������̶�� ���� �����Ƿ�
	// ���� �ð� ���� ���� �ð����� �ʱ�ȭ�Ѵ�
	m_PrevTime = _CurrTime;
	m_StopTime = 0;
	m_Stopped = false;
}

void GameTimer::Start()
{
	__int64 _StartTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&_StartTime);

	// �ߴܰ� ����(�簳) ���̿� �帥 �ð��� �����Ѵ�
	//
	//      |<----d---->|
	// -----*-----------*----------> �ð�
	//  StopTime   StartTime

	if (m_Stopped)
	{
		// �Ͻ� ���� �ð��� �����Ѵ�
		m_PauseTime += (_StartTime - m_StopTime);

		// ���� �������� �ð� ���� ���� �ð����� �缳���Ѵ�
		// Ÿ�̸Ӹ� �ٽ� �����ϴ� ���̹Ƿ� ���� �ð��� ����
		// �Ͻ� ���߿� ���ŵ� ���̱� ������ ��ȿ���� �ʴ�
		m_PrevTime = _StartTime;
		// �ߴ� ���¸� �����Ѵ�
		m_StopTime = 0;
		m_Stopped = false;
	}
}

void GameTimer::Stop()
{
	// ���� ���°� �ƴ� ��
	if (!m_Stopped)
	{
		__int64 _currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&_currTime);

		// �Ͻ� ������ ������ �ð��� �����ϰ�, �÷��׸� �����Ѵ�
		m_StopTime = _currTime;
		m_Stopped = true;
	}
}

void GameTimer::Tick()
{
	// �Ͻ� ���� ��, ��Ÿ Ÿ���� 0
	if (m_Stopped)
	{
		m_DeltaTime = 0;
		return;
	}

	// ���� �������� �ð��� ��´�
	__int64 _currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&_currTime);
	m_CurrTime = _currTime;

	// ���� �������� �ð��� ���� �������� �ð��� ���̸� ���Ѵ�
	// (���� �������� �ð� ���� - �̹� �������� �ð� ����) * ������ ��
	m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;

	// ���� �������� �غ��Ѵ�
	m_PrevTime = m_CurrTime;

	// ���� ��峪 �ٸ� ���μ����� ��Ű�� ���,
	// m_DeltaTime�� 0�� �� �� �����Ƿ� ������ ���� �ʵ��� �Ѵ�
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}
