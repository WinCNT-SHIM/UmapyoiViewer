#include "ClientPCH.h"
#include "StressTestScene.h"

#include "UmaMusume.h"
#include "Animator.h"

StressTestScene::StressTestScene()
	: SceneBase("StressTestScene")
{

}

StressTestScene::~StressTestScene()
{
}

void StressTestScene::BuildScene()
{
	// 메인 카메라 셋팅
	std::shared_ptr<GameObject> testCam = CreateGameObject();
	auto testCamComp = testCam->AddComponent<Camera>();
	testCamComp->SetNearZ(0.1f);
	testCamComp->SetFarZ(2000.f);
	SetMainCamera(testCamComp);
	testCam->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, 5.0f, -20.0f }, Space::WORLD);


	//// Axis
	//std::shared_ptr<GameObject> m_Axis = CreateGameObject("m_Axis");
	//auto m_AxisComp = m_Axis->AddComponent<MeshRenderer>();
	//m_AxisComp->m_RenderingData->m_ObjectInfo->m_MeshID = ResourceManager::GetInstance()->GetMeshID("Axis");
	//m_AxisComp->m_RenderingData->m_ObjectInfo->m_UsingLighting = false;
	//m_AxisComp->m_RenderingData->m_ShaderInfo->m_TechName = _T("PosColor");
	////ReleaseGameObject(m_Axis);

	//// Grid
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


	// 배경음 로드
	// snd_bgm_live_1001_oke_01_0000.wav
	m_LiveBGMFile = "snd_bgm_live_" + m_LiveID + "_oke_01_0000" + ".wav";
	m_PathLiveBGMFile = BGMPath + m_LiveBGMFile;
	ResourceManager::GetInstance()->LoadResource_Sound(m_LiveBGMFile, m_PathLiveBGMFile, 1);

	int charaCnt = 15;
	for (int i = 0; i < charaCnt; i++)
	{
		float offset = (float)charaCnt * 0.5;
		std::shared_ptr<GameObject> temp = CreateUmaMusume(m_UmaMusumeID_1st, "00", m_LiveID, "1st");
		temp->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });
		temp->GetComponent<Transform>()->SetPosition(XMFLOAT3{ i - offset, 0.0f, -3.0f }, Space::WORLD);
		testUmaMusume_1st.emplace_back(temp);

		temp = CreateUmaMusume(m_UmaMusumeID_2nd, "00", m_LiveID, "1st");
		temp->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });
		temp->GetComponent<Transform>()->SetPosition(XMFLOAT3{ i - offset, 0.0f, 0.0f }, Space::WORLD);
		testUmaMusume_2nd.emplace_back(temp);

		temp = CreateUmaMusume(m_UmaMusumeID_3rd, "00", m_LiveID, "1st");
		temp->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });
		temp->GetComponent<Transform>()->SetPosition(XMFLOAT3{ i - offset, 0.0f, 3.0f }, Space::WORLD);
		testUmaMusume_3rd.emplace_back(temp);
	}

	//m_UmaMusume1st = CreateUmaMusume(m_UmaMusumeID_1st, "00", m_LiveID, "1st");
	//m_UmaMusume1st->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });

	//m_UmaMusume2nd = CreateUmaMusume(m_UmaMusumeID_2nd, "00", m_LiveID, "2nd");
	//m_UmaMusume2nd->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });

	//m_UmaMusume3rd = CreateUmaMusume(m_UmaMusumeID_3rd, "00", m_LiveID, "3rd");
	//m_UmaMusume3rd->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });
}

void StressTestScene::Init()
{
	BuildScene();
	ResourceManager::GetInstance()->Play_SoundResource(m_LiveBGMFile, IPlayMode::BGM);
	ResourceManager::GetInstance()->SetVolume_SoundResource(3.f, IPlayMode::BGM);
	SceneBase::Init();
}

void StressTestScene::Release()
{
	SceneBase::Release();
}

void StressTestScene::LateUpdate(float deltaTime)
{
	SceneBase::LateUpdate(deltaTime);

	if (testUmaMusume_1st[0]->GetComponent<UmaMusume>()->GetUmaMusumeBody()->GetComponent<Animator>()->IsAnimationOver())
	{
		ResourceManager::GetInstance()->Stop_SoundResource(m_LiveBGMFile, IPlayMode::BGM);
		ClearScene();

		testUmaMusume_1st.clear();
		testUmaMusume_2nd.clear();
		testUmaMusume_3rd.clear();

		// 다시 씬 재생
		ResourceManager::GetInstance()->Init();
		BuildScene();
		ResourceManager::GetInstance()->Play_SoundResource(m_LiveBGMFile, IPlayMode::BGM);
		//ResourceManager::GetInstance()->SetVolume_SoundResource(3.f, IPlayMode::BGM);
		SceneBase::Init();
	}
}
