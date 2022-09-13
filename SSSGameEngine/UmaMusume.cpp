#include "GameEnginePCH.h"
#include "UmaMusume.h"

#include "SceneBase.h"
#include "ResourceManager.h"

#include "Transform.h"
#include "PrefabASE.h"
#include "MeshRenderer.h"
#include "SkinnedMeshRenderer.h"
#include "Animator.h"
#include "AnimationClip.h"

UmaMusume::UmaMusume(std::weak_ptr<GameObject> _GameObj)
	: ComponentBase(_GameObj, "UmaMusume")
{
}

UmaMusume::~UmaMusume()
{
}

void UmaMusume::Release()
{
	ResourceManager::GetInstance()->Stop_SoundResource(m_CharVocalFile, IPlayMode::Effect);
	if (m_LiveID == "1032")
		ResourceManager::GetInstance()->Stop_SoundResource(m_CharSubVocalFile, IPlayMode::Effect);

	m_UmaMusumeBody->Release();
	m_UmaMusumeHead->Release();
	m_UmaMusumeEyes->Release();
	__super::Release();

	m_UmaMusumeBody.reset();
	m_UmaMusumeHead.reset();
	m_BodyNeckGameObj.reset();
	m_BodyHeadGameObj.reset();
	m_HeadNeckGameObj.reset();
	m_HeadHeadGameObj.reset();
}

void UmaMusume::CreateUmaMusume(std::string charID, std::string charSubID, std::string liveID, std::string liveSubID)
{
	m_CharID = charID;
	m_CharSubID= charSubID;
	m_LiveID = liveID;
	m_LiveSubID = liveSubID;

	// 리소스 경로 설정
	SetResourcePath(charID, charSubID, liveID, liveSubID);
	
	// 리로스 Load!!!(ASE, Texture 등등)
	SetUmaMusumeResource();

	std::shared_ptr<GameObject> body = CreateGameObjectFromPrefab(m_PathUmaMusumeBody);
	std::shared_ptr<GameObject> head = CreateGameObjectFromPrefab(m_PathUmaMusumeHead);
	std::shared_ptr<GameObject> eyes = CreateGameObjectFromPrefab(m_PathUmaMusumeEyes);
	std::shared_ptr<GameObject> anim = CreateGameObjectFromPrefab(m_PathLiveAnimation);

	// 우마무스메 쉐이더 설정
	SetUmaMusumeShader(body);
	SetUmaMusumeShader(head);
	SetUmaMusumeShader(eyes);

	// 몸통 + 머리 + 애니메이션
	AssemblyUmaMusume(
		body
		, head
		, eyes
		, anim
	);
}

void UmaMusume::SetResourcePath(std::string charID, std::string charSubID, std::string liveID, std::string liveSubID)
{
	// 우선 필요한 리소스들을 로드하자
	// ASE 경로 설정
	const std::string ASEPath = "Resource/ASEFile/";
	m_PathUmaMusumeBody = ASEPath + "pfb_bdy" + charID + "_" + charSubID + ".ase";
	m_PathUmaMusumeHead = ASEPath + "pfb_chr" + charID + "_" + charSubID + ".ase";
	m_PathUmaMusumeEyes = ASEPath + "pfb_chr" + charID + "_" + charSubID + "_Eyes" + ".ase";
	m_PathLiveAnimation = ASEPath + "anm_liv_son" + liveID + "_" + liveSubID + ".ase";
	
	// 텍스처 경로 설정
	const std::string TXTPath = "Resource/Textures/";
	// Body 관련
	m_PathBodyBase = TXTPath + "tex_bdy" + charID + "_" + charSubID + "_base" + ".dds";
	m_PathBodyCtrl = TXTPath + "tex_bdy" + charID + "_" + charSubID + "_ctrl" + ".dds";
	m_PathBodyDiff = TXTPath + "tex_bdy" + charID + "_" + charSubID + "_diff" + ".dds";
	m_PathBodyShad_c = TXTPath + "tex_bdy" + charID + "_" + charSubID + "_shad_c" + ".dds";
	
	// Hair 관련
	m_PathHairBase = TXTPath + "tex_chr" + charID + "_" + charSubID + "_hair" + "_base" + ".dds";
	m_PathHairCtrl = TXTPath + "tex_chr" + charID + "_" + charSubID + "_hair" + "_ctrl" + ".dds";
	m_PathHairDiff = TXTPath + "tex_chr" + charID + "_" + charSubID + "_hair" + "_diff" + ".dds";
	m_PathHairShad_c= TXTPath + "tex_chr" + charID + "_" + charSubID + "_hair" + "_shad_c" + ".dds";
	
	// Face 관련
	m_PathFaceBase = TXTPath + "tex_chr" + charID + "_" + charSubID + "_face" + "_base" + ".dds";
	m_PathFaceCtrl = TXTPath + "tex_chr" + charID + "_" + charSubID + "_face" + "_ctrl" + ".dds";
	m_PathFaceDiff = TXTPath + "tex_chr" + charID + "_" + charSubID + "_face" + "_diff" + ".dds";
	m_PathFaceShad_c = TXTPath + "tex_chr" + charID + "_" + charSubID + "_face" + "_shad_c" + ".dds";
	
	// Eyes 관련
	m_PathEyes0 = TXTPath + "tex_chr" + charID + "_" + charSubID + "_eye0" + ".dds";
	m_PathEyesHighlight00 = TXTPath + "tex_chr" + charID + "_" + charSubID + "_eyehi00" + ".dds";
	m_PathEyesHighlight01 = TXTPath + "tex_chr" + charID + "_" + charSubID + "_eyehi01" + ".dds";
	m_PathEyesHighlight02 = TXTPath + "tex_chr" + charID + "_" + charSubID + "_eyehi02" + ".dds";
	
	// Cheek 관련
	m_PathCheek0 = TXTPath + "tex_chr" + charID + "_" + charSubID + "_cheek0" + ".dds";


	// 사운드 경로 설정
	const std::string BGMPath = "Resource/Sound/";
	m_LiveBGMFile = "snd_bgm_live_" + liveID + "_oke_01_0000" + ".wav";
	m_PathLiveBGMFile = BGMPath + m_LiveBGMFile;
	m_CharVocalFile = "snd_bgm_live_" + liveID + "_chara_" + charID + "_01" + "_0000" + ".wav";
	m_PathCharVocalFile = BGMPath + m_CharVocalFile;

	m_CharSubVocalFile = "snd_bgm_live_" + liveID + "_chara_" + charID + "_01" + "_0001" + ".wav";
	m_PathCharSubVocalFile = BGMPath + m_CharSubVocalFile;
}

void UmaMusume::SetUmaMusumeResource()
{
	// 리소스 로드!
	// ASE
	ResourceManager::GetInstance()->LoadResource_ASEFile(m_PathUmaMusumeBody);
	ResourceManager::GetInstance()->LoadResource_ASEFile(m_PathUmaMusumeHead);
	ResourceManager::GetInstance()->LoadResource_ASEFile(m_PathUmaMusumeEyes);
	ResourceManager::GetInstance()->LoadResource_ASEFile(m_PathLiveAnimation);
	// Texture
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathBodyBase);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathBodyCtrl);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathBodyDiff);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathBodyShad_c);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathHairBase);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathHairCtrl);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathHairDiff);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathHairShad_c);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathFaceBase);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathFaceCtrl);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathFaceDiff);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathFaceShad_c);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathEyes0);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathEyesHighlight00);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathEyesHighlight01);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathEyesHighlight02);
	ResourceManager::GetInstance()->LoadResource_Texture(m_PathCheek0);
	// 환경 맵
	ResourceManager::GetInstance()->LoadResource_Texture("Resource/Textures/EnvironmentMap.dds");

	// Sound
	bool resLoadVocal;
	resLoadVocal= ResourceManager::GetInstance()->LoadResource_Sound(m_CharVocalFile, m_PathCharVocalFile, 1);
	ResourceManager::GetInstance()->LoadResource_Sound(m_CharSubVocalFile, m_PathCharSubVocalFile, 1);

	// 보이스가 없는 경우!!
	// 다른 캐릭터의 보이스로 바꾼다
	if (!resLoadVocal)
	{
		const std::string BGMPath = "Resource/Sound/";
		std::string liveID;
		std::string charID;
		if (m_LiveID == "1001")
		{
			liveID = "1001";
			charID = "1003";
		}
		else if (m_LiveID == "1027")
		{
			liveID = "1027";
			charID = "1071";
		}
		else if (m_LiveID == "1028")
		{
			liveID = "1028";
			charID = "1017";
		}
		else if (m_LiveID == "1032")
		{
			liveID = "1032";
			charID = "1003";
		}

		m_LiveBGMFile = "snd_bgm_live_" + liveID + "_oke_01_0000" + ".wav";
		m_PathLiveBGMFile = BGMPath + m_LiveBGMFile;
		m_CharVocalFile = "snd_bgm_live_" + liveID + "_chara_" + charID + "_01" + "_0000" + ".wav";
		m_PathCharVocalFile = BGMPath + m_CharVocalFile;

		m_CharSubVocalFile = "snd_bgm_live_" + liveID + "_chara_" + charID + "_01" + "_0001" + ".wav";
		m_PathCharSubVocalFile = BGMPath + m_CharSubVocalFile;


		ResourceManager::GetInstance()->LoadResource_Sound(m_CharVocalFile, m_PathCharVocalFile, 1);
		ResourceManager::GetInstance()->LoadResource_Sound(m_CharSubVocalFile, m_PathCharSubVocalFile, 1);
	}
}

void UmaMusume::AssemblyUmaMusume(
	std::shared_ptr<GameObject> umaMusumeBody
	, std::shared_ptr<GameObject> umaMusumeHead
	, std::shared_ptr<GameObject> umaMusumeEyes
	, std::shared_ptr<GameObject> umaMusumeLiveAnim)
{
	this->GetGameObject()->SetChildren({ umaMusumeBody, umaMusumeHead, umaMusumeEyes });

	// 캐싱용
	m_UmaMusumeBody = umaMusumeBody;
	m_UmaMusumeHead = umaMusumeHead;
	m_UmaMusumeEyes = umaMusumeEyes;


	// Neck과 Head 찾기
	m_BodyNeckGameObj = m_UmaMusumeBody->FindChildInAllChildren("Neck");
	m_BodyHeadGameObj = m_UmaMusumeBody->FindChildInAllChildren("Head");

	m_HeadNeckGameObj = m_UmaMusumeHead->FindChildInAllChildren("Neck");
	m_HeadHeadGameObj = m_UmaMusumeHead->FindChildInAllChildren("Head");

	m_EyesNeckGameObj = m_UmaMusumeEyes->FindChildInAllChildren("Neck");
	m_EyesHeadGameObj = m_UmaMusumeEyes->FindChildInAllChildren("Head");


	// 눈물 모델링 끄기!
	m_UmaMusumeHead->FindChildInAllChildren("M_Tear_L")->GetComponent<MeshRenderer>()->SetIsEnable(false);
	m_UmaMusumeHead->FindChildInAllChildren("M_Tear_R")->GetComponent<MeshRenderer>()->SetIsEnable(false);


	// 텍스처 설정
	SetTexture();


	// 애니메이터 컴포넌트의 애니메이션 클립을 몸통 쪽의 애니메이터로 옮긴다
	SetAnimClip(umaMusumeLiveAnim);


	// 몸통의 본과 머리의 본을 동기화시킴
	SynchronizeBodyHead();
}

void UmaMusume::SetTexture()
{
	// 몸통에 대한 텍스처 설정
	if (m_UmaMusumeBody == nullptr)
		return;

	const auto& bodyComp =  m_UmaMusumeBody->FindChildInAllChildren("M_Body")->GetComponent<SkinnedMeshRenderer>();
	bodyComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID = ResourceManager::GetInstance()->GetTextureID(m_PathBodyDiff);
	bodyComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_TripleMaskMapID = ResourceManager::GetInstance()->GetTextureID(m_PathBodyBase);
	bodyComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_OptionMaskMapID = ResourceManager::GetInstance()->GetTextureID(m_PathBodyCtrl);
	bodyComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_ToonShadowMapID = ResourceManager::GetInstance()->GetTextureID(m_PathBodyShad_c);
	bodyComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_EnvironmentMapID = ResourceManager::GetInstance()->GetTextureID("Resource/Textures/EnvironmentMap.dds");


	// 머리에 대한 텍스처 설정
	if (m_UmaMusumeHead == nullptr)
		return;

	// Hair
	auto tempComp = m_UmaMusumeHead->FindChildInAllChildren("M_Hair")->GetComponent<SkinnedMeshRenderer>();
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID = ResourceManager::GetInstance()->GetTextureID(m_PathHairDiff);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_TripleMaskMapID = ResourceManager::GetInstance()->GetTextureID(m_PathHairBase);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_OptionMaskMapID = ResourceManager::GetInstance()->GetTextureID(m_PathHairCtrl);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_ToonShadowMapID = ResourceManager::GetInstance()->GetTextureID(m_PathHairShad_c);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_EnvironmentMapID = ResourceManager::GetInstance()->GetTextureID("Resource/Textures/EnvironmentMap.dds");

	// Face
	tempComp = m_UmaMusumeHead->FindChildInAllChildren("M_Face")->GetComponent<SkinnedMeshRenderer>();
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID = ResourceManager::GetInstance()->GetTextureID(m_PathFaceDiff);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_TripleMaskMapID = ResourceManager::GetInstance()->GetTextureID(m_PathFaceBase);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_OptionMaskMapID = ResourceManager::GetInstance()->GetTextureID(m_PathFaceCtrl);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_ToonShadowMapID = ResourceManager::GetInstance()->GetTextureID(m_PathFaceShad_c);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_EnvironmentMapID = ResourceManager::GetInstance()->GetTextureID("Resource/Textures/EnvironmentMap.dds");

	// Eyes
	tempComp = m_UmaMusumeEyes->FindChildInAllChildren("M_Face")->GetComponent<SkinnedMeshRenderer>();
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID = ResourceManager::GetInstance()->GetTextureID(m_PathEyes0);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_EyesHighlight00MapID = ResourceManager::GetInstance()->GetTextureID(m_PathEyesHighlight00);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_EyesHighlight01MapID = ResourceManager::GetInstance()->GetTextureID(m_PathEyesHighlight01);
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_EyesHighlight02MapID = ResourceManager::GetInstance()->GetTextureID(m_PathEyesHighlight02);

	// Cheek
	tempComp = m_UmaMusumeHead->FindChildInAllChildren("M_Cheek")->GetComponent<SkinnedMeshRenderer>();
	tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_DiffuseMapID = ResourceManager::GetInstance()->GetTextureID(m_PathCheek0);
	//tempComp->m_RenderingData->m_MaterialInfo->m_TextureInfo.m_EnvironmentMapID = ResourceManager::GetInstance()->GetTextureID("Resource/Textures/EnvironmentMap.dds");
}

void UmaMusume::SetAnimClip(std::shared_ptr<GameObject> umaMusumeLiveAnim)
{
	// 애니메이터 컴포넌트의 애니메이션 클립을 몸통 쪽의 애니메이터로 옮긴다
	std::shared_ptr<AnimationClip> clip = umaMusumeLiveAnim->GetComponent<Animator>()->GetAnimClip();

	// 애니메이션 클립의 하이어라키의 캐릭터 번호를 바꿔준다
	ConvertAnimClipName(clip);

	// 애니메이터 클립을 설정한다
	auto animatorComp = m_UmaMusumeBody->GetComponent<Animator>();
	animatorComp->SetAnimClip(clip);
	SetStartAnimDelayTime(clip);	// 애니메이션 딜레이 설정
	umaMusumeLiveAnim->GetComponent<Animator>()->SetAnimClip(nullptr);
	umaMusumeLiveAnim->Release();
}

void UmaMusume::ConvertAnimClipName(std::shared_ptr<AnimationClip> clip)
{
	// 애니메이션 클립의 하이어라키의 캐릭터 번호를 바꿔주는 처리
	// 데이터 추출의 프로세스 상, 최상위 부모에 캐릭터 번호가 들어가 버린다
	// 그 부분을 범용적으로 쓰기 위해 다른 캐릭터 번호로 바꾸는 처리

	// 데이터 추출에 따른 하드 코딩인 부분
	// 1st의 애니메이션은 1013(메지로 맥퀸)으로,
	// 2nd의 애니메이션은 1003(토카이 테이오)으로,
	// 3rd의 애니메이션은 1002(사이렌스 스즈카)로
	// 하기로 결정했으므로 중복되는 처리를 피하기 위한 조건문
	if (m_CharID == "1013" && m_LiveSubID == "1st")
		return;
	if (m_CharID == "1003" && m_LiveSubID == "2nd")
		return;
	if (m_CharID == "1002" && m_LiveSubID == "3rd")
		return;

	std::string targetID;
	if (m_LiveSubID == "1st")
		targetID = "1013";
	else if (m_LiveSubID == "2nd")
		targetID = "1003";
	else if (m_LiveSubID == "3rd")
		targetID = "1002";


	// 애니메이션 클립의 하이어라키의 캐릭터 번호를 바꿔준다
	for (auto& item : clip->GetPositionTrackList())
	{
		for (auto& hierarchyName : item._targetHierarchy)
		{
			if (hierarchyName.find(targetID) != std::string::npos)
			{
				hierarchyName.replace(hierarchyName.find(targetID), 4, m_CharID);
			}
		}
	}
	for (auto& item : clip->GetRotationTrackList())
	{
		for (auto& hierarchyName : item._targetHierarchy)
		{
			if (hierarchyName.find(targetID) != std::string::npos)
			{
				hierarchyName.replace(hierarchyName.find(targetID), 4, m_CharID);
			}
		}
	}
}

void UmaMusume::SetStartAnimDelayTime(std::shared_ptr<AnimationClip> animator)
{
	// 라이브와 댄스의 싱크를 위한 시작 딜레이 설정
	animator->SetStartAnimDelayTime(1.0f);
	//animator->SetStartAnimDelayTime(2.5f);

	//if (m_LiveID == "1001")
	//	animator->SetStartAnimDelayTime(1.0f);
	//	//animator->SetStartAnimDelayTime(2.5f);

	//if (m_LiveID == "1032")
	//	animator->SetStartAnimDelayTime(1.0f);
	//	//animator->SetStartAnimDelayTime(2.5f);
}

std::shared_ptr<GameObject> UmaMusume::CreateGameObjectFromPrefab(std::string prefabName)
{
	std::shared_ptr<PrefabASE> prefabASE = ResourceManager::GetInstance()->GetPrefab(prefabName);
	assert(prefabASE);
	std::shared_ptr<GameObject> newObject = prefabASE->MakeGameObject();

	return newObject;
}

void UmaMusume::SetUmaMusumeShader(std::shared_ptr<GameObject> gameObject)
{
	// 우마무스메 모델링에 쉐이더 정보를 변경한다
	// Body
	std::shared_ptr<GameObject> gameObj = gameObject->FindChildInAllChildren("M_Body");
	if (gameObj != nullptr)
	{
		auto renderingData = gameObj->GetComponent<SkinnedMeshRenderer>()->m_RenderingData;
		renderingData->m_ShaderInfo->m_TechName = TEXT("UmaMusumeHair");
		return;
	}
	
	// Hair
	gameObj = gameObject->FindChildInAllChildren("M_Hair");
	if (gameObj != nullptr)
	{
		auto renderingData = gameObj->GetComponent<SkinnedMeshRenderer>()->m_RenderingData;
		renderingData->m_ShaderInfo->m_TechName = TEXT("UmaMusumeHair");
	}
	
	// Face
	gameObj = gameObject->FindChildInAllChildren("M_Face");
	if (gameObj != nullptr)
	{
		auto renderingData = gameObj->GetComponent<SkinnedMeshRenderer>()->m_RenderingData;
		if (gameObject->GetName().find("Eyes") != std::string::npos)
		{
			// 눈 모델링인 경우
			renderingData->m_ShaderInfo->m_TechName = TEXT("UmaMusumeEyes");
			return;	// 눈 모델링과 본 밖에 없기 때문에 return함
		}
		else
		{
			// 얼굴 모델링인 경우
			renderingData->m_ShaderInfo->m_TechName = TEXT("UmaMusumeFace");
		}
	}
	
	// Cheek
	gameObj = gameObject->FindChildInAllChildren("M_Cheek");
	if (gameObj != nullptr)
	{
		auto renderingData = gameObj->GetComponent<SkinnedMeshRenderer>()->m_RenderingData;
		renderingData->m_ShaderInfo->m_TechName = TEXT("UmaMusumeCheek");
		return;
	}
}

void UmaMusume::SynchronizeBodyHead()
{
	if (m_BodyNeckGameObj == nullptr
		|| m_BodyHeadGameObj == nullptr
		|| m_HeadNeckGameObj == nullptr
		|| m_HeadHeadGameObj == nullptr
		|| m_EyesNeckGameObj == nullptr
		|| m_EyesHeadGameObj == nullptr)
		return;

	m_HeadNeckGameObj->GetTransform()->SetWorldTM(m_BodyNeckGameObj->GetTransform()->GetWorldTM());
	m_HeadHeadGameObj->GetTransform()->SetWorldTM(m_BodyHeadGameObj->GetTransform()->GetWorldTM());

	m_EyesNeckGameObj->GetTransform()->SetWorldTM(m_BodyNeckGameObj->GetTransform()->GetWorldTM());
	m_EyesHeadGameObj->GetTransform()->SetWorldTM(m_BodyHeadGameObj->GetTransform()->GetWorldTM());

	// 눈동자 모델링이 얼굴하고 겹치지 않도록 위치를 살짝 조정함
	XMFLOAT3 temp = m_EyesNeckGameObj->GetTransform()->GetLocalPosition();
	m_EyesNeckGameObj->GetTransform()->SetPosition(XMFLOAT3{ temp.x, temp.y + 0.01f, temp.z }, Space::LOCAL);
	temp = m_EyesHeadGameObj->GetTransform()->GetLocalPosition();
	m_EyesHeadGameObj->GetTransform()->SetPosition(XMFLOAT3{ temp.x, temp.y + 0.01f, temp.z }, Space::LOCAL);
}

void UmaMusume::Init()
{
	// 보이스 재생
	ResourceManager::GetInstance()->Play_SoundResource(m_CharVocalFile, IPlayMode::Effect);
	if (m_LiveID == "1032")
		ResourceManager::GetInstance()->Play_SoundResource(m_CharSubVocalFile, IPlayMode::Effect);
}

void UmaMusume::LateUpdate(float deltaTime)
{
	SynchronizeBodyHead();
}
