#include "GraphicsEnginePCH.h"
#include "ForwardRenderingPass.h"

#include "GraphicsRenderingData.h"
#include "RenderTarget.h"

#include "EffectManager.h"
#include "EffectBase.h"

void GraphicsEngine::ForwardRenderingPass::Init(const uint32& screenWidth, const uint32& screenHeight)
{
	__super::Init(screenWidth, screenHeight);

	m_SwapChain = Renderer::GetInstance()->GetSwapChain();
	m_RenderTarget = Renderer::GetInstance()->GetRenderTarget();
}

void GraphicsEngine::ForwardRenderingPass::OnResize(const uint32& screenWidth, const uint32& screenHeight)
{
	m_ScreenWidth = screenWidth;
	m_ScreenHeight = screenHeight;
}

void GraphicsEngine::ForwardRenderingPass::Release()
{
	m_RenderTarget.reset();
	m_SwapChain.reset();



	__super::Release();
}

void GraphicsEngine::ForwardRenderingPass::BeginRender()
{
	m_RenderTarget->Clear();
}

void GraphicsEngine::ForwardRenderingPass::EndRender()
{
	m_SwapChain->Present();
}

void GraphicsEngine::ForwardRenderingPass::Render(std::shared_ptr<PerFrameData> m_PerFrameData, std::vector<std::shared_ptr<RenderingData>>& renderQueue)
{
	this->BeginRender();

	// Per Frame ��� ���� Update(CBPerFrame ����Ʈ�� ���̷� �׷��� ������Ʈ)
	std::shared_ptr<EffectBase> fx = EffectManager::GetInstance()->GetEffect(_T("CBPerFrame"));
	fx->UpdatePerFrameData(m_PerFrameData);
	//fx.reset();
	
	for (auto& renderingData : renderQueue)
	{
		if (renderingData->m_ShaderInfo->m_RasterizerState == ShaderInfo::RasterizerState::SOLID)
		{
			// todo: �����Ͷ����� ������Ʈ �߻�ȭ �����̶�...�ϴ� �ο� ������ ����
			ComPtr<ID3D11RasterizerState> rasterizerState;
			D3D11_RASTERIZER_DESC solidDesc;
			ZeroMemory(&solidDesc, sizeof(D3D11_RASTERIZER_DESC));
			solidDesc.FillMode = D3D11_FILL_SOLID;
			solidDesc.CullMode = D3D11_CULL_BACK;
			solidDesc.FrontCounterClockwise = false;
			solidDesc.DepthClipEnable = true;
			HR(m_Device->CreateRasterizerState(&solidDesc, &rasterizerState));

			m_DeviceContext->RSSetState(rasterizerState.Get());


			//m_RasterizerState->SetSolidRenderState(_deviceContext);
		}
		else if (renderingData->m_ShaderInfo->m_RasterizerState == ShaderInfo::RasterizerState::WIREFRAME)
		{
			// todo: ���� ������Ʈ �߻�ȭ �����̶�...�ϴ� �ο� ������ ����
			ComPtr<ID3D11RasterizerState> rasterizerState;
			D3D11_RASTERIZER_DESC wireframeDesc;
			ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
			wireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
			wireframeDesc.CullMode = D3D11_CULL_BACK;
			wireframeDesc.FrontCounterClockwise = false;
			wireframeDesc.DepthClipEnable = true;
			HR(m_Device->CreateRasterizerState(&wireframeDesc, &rasterizerState));

			m_DeviceContext->RSSetState(rasterizerState.Get());



			//_rasterizerState->SetWireFrameRenderState(_deviceContext);
		}

		switch (renderingData->m_DataType)
		{
			case RenderingdataType::STATIC_MESH_OBJECT:
			{
				ExecuteStaticMeshObject(renderingData);
				break;
			}
			case RenderingdataType::SKINNED_MESH_OBJECT:
			{
				ExecuteForSkinnedMeshObject(renderingData);
				break;
			}
			case RenderingdataType::SKY_BOX:
			{
				// ��ī�� �ڽ��� ���� ������ ����!!
				//m_rasterizerState->SetSolidRenderStateNoneCull(_deviceContext);
				//ExecuteForStaticMeshObject(renderingData);
				//_rasterizerState->SetSolidRenderState(_deviceContext);

				break;
			}
		}
	}
	this->EndRender();
}

void GraphicsEngine::ForwardRenderingPass::ExecuteStaticMeshObject(std::shared_ptr<RenderingData>& renderingData)
{
	// ������ �����Ϳ��� ����Ʈ ȿ���� �����Ǿ� ������ ����Ʈ �����ӿ�ũ�� �̿��ؼ� �������Ѵ�
	if (renderingData->m_ShaderInfo->m_TechName != _T(""))
	{
		// �ش� ����Ʈ�� ȹ��
		std::shared_ptr<EffectBase> fx = EffectManager::GetInstance()->GetEffect(renderingData->m_ShaderInfo->m_TechName);
		fx->EffectRendering(renderingData);
	}
	// ����Ʈ �����ӿ�ũ�� ������� �ʴ� ������(�ڼ��Ѱ� ���� ��?��)
	else
	{
	}
}

void GraphicsEngine::ForwardRenderingPass::ExecuteForSkinnedMeshObject(std::shared_ptr<RenderingData> renderingData)
{
	// ������ �����Ϳ��� ����Ʈ ȿ���� �����Ǿ� ������ ����Ʈ �����ӿ�ũ�� �̿��ؼ� �������Ѵ�
	if (renderingData->m_ShaderInfo->m_TechName != _T(""))
	{
		// �ش� ����Ʈ�� ȹ��
		std::shared_ptr<EffectBase> fx = EffectManager::GetInstance()->GetEffect(renderingData->m_ShaderInfo->m_TechName);
		fx->EffectRendering(renderingData);
	}
	// ����Ʈ �����ӿ�ũ�� ������� �ʴ� ������(�ڼ��Ѱ� ���� ��?��)
	else
	{
	}
}
