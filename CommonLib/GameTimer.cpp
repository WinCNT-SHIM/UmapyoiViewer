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
	// 생성자에서는 성능 카운터의 주파수(초당 개수)를 조회해서, 개수당 초 수를 설정한다
	// 성능 카운터의 주파수(초당 개수)을 변수에 저장한다
	__int64 _CountsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*) &_CountsPerSecond);
	// 초당 개수의 역수로 "개수당 초"를 구한다
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
	// 일지 정시 상태라면 중단된 후의 시간은 계산하지 않는다
	// 또한 이전에 일시 정시된 시간이 있으면 그 시간도 제외한다
	if (m_Stopped)
	{
		// (일시 정시된 시간 - 이전 일시 정지된 시간) - 게임 시작 시간
		return (float)(((m_StopTime - m_PauseTime) - m_BaseTime) * m_SecondsPerCount);
	}
	// 현재 시간에서 게임 시작 시간을 제외하고,
	// 일시 정지되었던 시간도 제외한다
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
	// 애니메이션의 첫 프레임에는 이전 프레임이라는 것이 없으므로
	// 이전 시간 값도 현재 시간으로 초기화한다
	m_PrevTime = _CurrTime;
	m_StopTime = 0;
	m_Stopped = false;
}

void GameTimer::Start()
{
	__int64 _StartTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&_StartTime);

	// 중단과 시작(재개) 사이에 흐른 시간을 누적한다
	//
	//      |<----d---->|
	// -----*-----------*----------> 시간
	//  StopTime   StartTime

	if (m_Stopped)
	{
		// 일시 정지 시간을 누적한다
		m_PauseTime += (_StartTime - m_StopTime);

		// 이전 프레임의 시간 값을 현재 시간으로 재설정한다
		// 타이머를 다시 시작하는 것이므로 이전 시간의 값은
		// 일시 도중에 갱신된 값이기 때문에 유효하지 않다
		m_PrevTime = _StartTime;
		// 중단 상태를 해제한다
		m_StopTime = 0;
		m_Stopped = false;
	}
}

void GameTimer::Stop()
{
	// 정지 상태가 아닐 때
	if (!m_Stopped)
	{
		__int64 _currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&_currTime);

		// 일시 정지된 시점의 시간을 저장하고, 플래그를 설정한다
		m_StopTime = _currTime;
		m_Stopped = true;
	}
}

void GameTimer::Tick()
{
	// 일시 정시 시, 델타 타임은 0
	if (m_Stopped)
	{
		m_DeltaTime = 0;
		return;
	}

	// 현재 프레임의 시간을 얻는다
	__int64 _currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&_currTime);
	m_CurrTime = _currTime;

	// 현재 프레임의 시간과 이전 프레임의 시간의 차이를 구한다
	// (현재 프레임의 시간 개수 - 이번 프레임의 시간 개수) * 개수당 초
	m_DeltaTime = (m_CurrTime - m_PrevTime) * m_SecondsPerCount;

	// 다음 프레임을 준비한다
	m_PrevTime = m_CurrTime;

	// 절전 모드나 다른 프로세서와 엉키는 경우,
	// m_DeltaTime가 0이 될 수 있으므로 음수가 되지 않도록 한다
	if (m_DeltaTime < 0.0)
	{
		m_DeltaTime = 0.0;
	}
}
