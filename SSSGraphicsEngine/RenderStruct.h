/// <summary>
/// 그래픽스 엔진 전반에 사용할 구조체들을 정의한 클래스
/// 
/// Created : 2022/06/29 심승수
/// Updated : 2022/06/29 심승수
/// </summary>
#pragma once

struct ScreenInfo
{
	int m_ScreenWidth;
	int m_ScreenHeight;
	bool m_Enable4xMsaa;	// 4X MSAA 사용 플래그
	UINT m_4xMsaaQuality;	// 4X MSAA 품질 수준 체크
};

