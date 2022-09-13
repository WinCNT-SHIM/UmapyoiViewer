#pragma once
#include <d3d11.h>
#include <DirectXMath.h>			//#include <xnamath.h>에서 교체
#include <DirectXPackedVector.h>	//#include <xnamath.h>에서 교체
#include <DirectXColors.h>			// DirectX::Colors
//#include <d3dcommon.h>			// FEATURE_LEVEL, d3d11.h 내부에 있어서 삭제
//#include <dxgiformat.h>			// DXGI_FORMAT, d3d11.h 내부에 있어서 삭제

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

// HR()은 dxerr.lib에 포함되어 있다가 DX SDK가 Windows10 SDK로 통합되면서 사라졌으므로 대신 다음이 처리를 추가
// 출처: https://wergia.tistory.com/6 [베르의 프로그래밍 노트]
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

// COM 인터페이스를 안전하게 해제하기 위한 매크로 함수
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