/// <summary>
/// ����Ʈ���� �����ϴ� �Ŵ��� Ŭ����
/// ���±��� Effects Ŭ�������� ó���Ϸ��� ������ ���⵵�� ���Ƽ� �и���
/// �����ϴٸ� Effect�鵵 �ϳ��� Ŭ������ ���� �����̴�
/// 
/// Created : 2022/08/08 �ɽ¼�
/// Updated : 2022/08/08 �ɽ¼�
/// </summary>
#pragma once

namespace GraphicsEngine
{
	class EffectBase;

	class EffectManager
	{
	// �굵...�̱���...? �̷��� �̱��� �ƴ� �༮�� ã�� �� ���������...
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
