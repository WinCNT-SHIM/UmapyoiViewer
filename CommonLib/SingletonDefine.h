/// <summary>
/// �̱���ȭ ��ų Ŭ������ ���� #define
/// ��������, ����, �������� ������ ����
/// 
/// 2022. 04. 16 �ɽ¼�
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
	