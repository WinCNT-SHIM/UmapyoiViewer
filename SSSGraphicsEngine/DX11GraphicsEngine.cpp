#include "GraphicsEnginePCH.h"
#include "DX11GraphicsEngine.h"

#include "Renderer.h"

//#include "Effects.h"
//#include "InputLayouts.h"
//
//#include "RenderTarget.h"
//#include "../ParserLib/CASEParser.h"	// ASE Parser
#include "ResourceManager.h"
#include "EffectManager.h"

namespace GraphicsEngine
{
	DX11GraphicsEngine::DX11GraphicsEngine()
		: m_hWnd(nullptr)
	{
	}

	DX11GraphicsEngine::~DX11GraphicsEngine()
	{
		
	}

	bool DX11GraphicsEngine::Init(void* hInst, void* hWnd, int screenWidth, int screenHeight)
	{
		m_hWnd = reinterpret_cast<HWND>(hWnd);

		// (�̱��� ����)�ʱ�ȭ�� ���⼭ �ϵ��� �ұ�?
		Renderer::GetInstance()->Init(m_hWnd, screenWidth, screenHeight);
		ResourceManager::GetInstance()->Init();
		EffectManager::GetInstance()->Init(Renderer::GetInstance()->GetDXDevice());

		//Effects::InitAll(m_D3DXDevice);
		//InputLayouts::InitAll(m_D3DXDevice);

		// ����� ������ ȹ��
		//GetAdapterInfo();

		// �׷��Ƚ� ������ ī�޶� �����Ѵ�
		//SetCamera(m_Camera);

		// ���� ������Ʈ ����
		//CreateRenderStates();

		// ��Ʈ(�۾�)�� �׸� ��ü�� �����Ѵ�
		//m_Font = new D3DX11Font();
		//m_Font->Initialize(m_D3DXDevice, m_SolidRS, NormalDSS);

		//// ==================== ��ǲ ���̾ƿ��� ����Ʈ ������ �ʱ�ȭ ====================
		//Effects::InitAll(m_D3DXDevice);
		//InputLayouts::InitAll(m_D3DXDevice);

		//// todo: ASE Parser �׽�Ʈ��
		//CASEParser* m_pASEParser;
		//m_pASEParser = new CASEParser();
		//m_pASEParser->Init();
		//m_pASEParser->Load((LPSTR)"ASEFile/genji_max.ASE");
		//
		//m_pASEParser->Load((LPSTR)"ASEFile/box.ASE");
		//m_pASEParser->Load((LPSTR)"ASEFile/box.ASE");
		//m_pASEParser->Load((LPSTR)"ASEFile/box.ASE");
		//m_pASEParser->Load((LPSTR)"ASEFile/box.ASE");
		//m_pASEParser->Load((LPSTR)"ASEFile/box.ASE");
		//
		//delete m_pASEParser;

		//// todo: ���ҽ� �Ŵ��� ASE Parser �׽�Ʈ��
		////ResourceManager::GetInstance();
		//std::vector<VertexBuffer*> _tmpVB;
		//std::vector<IndexBuffer*> _tmpIB;
		//ResourceManager::GetInstance()->GetResource_ASEFile(m_D3DXDevice, "ASEFile/box.ASE", _tmpVB, _tmpIB);
		//ResourceManager::GetInstance()->GetResource_ASEFile(m_D3DXDevice, "ASEFile/box.ASE", _tmpVB, _tmpIB);
		//ResourceManager::GetInstance()->GetResource_ASEFile(m_D3DXDevice, "ASEFile/box.ASE", _tmpVB, _tmpIB);

		return true;
	}

	void DX11GraphicsEngine::Update(float deltaTime)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DX11GraphicsEngine::BeginRender()
	{
		// ���� Ÿ��(�� ����)�� Ŭ�����Ѵ�
		Renderer::GetInstance()->BeginRender();
	}

	void DX11GraphicsEngine::PostPerFrameData(std::shared_ptr<PerFrameData> perFrameData)
	{
		Renderer::GetInstance()->PostPerFrameData(perFrameData);
	}

	void DX11GraphicsEngine::PostRenderingData(std::shared_ptr<RenderingData> renderingData)
	{
		Renderer::GetInstance()->PostRenderingData(renderingData);
	}

	void DX11GraphicsEngine::Render()
	{
		// ������ �׸��� ó���� �ϴ� �κ�
		Renderer::GetInstance()->Render();
	}

	void DX11GraphicsEngine::PostProcess()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DX11GraphicsEngine::EndRender()
	{
		// �ĸ� ���۸� ȭ�鿡 Present�Ѵ�
		Renderer::GetInstance()->EndRender();
	}

	void DX11GraphicsEngine::Release()
	{
		// RAII(�� �³�?)�� ���Ѻ���...
		EffectManager::GetInstance()->Release();
		ResourceManager::GetInstance()->Release();
		Renderer::GetInstance()->Release();
	}

	void DX11GraphicsEngine::Draw_Test()
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DX11GraphicsEngine::OnResize(const int& screenWidth, const int& screenHeight)
	{
		Renderer::GetInstance()->OnResize(screenWidth, screenHeight);
	}

	bool DX11GraphicsEngine::IsValidDevice()
	{
		//return m_D3DXDevice->IsValid();
		return Renderer::GetInstance()->IsValidDevice();
	}

	void DX11GraphicsEngine::SetClientSize(int width, int height)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DX11GraphicsEngine::SetWinMinMax(bool isMinimized, bool isMaxmized)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DX11GraphicsEngine::OnMouseDown(int x, int y)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DX11GraphicsEngine::OnMouseUp(int x, int y)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	void DX11GraphicsEngine::OnMouseMove(int btnState, int x, int y)
	{
		throw std::logic_error("The method or operation is not implemented.");
	}

	const uint32 DX11GraphicsEngine::LoadMesh(
		const std::string& meshName
		, bool isDynamic /*= false */
		, bool isLineList /*= false */
	)
	{
		uint32 res = UINT_MAX;

		res = GraphicsEngine::ResourceManager::GetInstance()->LoadMesh<VertexStruct::VertexBasic>(
			meshName
			, isDynamic
			, isLineList);

		return res;
	}

	const uint32 DX11GraphicsEngine::LoadMesh(
		const std::string& meshName
		, const std::vector<VertexStruct::VertexBasic>& vertexVector
		, const std::vector<uint32>& indexVector
		, bool isDynamic /*= false */
		, bool isLineList /*= false */
	)
	{
		uint32 res = 0;
		res = GraphicsEngine::ResourceManager::GetInstance()->LoadMesh<VertexStruct::VertexBasic>(
			meshName, vertexVector, indexVector, isDynamic, isLineList
			);
		return res;
	}

	const uint32 DX11GraphicsEngine::LoadMesh(
		const std::string& meshName
		, const std::vector<VertexStruct::VertexBasicSkinned>& vertexVector
		, const std::vector<uint32>& indexVector
		, bool isDynamic /*= false */
		, bool isLineList /*= false */
	)
	{
		uint32 res = 0;
		res = GraphicsEngine::ResourceManager::GetInstance()->LoadMesh<VertexStruct::VertexBasicSkinned>(
			meshName, vertexVector, indexVector, isDynamic, isLineList
			);
		return res;
	}

	const uint32 DX11GraphicsEngine::LoadTexture(const std::string& path)
	{
		uint32 res = 0;
		res = GraphicsEngine::ResourceManager::GetInstance()->LoadTexture(path);
		return res;
	}

	void DX11GraphicsEngine::ReleaseResourceOnly()
	{
		GraphicsEngine::ResourceManager::GetInstance()->Release();
	}

}