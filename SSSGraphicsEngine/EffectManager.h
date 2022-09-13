/// <summary>
/// 이펙트들을 관리하는 매니저 클래스
/// 여태까진 Effects 클래스에서 처리하려고 해지만 복잡도가 높아서 분리함
/// 가능하다면 Effect들도 하나의 클래스로 나눌 예정이다
/// 
/// Created : 2022/08/08 심승수
/// Updated : 2022/08/08 심승수
/// </summary>
#pragma once

namespace GraphicsEngine
{
	class EffectBase;

	class EffectManager
	{
	// 얘도...싱글톤...? 이러다 싱글톤 아닌 녀석을 찾기 더 힘들어질듯...
	public:
		EffectManager() = default;
		EffectManager(const EffectManager& ref) = delete;
		EffectManager& operator=(const EffectManager& ref) = delete;
		~EffectManager() = default;
	private:
		static std::shared_ptr<EffectManager> m_Instance;
	public:
		static std::shared_ptr<EffectManager> GetInstance()
		{
			if (m_Instance == nullptr)
			{
				m_Instance = std::make_shared<EffectManager>();
			}
			return m_Instance;
		}

	private:
		std::unordered_map<tstring, std::shared_ptr<EffectBase>> m_Effects;
		//std::map<tstring, std::shared_ptr<VertexShader>> _vertexShaders;
		//std::map<tstring, std::shared_ptr<PixelShader>> _pixelShaders;
		//std::map<tstring, std::shared_ptr<GeometryShader>> _geometryShaders;

	public:
		std::shared_ptr<EffectBase> GetEffect(const tstring& name);

	public:
		//void Init(ComPtr<ID3D11Device> device);
		void Init(std::shared_ptr<DXDevice> device);
		void Release();
	};
}
