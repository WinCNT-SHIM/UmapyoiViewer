#pragma once

#include "ResourceBase.h"

namespace GraphicsEngine
{
	class Texture
		: public ResourceBase<ResourceType::TEXTURE>
	{
	public:
		Texture();
		virtual ~Texture();

	public:
		ComPtr<ID3D11ShaderResourceView> m_ShaderResourceView;

	public:
		ComPtr<ID3D11ShaderResourceView> GetSRV() { return m_ShaderResourceView; }

	public:
		void Initialize(const std::string& path);
		virtual void Release() override;
	};
}