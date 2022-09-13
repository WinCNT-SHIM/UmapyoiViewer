#include "ClientPCH.h"
#include "WinningTheSoulScene.h"

#include "UmaMusume.h"
#include "Animator.h"

// todo: test용

WinningTheSoulScene::WinningTheSoulScene()
	: SceneBase("WinningTheSoulScene")
{

}

WinningTheSoulScene::~WinningTheSoulScene()
{
}

void WinningTheSoulScene::BuildScene()
{
	// 메인 카메라 셋팅
	std::shared_ptr<GameObject> mainCam = CreateGameObject();
	auto testCamComp = mainCam->AddComponent<Camera>();
	testCamComp->SetNearZ(0.1f);
	testCamComp->SetFarZ(2000.f);
	mainCam->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, 3.0f, -10.0f }, Space::WORLD);
	SetMainCamera(testCamComp);


	// 리소스 로드
	// UmaMusume Stage
	ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/pfb_env_live10105.ase");


	// 게임 오브젝트
	//// Axis
	//std::shared_ptr<GameObject> m_Axis = CreateGameObject("m_Axis");
	//auto m_AxisComp = m_Axis->AddComponent<MeshRenderer>();
	//m_AxisComp->m_RenderingData->m_ObjectInfo->m_MeshID = ResourceManager::GetInstance()->GetMeshID("Axis");
	//m_AxisComp->m_RenderingData->m_ObjectInfo->m_UsingLighting = false;
	//m_AxisComp->m_RenderingData->m_ShaderInfo->m_TechName = _T("PosColor");

	// Grid
	std::shared_ptr<GameObject> m_Grid = CreateGameObject("m_Grid");
	auto m_GridComp = m_Grid->AddComponent<MeshRenderer>();
	m_GridComp->m_RenderingData->m_ObjectInfo->m_MeshID = ResourceManager::GetInstance()->GetMeshID("Grid");
	m_GridComp->m_RenderingData->m_ObjectInfo->m_UsingLighting = false;
	m_GridComp->m_RenderingData->m_ShaderInfo->m_TechName = _T("PosColor");


	// Directional Light
	//std::shared_ptr<GameObject> dirLight = CreateGameObject("dirLight");
	//const auto& dirLightComp = dirLight->AddComponent<DirectionalLight>();
	//dirLightComp->m_DirectionalLightInfo->m_Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	//dirLightComp->m_DirectionalLightInfo->m_Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	//dirLightComp->m_DirectionalLightInfo->m_Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	////dirLight->GetComponent<Transform>()->SetRotation(XMFLOAT3{ XM_PI / 5.4, XM_PI / 5.4, XM_PI / 5.4 }, Space::WORLD);

	std::shared_ptr<GameObject> dirLight2 = CreateGameObject("dirLight2");
	const auto& dirLight2Comp = dirLight2->AddComponent<DirectionalLight>();
	dirLight2Comp->m_DirectionalLightInfo->m_Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight2Comp->m_DirectionalLightInfo->m_Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight2Comp->m_DirectionalLightInfo->m_Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight2->AddComponent<DirectionalLight>();
	dirLight2->GetComponent<Transform>()->SetRotation(XMFLOAT3{ 0.f, -180.f, 0.0f }, Space::WORLD);

	//std::shared_ptr<GameObject> dirLight3 = CreateGameObject("dirLight3");
	//dirLight3->AddComponent<DirectionalLight>();
	//dirLight3->GetComponent<Transform>()->SetRotation(XMFLOAT3{ 0.f, 90.f, 0.f }, Space::WORLD);

	//std::shared_ptr<GameObject> dirLight4 = CreateGameObject("dirLight4");
	//dirLight4->AddComponent<DirectionalLight>();
	//dirLight4->GetComponent<Transform>()->SetRotation(XMFLOAT3{ 0.f, -90.f, 0.f }, Space::WORLD);

	//std::shared_ptr<GameObject> dirLight5 = CreateGameObject("dirLight5");
	//dirLight5->AddComponent<DirectionalLight>();
	//dirLight5->GetComponent<Transform>()->SetRotation(XMFLOAT3{ 0.f, -200.f, 0.0f }, Space::WORLD);

	//std::shared_ptr<GameObject> dirLight6 = CreateGameObject("dirLight6");
	//dirLight6->AddComponent<DirectionalLight>();
	//dirLight6->GetComponent<Transform>()->SetRotation(XMFLOAT3{ 70.f, 0.f, 0.0f }, Space::WORLD);

	std::shared_ptr<GameObject> dirLight7 = CreateGameObject("dirLight7");
	const auto& dirLight7Comp = dirLight7->AddComponent<DirectionalLight>();
	dirLight7Comp->m_DirectionalLightInfo->m_Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight7Comp->m_DirectionalLightInfo->m_Diffuse = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight7Comp->m_DirectionalLightInfo->m_Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight7->GetComponent<Transform>()->SetRotation(XMFLOAT3{ 0.f, 180.f, 0.0f }, Space::WORLD);


	// UmaMusume Assemble
	// Stage
	m_Stage = CreateGameObjectFromPrefab("Resource/ASEFile/pfb_env_live10105.ase");
	//m_Stage->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.02f, 0.02f, 0.02f });
	m_Stage->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.015f, 0.015f, 0.015f });
	m_Stage->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, -0.6f, 5.0f });

	m_UmaMusume1st = CreateUmaMusume(m_UmaMusumeID_1st, "00", m_LiveID, "1st");
	m_UmaMusume1st->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });
	//m_UmaMusume1st->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, 0.0f, 0.0f });

	m_UmaMusume2nd = CreateUmaMusume(m_UmaMusumeID_2nd, "00", m_LiveID, "2nd");
	m_UmaMusume2nd->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });
	//m_UmaMusume2nd->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, -0.25f, 2.00f });

	m_UmaMusume3rd = CreateUmaMusume(m_UmaMusumeID_3rd, "00", m_LiveID, "3rd");
	m_UmaMusume3rd->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });
	//m_UmaMusume3rd->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, -0.25f, 2.00f });

	// 위치 버그 하드 코딩
	// 캐릭터 모델링 위치에 따른 위치 버그 수정 하드 코딩
	if (m_UmaMusumeID_1st == "1060")
		m_UmaMusume1st->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, 0.0f, 3.4f });
	if (m_UmaMusumeID_1st == "1002")
		m_UmaMusume1st->GetComponent<Transform>()->SetPosition(XMFLOAT3{ -0.9f, 0.0f, 0.1f });
	if (m_UmaMusumeID_1st == "1003")
		m_UmaMusume1st->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.9f, 0.0f, 0.1f });

	if (m_UmaMusumeID_2nd == "1060")
		m_UmaMusume2nd->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, 0.0f, 3.4f });
	if (m_UmaMusumeID_2nd == "1002")
		m_UmaMusume2nd->GetComponent<Transform>()->SetPosition(XMFLOAT3{ -0.9f, 0.0f, 0.1f });
	if (m_UmaMusumeID_2nd == "1003")
		m_UmaMusume2nd->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.9f, 0.0f, 0.1f });

	if (m_UmaMusumeID_3rd == "1060")
		m_UmaMusume3rd->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, 0.0f, 3.4f });
	if (m_UmaMusumeID_3rd == "1002")
		m_UmaMusume3rd->GetComponent<Transform>()->SetPosition(XMFLOAT3{ -0.9f, 0.0f, 0.1f });
	if (m_UmaMusumeID_3rd == "1003")
		m_UmaMusume3rd->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.9f, 0.0f, 0.1f });


	// 댄스 위치 버그 수정 하드 코딩
	XMFLOAT3 tempLocal_1st = m_UmaMusume1st->GetComponent<Transform>()->GetLocalPosition();
	XMFLOAT3 tempLocal_2nd = m_UmaMusume2nd->GetComponent<Transform>()->GetLocalPosition();
	tempLocal_2nd.x = tempLocal_2nd.x - 2.0f;
	XMFLOAT3 tempLocal_3rd = m_UmaMusume3rd->GetComponent<Transform>()->GetLocalPosition();
	tempLocal_3rd.x = tempLocal_3rd.x + 2.0f;

	m_UmaMusume1st->GetComponent<Transform>()->SetPosition(tempLocal_1st);
	m_UmaMusume2nd->GetComponent<Transform>()->SetPosition(tempLocal_2nd);
	m_UmaMusume3rd->GetComponent<Transform>()->SetPosition(tempLocal_3rd);

	// 배경음 로드
	// snd_bgm_live_1001_oke_01_0000.wav
	m_LiveBGMFile = "snd_bgm_live_" + m_LiveID + "_oke_01_0000" + ".wav";
	m_PathLiveBGMFile = BGMPath + m_LiveBGMFile;
	ResourceManager::GetInstance()->LoadResource_Sound(m_LiveBGMFile, m_PathLiveBGMFile, 1);



	//testCam->SetParent(TestUmaMusume);

	// ...
	// ...
}

void WinningTheSoulScene::InputCharID()
{
	std::cout << std::endl;
	std::cout << "==================== 캐릭터 ID 리스트 ====================" << std::endl;
	std::cout << "1002 - 사일렌스 스즈카" << std::endl;
	std::cout << "1003 - 토카이 테이오" << std::endl;
	std::cout << "1004 - 마루젠스키" << std::endl;
	std::cout << "1006 - 오구리 캡" << std::endl;
	std::cout << "1009 - 다이와 스칼렛" << std::endl;
	std::cout << "1013 - 메지로 맥퀸" << std::endl;
	std::cout << "1017 - 심볼리 루돌프" << std::endl;
	std::cout << "1025 - 맨하탄 카페" << std::endl;
	std::cout << "1030 - 라이스 샤워" << std::endl;
	std::cout << "1037 - 에이신 플래시" << std::endl;
	std::cout << "1052 - 하루 우라라" << std::endl;
	std::cout << "1060 - 나이스 네이처" << std::endl;
	std::cout << "1065 - 다이타쿠 헬리오스" << std::endl;
	std::cout << "1061 - 킹 헤일로" << std::endl;
	std::cout << "1071 - 메지로 아르당" << std::endl;
	std::cout << "==================== 캐릭터 ID 리스트 ====================" << std::endl;

	std::cout << "배치할 자리에 캐릭터 ID를 입력해주세요" << std::endl;
	std::string inputID;

	bool isSelected = false;

	while (!isSelected)
	{
		std::cout << "센  터 자리: ";
		std::cin >> inputID;
		isSelected = CheckCharID(inputID);
	}
	m_UmaMusumeID_1st = inputID;
	isSelected = false;

	while (!isSelected)
	{
		std::cout << "왼  쪽 자리: ";
		std::cin >> inputID;
		isSelected = CheckCharID(inputID);
	}
	m_UmaMusumeID_2nd = inputID;
	isSelected = false;

	while (!isSelected)
	{
		std::cout << "오른쪽 자리: ";
		std::cin >> inputID;
		isSelected = CheckCharID(inputID);
	}
	m_UmaMusumeID_3rd = inputID;
	isSelected = false;
}

bool WinningTheSoulScene::CheckCharID(std::string charID)
{
	if (charID == "1002"
		|| charID == "1003"
		|| charID == "1004"
		|| charID == "1006"
		|| charID == "1009"
		|| charID == "1013"
		|| charID == "1017"
		|| charID == "1025"
		|| charID == "1030"
		|| charID == "1037"
		|| charID == "1052"
		|| charID == "1060"
		|| charID == "1065"
		|| charID == "1061"
		|| charID == "1071")
	{
		return true;
	}
	else
	{
		std::cout << "잘못된 입력입니다." << std::endl;
		return false;
	}
}

void WinningTheSoulScene::Init()
{
	InputCharID();

	BuildScene();
	ResourceManager::GetInstance()->Play_SoundResource(m_LiveBGMFile, IPlayMode::BGM);
	ResourceManager::GetInstance()->SetVolume_SoundResource(3.f, IPlayMode::BGM);
	SceneBase::Init();
}

void WinningTheSoulScene::Release()
{
	SceneBase::Release();
}

void WinningTheSoulScene::LateUpdate(float deltaTime)
{
	SceneBase::LateUpdate(deltaTime);

	if (m_UmaMusume1st->GetComponent<UmaMusume>()->GetUmaMusumeBody()->GetComponent<Animator>()->IsAnimationOver())
	{
		ResourceManager::GetInstance()->Stop_SoundResource(m_LiveBGMFile, IPlayMode::BGM);
		ClearScene();

		// 다시 씬 재생
		ResourceManager::GetInstance()->Init();
		BuildScene();
		ResourceManager::GetInstance()->Play_SoundResource(m_LiveBGMFile, IPlayMode::BGM);
		ResourceManager::GetInstance()->SetVolume_SoundResource(3.f, IPlayMode::BGM);
		SceneBase::Init();
	}
}
