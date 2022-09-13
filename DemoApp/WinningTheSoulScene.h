#pragma once

#include "..\CommonLib\GameEngineFramework.h"

class WinningTheSoulScene :
	public SceneBase
{
//////////////////////////////////////////////////////////////////////////
// 생성자 및 소멸자
//////////////////////////////////////////////////////////////////////////
public:
	WinningTheSoulScene();
	virtual ~WinningTheSoulScene();

//////////////////////////////////////////////////////////////////////////
// 변수
//////////////////////////////////////////////////////////////////////////
private:
	std::string m_UmaMusumeID_1st;
	std::string m_UmaMusumeID_2nd;
	std::string m_UmaMusumeID_3rd;

	std::shared_ptr<GameObject> m_Stage;
	std::shared_ptr<GameObject> m_UmaMusume1st;
	std::shared_ptr<GameObject> m_UmaMusume2nd;
	std::shared_ptr<GameObject> m_UmaMusume3rd;

	const std::string BGMPath = "Resource/Sound/";
	const std::string m_LiveID = "1028";
	std::string m_LiveBGMFile = "";
	std::string m_PathLiveBGMFile = "";

//////////////////////////////////////////////////////////////////////////
// 함수
//////////////////////////////////////////////////////////////////////////
public:
	void BuildScene() override;

private:
	void InputCharID();
	bool CheckCharID(std::string charID);

public:
	virtual void Init() override final;
	virtual void Release() override final;
	virtual void LateUpdate(float deltaTime);
};
