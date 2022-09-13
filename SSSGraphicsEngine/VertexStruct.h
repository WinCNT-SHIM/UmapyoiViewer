#pragma once

#define  _DX11RENDERER
#include "IDefine.h"

namespace GraphicsEngine
{
	namespace VertexStruct
	{
		class VertexPos
		{
		public:
			XMFLOAT3 Pos;
			static const D3D11_INPUT_ELEMENT_DESC InputLayout[1];
		};

		class VertexPosColor
		{
		public:
			XMFLOAT3 Pos;
			XMFLOAT4 Color;
			static const D3D11_INPUT_ELEMENT_DESC InputLayout[2];
		};

		class VertexPosTex
		{
		public:
			XMFLOAT3 Pos;
			XMFLOAT2 Tex;
			static const D3D11_INPUT_ELEMENT_DESC InputLayout[2];
		};

		class VertexPosNormalColor
		{
		public:
			XMFLOAT3 Pos;
			XMFLOAT3 Normal;
			XMFLOAT4 Color;
			static const D3D11_INPUT_ELEMENT_DESC InputLayout[3];
		};

		class VertexPosNormalTex
		{
		public:
			XMFLOAT3 Pos;
			XMFLOAT3 Normal;
			XMFLOAT2 Tex;
			static const D3D11_INPUT_ELEMENT_DESC InputLayout[3];
		};

		class VertexPosNormalTexTangent
		{
		public:
			XMFLOAT3 Pos;
			XMFLOAT3 Normal;
			XMFLOAT2 Tex;
			XMFLOAT3 TangentL;
			static const D3D11_INPUT_ELEMENT_DESC InputLayout[4];
		};

		class VertexBasic
		{
		public:
			XMFLOAT3 Pos;
			XMFLOAT3 Normal;
			XMFLOAT2 Tex;
			XMFLOAT3 TangentL;
			XMFLOAT4 Color;
			static const D3D11_INPUT_ELEMENT_DESC InputLayout[5];
		};

		class VertexBasicSkinned
		{
		public:
			XMFLOAT3 Pos;
			XMFLOAT3 Normal;
			XMFLOAT2 Tex;
			XMFLOAT3 TangentL;
			XMFLOAT3 Weights;
			UINT BoneIndices[4];
			XMFLOAT4 Color;
			static const D3D11_INPUT_ELEMENT_DESC InputLayout[7];
		};
	}
}