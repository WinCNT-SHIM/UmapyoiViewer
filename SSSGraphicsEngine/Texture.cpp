#include "GraphicsEnginePCH.h"
#include "Texture.h"

#include "../Inc/DDSTextureLoader.h"	// CreateDDSTextureFromFile

using namespace GraphicsEngine;

Texture::Texture()
	: ResourceBase<ResourceType::TEXTURE>()
	, m_ShaderResourceView(nullptr)
{
}

Texture::~Texture()
{
}

void Texture::Initialize(const std::string& path)
{
	// Ȯ���� ����...�� �� ���� �Ƴ�
	//tstring ext = std::filesystem::path(path).extension();

	std::wstring convertPath;
	convertPath.assign(path.begin(), path.end());

	ComPtr<ID3D11Device> device = Renderer::GetInstance()->GetDevice();
	ComPtr<ID3D11Resource> texResource = nullptr;
	HR(CreateDDSTextureFromFile(
		device.Get()
		, convertPath.c_str()
		, texResource.GetAddressOf()
		, m_ShaderResourceView.GetAddressOf()
	));
	texResource.Reset();
}

void Texture::Release()
{
	m_ShaderResourceView.Reset();
}
