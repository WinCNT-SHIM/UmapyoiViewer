#pragma once

#include "..\CommonLib\GameEngineFramework.h"

class StressTestScene :
	public SceneBase
{
//////////////////////////////////////////////////////////////////////////
// 생성자 및 소멸자
//////////////////////////////////////////////////////////////////////////
public:
	StressTestScene();
	virtual ~StressTestScene();

//////////////////////////////////////////////////////////////////////////
// 변수
//////////////////////////////////////////////////////////////////////////
private:
	std::string m_UmaMusumeID_1st = "1003";
	std::string m_UmaMusumeID_2nd = "1013";
	std::string m_UmaMusumeID_3rd = "1060";

	std::shared_ptr<GameObject> m_Stage;
	//std::shared_ptr<GameObject> m_UmaMusume1st;
	//std::shared_ptr<GameObject> m_UmaMusume2nd;
	//std::shared_ptr<GameObject> m_UmaMusume3rd;

	std::vector<std::shared_ptr<GameObject>> testUmaMusume_1st;
	std::vector<std::shared_ptr<GameObject>> testUmaMusume_2nd;
	std::vector<std::shared_ptr<GameObject>> testUmaMusume_3rd;

	const std::string BGMPath = "Resource/Sound/";
	const std::string m_LiveID = "1032";
	std::string m_LiveBGMFile = "";
	std::string m_PathLiveBGMFile = "";

//////////////////////////////////////////////////////////////////////////
// 함수
//////////////////////////////////////////////////////////////////////////
public:
	void BuildScene() override;

public:
	virtual void Init() override final;
	virtual void Release() override final;
	virtual void LateUpdate(float deltaTime);
};
