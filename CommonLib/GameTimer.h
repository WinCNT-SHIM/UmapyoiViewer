#pragma once
#include <windows.h> // performance timer(counter) 사용

/// <summary>
/// Windows가 제공하는 성능 타이머(Performance Timer)를 사용해 시간 측정을 하는 클래스
/// 성능 타이머는 시간을 개수(Count)단위로 정밀한 시간 측정한다
/// 
/// </summary>
class GameTimer
{
// 생성자 및 소멸자
public:
	GameTimer();
	virtual ~GameTimer();

// 함수
public:
	/// <summary>
	/// 프레임 사이의 경과 시간(델타 타임)을 반환하는 함수
	/// </summary>
	/// <returns></returns>
	float DeltaTime() const;
	/// <summary>
	/// Reset 이후의 시간에서 일시 정지된 시간을 제외한 시간을 반환하는 함수
	/// </summary>
	/// <returns></returns>
	float TotalGamePlayTime() const;

	// 메시지 루프 이전에 호출하는 함수
	void Reset();
	// 타이머 시작, 재개 시 호출하는 함수
	void Start();
	/// <summary>
	/// 일시 정시 시 호출하는 함수
	/// </summary>
	void Stop();
	/// <summary>
	/// 매 프레임마다 호출하는 함수
	/// 이전 프레임과 현재 프레임 사이의 경과 시간(델타 타임)을 구한다
	/// </summary>
	void Tick();

// 변수
private:
	// 초 단위 시간
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

