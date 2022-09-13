/// <summary>
/// 리소스 매니저(게임 엔진용)
/// 그래픽 리소스가 필요한 경우는 그래픽스 엔진의 리소스 매니저와 통신해서 처리하도록 하자
/// 
/// Created : 2022/08/04 심승수
/// Updated : 2022/08/04 심승수
/// </summary>
#pragma once

#ifdef _SSSEngine_EXPORTS
#define DLL_DECLSPEC __declspec(dllexport)
#else
#define DLL_DECLSPEC __declspec(dllimport)
#endif

class CASEParser;
class PrefabASE;

class DLL_DECLSPEC ResourceManager
{
	//싱글톤으로 만들자
public:
	ResourceManager() = default;
	ResourceManager(const ResourceManager & ref) = delete;
	ResourceManager& operator=(const ResourceManager & ref) = delete;
	~ResourceManager() { /*Release();*/ }
private:
	static std::shared_ptr<ResourceManager> m_Instance;
public:
	static std::shared_ptr<ResourceManager> GetInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = std::make_shared<ResourceManager>();
			m_Instance->Init();
		}
		return m_Instance;
	}

private:
	std::unordered_map<std::string, unsigned int> m_MeshIDs;
	std::unordered_map<std::string, unsigned int> m_TextureIDs;
	std::unordered_map<std::string, std::shared_ptr<PrefabASE>> m_PrefabASEs;

private:
	std::string m_FilePath;

public:
	const unsigned int GetMeshID(const std::string& meshPath);
	const unsigned int GetTextureID(const std::string& texturePath);
	const std::shared_ptr<PrefabASE> GetPrefab(const std::string& prefabName);

public:
	void LoadResource_Texture(const std::string& filePath);
	void LoadResource_ASEFile(const std::string& filePath);
	bool LoadResource_Sound(const std::string& name, std::string filePath, unsigned int mode = 0);

	void Play_SoundResource(std::string name, IPlayMode mode);
	void Stop_SoundResource(std::string name, IPlayMode mode);
	void SetVolume_SoundResource(float volume, IPlayMode mode);

private:
	CASEParser* LoadASEFile(const std::string& filePath);
	std::shared_ptr<PrefabASE> BuildPrefabFromASE(std::string filePath, CASEParser* aseData);

private:
	void CreateBasicMesh();

public:
	void Init();
	void Update() {}
	void Release();
};

