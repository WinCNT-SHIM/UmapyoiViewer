/// <summary>
/// 싱글톤화 시킬 클래스를 위한 #define
/// 지훈이형, 영래, 혜성이의 조언을 받음
/// 
/// 2022. 04. 16 심승수
/// </summary>
#pragma once
#define DeclareSingleton(className) \
\
private:\
	className() {}\
	className(const className& ref) {}\
	className& operator=(const className& ref) {}\
	~className() {}\
\
public:\
	static className* GetInstance()\
	{\
		static className m_Instace;\
		return &m_Instace;\
	}\
	