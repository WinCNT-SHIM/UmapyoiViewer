#include "ClientPCH.h"
#include "TestScene.h"

// todo: test용

TestScene::TestScene()
	: SceneBase("Test")
{

}

TestScene::~TestScene()
{
}

void TestScene::BuildScene()
{
	// 메인 카메라 셋팅
	std::shared_ptr<GameObject> testCam = CreateGameObject();
	auto testCamComp = testCam->AddComponent<Camera>();
	testCamComp->SetNearZ(0.1f);
	testCamComp->SetFarZ(2000.f);
	SetMainCamera(testCamComp);
	//testCam->AddComponent<BasicCameraController>();
	testCam->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, 5.0f, -20.0f }, Space::WORLD);

	// 리소스 로드
	// Texture
	//ResourceManager::GetInstance()->LoadResource_Texture("Resource/Textures/tex_chr1013_00_hair_diff.dds");
	// ASE
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/box.ASE");
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/genji_max.ASE");
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/03IK-Joe.ASE");
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/Cylinder.ASE");
	
	// UmaMusume
	// Stage
	ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/pfb_env_live10102.ase");
	// Test용
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/pfb_bdy1013_00.ase");
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/pfb_chr1013_00.ase");
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/anm_liv_son1001_1st.ase");

	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/pfb_bdy1006_46.ase");
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/anm_liv_son1001_2nd.ase");

	// 추가할 오브젝트들 구성
	// Axis
	std::shared_ptr<GameObject> m_Axis = CreateGameObject("m_Axis");
	auto m_AxisComp = m_Axis->AddComponent<MeshRenderer>();
	m_AxisComp->m_RenderingData->m_ObjectInfo->m_MeshID = ResourceManager::GetInstance()->GetMeshID("Axis");
	m_AxisComp->m_RenderingData->m_ObjectInfo->m_UsingLighting = false;
	m_AxisComp->m_RenderingData->m_ShaderInfo->m_TechName = _T("PosColor");
	//ReleaseGameObject(m_Axis);

	// Grid
	std::shared_ptr<GameObject> m_Grid = CreateGameObject("m_Grid");
	auto m_GridComp = m_Axis->AddComponent<MeshRenderer>();
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


	//// 텍스트 박스
	//std::shared_ptr<GameObject> TestGameObj = SceneBase::CreateGameObject("TestGameObj");
	//TestGameObj->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 1.0f, 1.0f, 3.0f }, Space::WORLD);
	//auto m_TestGameObjComp = TestGameObj->AddComponent<MeshRenderer>();
	//m_TestGameObjComp->m_RenderingData->m_MaterialInfo->m_DiffuseMapID = ResourceManager::GetInstance()->GetTextureID("Resource/Textures/tex_chr1013_00_hair_diff.dds");
	////m_TestGameObjComp->m_RenderingData->m_ObjectInfo->m_UsingLighting = true;
	////m_TestGameObjComp->m_RenderingData->m_ShaderInfo->m_TechName = _T("PosColor");
	//m_TestGameObjComp->m_RenderingData->m_ShaderInfo->m_TechName = _T("Basic");


	//// ASE 박스
	//std::shared_ptr<GameObject> TestASEBox = CreateGameObjectFromPrefab("Resource/ASEFile/box.ASE");
	//TestASEBox->GetComponent<Transform>()->SetPosition(XMFLOAT3{ -1.0f, 0.0f, -3.0f }, Space::WORLD);
	//TestASEBox->GetComponent<Transform>()->SetRotation(XMFLOAT3{ XM_PI / 5.4, XM_PI / 5.4, XM_PI / 5.4 }, Space::WORLD);


	//// 겐지
	//std::shared_ptr<GameObject> TestGenji = CreateGameObjectFromPrefab("Resource/ASEFile/genji_max.ASE");
	////TestGenji->GetComponent<Transform>()->SetPosition(XMFLOAT3{ 0.0f, 0.0f, 10.0f }, Space::WORLD);
	////TestGenji->GetComponent<Transform>()->SetRotation(XMFLOAT3{ XM_PI / 2, 0, 0 }, Space::LOCAL);
	//SimpleMath::Quaternion tempQuat = { 0.707107, 0, 0, 0.707107 };
	//TestGenji->GetComponent<Transform>()->SetRotationQuat(tempQuat, Space::LOCAL);
	////TestGenji->GetComponent<Transform>()->SetRotation(XMFLOAT3{ 0, XM_PI, XM_PI });


	//// IK-Joe
	//std::shared_ptr<GameObject> TestIKJoe = CreateGameObjectFromPrefab("Resource/ASEFile/03IK-Joe.ASE");
	//TestIKJoe->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });
	//TestIKJoe->GetComponent<Transform>()->SetPosition(XMFLOAT3{ -1.0f, 0.0f, 3.0f }, Space::WORLD);

	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/pfb_bdy1013_00.ase");
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/pfb_chr1013_00.ase");
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/anm_liv_son1001_1st.ase");
	//ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/pfb_env_live10102.ase");

	// Cylinder
	//std::shared_ptr<GameObject> TestCylinder = CreateGameObjectFromPrefab("Resource/ASEFile/Cylinder.ASE");
	//TestCylinder->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.1f, 0.1f, 0.1f });

	// UmaMusume
	// UmaMusume Animation Test
	//std::shared_ptr<GameObject> TestUmaMusumeAni = CreateGameObjectFromPrefab("Resource/ASEFile/anm_liv_son1001_1st.ase");
	//TestUmaMusumeAni->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.1f, 0.1f, 0.1f });

	// UmaMusume Body Test
	//std::shared_ptr<GameObject> TestUmaMusumeBody = CreateGameObjectFromPrefab("Resource/ASEFile/pfb_bdy1013_00.ase");
	//TestUmaMusumeBody->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });
	//TestUmaMusumeBody->GetTransform()->SetPosition(XMFLOAT3{ -50.0f, 0.0f, 50.0f }, Space::WORLD);

	//// UmaMusume Head Test
	//std::shared_ptr<GameObject> TestUmaMusumeHead = CreateGameObjectFromPrefab("Resource/ASEFile/pfb_chr1013_00.ase");
	//TestUmaMusumeHead->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });
	//TestUmaMusumeHead->GetTransform()->SetPosition(XMFLOAT3{ -50.0f, 0.0f, 50.0f }, Space::WORLD);

	//// UmaMusume Assemble
	//// Stage
	//m_Stage = CreateGameObjectFromPrefab("Resource/ASEFile/pfb_env_live10102.ase");
	////TestStage->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.0254f, 0.0254f, 0.0254f });
	//m_Stage->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.0254f, 0.0254f, 0.057f });

	//m_UmaMusume1st = CreateUmaMusume("1003", "00", "1001", "1st");
	//m_UmaMusume1st->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });

	//m_UmaMusume2nd = CreateUmaMusume("1013", "00", "1001", "2nd");
	//m_UmaMusume2nd->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });

	//m_UmaMusume3rd = CreateUmaMusume("1013", "00", "1001", "3rd");
	//m_UmaMusume3rd->GetComponent<Transform>()->SetScale(XMFLOAT3{ 0.01f, 0.01f, 0.01f });


	ResourceManager::GetInstance()->LoadResource_ASEFile("Resource/ASEFile/pfb_chr1003_00_Eyes.ase");
	m_Stage = CreateGameObjectFromPrefab("Resource/ASEFile/pfb_chr1003_00_Eyes.ase");

	
	//testCam->SetParent(TestUmaMusume);




	// ...
	// ...
}

void TestScene::Init()
{
	BuildScene();
	SceneBase::Init();
}

void TestScene::Release()
{
	SceneBase::Release();
}
