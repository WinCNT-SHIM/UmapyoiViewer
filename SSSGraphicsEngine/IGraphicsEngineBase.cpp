#include "GraphicsEnginePCH.h"
#include "IGraphicsEngineBase.h"
#include "DX11GraphicsEngine.h"

//IGraphicsEngineBase* IGraphicsEngineBase::m_Instance = nullptr;
std::shared_ptr<IGraphicsEngineBase> IGraphicsEngineBase::m_Instance = nullptr;

shared_ptr<IGraphicsEngineBase> IGraphicsEngineBase::CreateGraphicsEngine()
{
	if (IGraphicsEngineBase::m_Instance == nullptr)
	{
		//IGraphicsEngineBase::m_Instance = new GraphicsEngine::DX11GraphicsEngine();
		IGraphicsEngineBase::m_Instance = std::make_shared<GraphicsEngine::DX11GraphicsEngine>();
	}
	return IGraphicsEngineBase::m_Instance;
}

shared_ptr<IGraphicsEngineBase> IGraphicsEngineBase::GetGraphicsEngine()
{
	if (IGraphicsEngineBase::m_Instance == nullptr)
	{
		//IGraphicsEngineBase::m_Instance = new GraphicsEngine::DX11GraphicsEngine();
		IGraphicsEngineBase::m_Instance = std::make_shared<GraphicsEngine::DX11GraphicsEngine>();
	}
	return IGraphicsEngineBase::m_Instance;
}

//DLL_DECLSPEC shared_ptr<IGraphicsEngineBase> CreateGraphicsEngine()
//{
//	if (IGraphicsEngineBase::m_Instance == nullptr)
//	{
//		//IGraphicsEngineBase::m_Instance = new GraphicsEngine::DX11GraphicsEngine();
//		IGraphicsEngineBase::m_Instance = std::make_shared<GraphicsEngine::DX11GraphicsEngine>();
//	}
//	return IGraphicsEngineBase::m_Instance;
//}

//DLL_DECLSPEC void ReleaseGraphicsEngine(IGraphicsEngineBase* pModule)
//{
//	if (pModule)
//	{
//		pModule->Finalize();
//		delete pModule;
//	}
//}

//std::shared_ptr<IGraphicsEngineBase> GetGraphicsEngine()
//{
//	if (IGraphicsEngineBase::m_Instance == nullptr)
//	{
//		IGraphicsEngineBase::m_Instance = std::make_shared<GraphicsEngine::DX11GraphicsEngine>();
//	}
//	return IGraphicsEngineBase::m_Instance;
//}
