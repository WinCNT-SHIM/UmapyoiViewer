#pragma once
#include <d3d11.h>
#include <DirectXMath.h>			//#include <xnamath.h>���� ��ü
#include <DirectXPackedVector.h>	//#include <xnamath.h>���� ��ü
#include <DirectXColors.h>			// DirectX::Colors
//#include <d3dcommon.h>			// FEATURE_LEVEL, d3d11.h ���ο� �־ ����
//#include <dxgiformat.h>			// DXGI_FORMAT, d3d11.h ���ο� �־ ����

#include <wrl.h>					// ComPtr
using namespace Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")

#include "../Inc/d3dx11effect.h" // effect, tech

#ifdef x64
#if (defined(DEBUG) || defined(_DEBUG))
#pragma comment( lib, "../Lib/Effects11d_x64.lib" )
#pragma comment( lib, "../Lib/DirectXTK_x64.lib" )
#else
#pragma comment( lib, "../Lib/Effects11_x64.lib" )
#pragma comment( lib, "../Lib/DirectXTKr_x64.lib" )
#endif // DEBUG
#else
#if (defined(DEBUG) || defined(_DEBUG))
#pragma comment( lib, "../Lib/Effects11d.lib" )
#pragma comment( lib, "../Lib/DirectXTK.lib" )
#else
#pragma comment( lib, "../Lib/Effects11.lib" )
#pragma comment( lib, "../Lib/DirectXTKr.lib" )
#endif // DEBUG
#endif // x64


//#include "LightHelper.h"

// HR()�� dxerr.lib�� ���ԵǾ� �ִٰ� DX SDK�� Windows10 SDK�� ���յǸ鼭 ��������Ƿ� ��� ������ ó���� �߰�
// ��ó: https://wergia.tistory.com/6 [������ ���α׷��� ��Ʈ]
#if defined(DEBUG) || defined(_DEBUG)
#ifndef HR
#define HR(x) 											\
{														\
	HRESULT hr = (x);									\
	if(FAILED(hr))										\
	{													\
		LPWSTR output;									\
		FormatMessage(									\
			FORMAT_MESSAGE_FROM_SYSTEM |				\
			FORMAT_MESSAGE_IGNORE_INSERTS |				\
			FORMAT_MESSAGE_ALLOCATE_BUFFER,				\
			NULL,										\
			hr,											\
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	\
			(LPTSTR) &output,							\
			0,											\
			NULL);										\
		MessageBox(NULL, output, _T("Error"), MB_OK);	\
	}													\
}
#endif
#else
#ifndef HR
#define HR(x) (x)
#endif
#endif

// COM �������̽��� �����ϰ� �����ϱ� ���� ��ũ�� �Լ�
#define ReleaseCOM(x) { if(x){ x->Release();x = nullptr; } }
#define SafeDelete(x) { delete x; x = 0; }

using int8 = __int8;
using int16 = __int16;
using int32 = __int32;
using int64 = __int64;
using uint8 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

#if defined(UNICODE)
using tstring = std::wstring;
#else
using tstring = std::string;
#endif