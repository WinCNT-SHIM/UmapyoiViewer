#pragma once
#include "CASEParser.h"

#include <filesystem>

// 이런 변수를 추가하는 게 맞나...
int _FaceNum;

CASEParser::CASEParser()
{
	m_MaterialCount = 0;
	m_parsingmode = eNone;
}

CASEParser::~CASEParser()
{
	delete m_lexer;
	for (auto& item : m_MeshList)
		delete item;
	m_MeshList.clear();

	for (auto item : m_MaterialList)
		delete item;
	m_MaterialList.clear();
}

bool CASEParser::Init()
{
	m_lexer = new ASE::CASELexer;

	return TRUE;
}
//---------------------------------------------------------------------------------------------------
// 로딩한다.
// 이것이 끝났다면 정해진 데이터형에 모든 데이터를 읽어서 들어가 있어야 한다.
//
// Parsing에 대해:
// 항상 느끼는 것이지만 parsing이라는 것은 데이터가 일정 규격에 맞게 들어가 있다는 것을 전제로 한다.
// 하지만, 파일 내부에 순서가 바뀌어 들어가 있는것이 가능하다던지 하는 규칙이 생기면
// 검색하는 루틴이 복잡해지기 마련. 일단은 순서대로 일정 규약으로 들어가 있다는것을 가정하자.
// -> 그래서, 재귀 호출을 하는 하나의 큰 함수로 해결봤다.
// -> depth를 기준으로 오동작에 대한 안전 코드를 넣어야겠다
//---------------------------------------------------------------------------------------------------
bool CASEParser::Load(LPSTR p_File)
{
	/// 0) 파일을 로드한다.
	if (!m_lexer->Open(p_File))
	{
		TRACE("파일을 여는 중에 문제가 발생했습니다!");
		return FALSE;
	}

	/// 1) 다롱이아부지김교수의 파워풀한 재귀함수 한개로 처리 끝!
	Parsing_DivergeRecursiveALL(0);

	m_lexer->Close();

	// 최적화도 끝내자
	for (auto& item : m_MeshList)
	{
		this->Convert_Optimize(item);
	}

	return TRUE;
}

/// 1차 변환

// CScenedata 복사
bool CASEParser::TranslateToD3DFormat_scene(Scenedata* pscene)
{
	// 값 복사
	*pscene = this->m_scenedata;
	return TRUE;
}

/// <summary>
/// 메시의 최적화를 해 준다.
/// 노말값, 텍스쳐 좌표에 따라 버텍스를 늘리고, 중첩되는것은 제거하고..
/// </summary>
bool CASEParser::Convert_Optimize(Mesh* pMesh)
{
	// 최적화를 끝낸 Mesh인 경우는 스킵한다
	if (pMesh->m_opt_vertex.size() != 0)
		return true;

	// ==================== Animation 추가 코드 ====================
	// 네거티브 스케일(Negative Scale)을 해결하자
	// 네거티브 스케일 판별 방법
	//   방법1. 회전 행렬의 행렬식이 -1이면 네거티브 스케일이다
	//   방법2. TM_ROW로 구성한 행렬의 1행(x축), 2행(y축), 3행(z축) 중 두 축을 외적한 값이
	//          나머지 한 축과 반대 방향이면 네거티브 스케일이다(내적으로 판별 가능)
	//   방법3. ASE의 Decompose된 이동, 회전, 크기 데이터로 변환 행렬을 구성하고,
	//          TM_ROW로 구성한 행렬의 행들과 각각 내적해서 0보다 작은 값이 있으면 네거티브 스케일이다
	// 
	// 현 프로젝트에는 방법2를 사용하기로 한다
	// 방법2로 네거티브 스케일을 판별했을 경우(정확히는 TM_ROW로 구성한 행렬인 경우)
	//   행렬과 노멀 값에 -1를 곱해야 한다
	// 참고로 Decompose된 데이터로 변환 행렬을 구성한 경우는 노멀 값만에 -1를 곱하면 된다

	// 1행(x축), 2행(y축)를 외적
	XMVECTOR _tmpRes = XMVector3Cross(pMesh->m_TMRow0, pMesh->m_TMRow1);
	// 1행(x축), 2행(y축)의 외적 값과 3행(z축)를 내적
	_tmpRes = XMVector3Dot(_tmpRes, pMesh->m_TMRow2);

	// 내적 값이 0보다 작은 경우는 네거티브 스케일이다
	if (_tmpRes.m128_f32[0] < 0)
	{
		// 행렬에 -1를 곱한다
		pMesh->m_TMRow0 = pMesh->m_TMRow0 * -1;
		pMesh->m_TMRow1 = pMesh->m_TMRow1 * -1;
		pMesh->m_TMRow2 = pMesh->m_TMRow2 * -1;

		// 노멀 값에 -1를 곱한다
		for (auto& item : pMesh->m_meshface)
		{
			item->m_normal *= -1;
			item->m_normalvertex[0] *= -1;
			item->m_normalvertex[1] *= -1;
			item->m_normalvertex[2] *= -1;
		}
	}

	// 2021.04.12
	// 버텍스들은 일단 모두 복사
	for (unsigned int i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		// ==================== Skinnig 추가 코드 ====================
		// 파싱한 본 가중치 정보가 있는 경우 Vertext에 넣어준다
		if (pMesh->m_WVertexs.size() != 0)
		{
			VertexWeight* _tmp = pMesh->m_WVertexs[i];
			// 가중치와 해당 본의 정보를 저장
			for (size_t item = 0; item < _tmp->m_bone_blending_weight.size(); item++)
			{
				if (item == 0)
					pMesh->m_meshvertex[i]->m_bw1 = { _tmp->m_bone_blending_weight[item]->m_bone_number, _tmp->m_bone_blending_weight[item]->m_bone_weight };

				if (item == 1)
					pMesh->m_meshvertex[i]->m_bw2 = { _tmp->m_bone_blending_weight[item]->m_bone_number, _tmp->m_bone_blending_weight[item]->m_bone_weight };
				
				if (item == 2)
					pMesh->m_meshvertex[i]->m_bw3 = { _tmp->m_bone_blending_weight[item]->m_bone_number, _tmp->m_bone_blending_weight[item]->m_bone_weight };

				if (item == 3)
					pMesh->m_meshvertex[i]->m_bw4 = { _tmp->m_bone_blending_weight[item]->m_bone_number, _tmp->m_bone_blending_weight[item]->m_bone_weight };
			}
		}

		pMesh->m_opt_vertex.emplace_back(move(pMesh->m_meshvertex[i]));
		Vertex* _pVertex = pMesh->m_opt_vertex.back();

		// ==================== Collision Detection 추가 코드 ====================
		// 겸사겸사 최소/최대 버텍스도 비교해둔다
		// 최소 버텍스 찾기
		if (_pVertex->m_pos.x < pMesh->m_MinVertexOneMesh.x)
			pMesh->m_MinVertexOneMesh.x = _pVertex->m_pos.x;
		if (_pVertex->m_pos.y < pMesh->m_MinVertexOneMesh.y)
			pMesh->m_MinVertexOneMesh.y = _pVertex->m_pos.y;
		if (_pVertex->m_pos.z < pMesh->m_MinVertexOneMesh.z)
			pMesh->m_MinVertexOneMesh.z = _pVertex->m_pos.z;
		// 최대 버텍스 찾기
		if (_pVertex->m_pos.x > pMesh->m_MaxVertexOneMesh.x)
			pMesh->m_MaxVertexOneMesh.x = _pVertex->m_pos.x;
		if (_pVertex->m_pos.y > pMesh->m_MaxVertexOneMesh.y)
			pMesh->m_MaxVertexOneMesh.y = _pVertex->m_pos.y;
		if (_pVertex->m_pos.z > pMesh->m_MaxVertexOneMesh.z)
			pMesh->m_MaxVertexOneMesh.z = _pVertex->m_pos.z;
	}

	// 파싱 때 찾아둔 전체 오브젝트의 최소/최대 버텍스도 넘겨준다
	pMesh->m_MinVertexObj = m_MinVertexObj;
	pMesh->m_MaxVertexObj = m_MaxVertexObj;

	// 나머지는 face를 기준으로 한 인덱스로 찾아야 함
	unsigned int _faceCount = pMesh->m_meshface.size();
	TRACE("FaceCount : %d", _faceCount);

#pragma region 노멀 값 기준으로 버텍스 쪼개기
	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// face기준으로 버텍스의 노말을 넣어준다.
			if (!_nowVertex->m_isnormalset)
			{
				_nowVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];
				_nowVertex->m_isnormalset = true;
			}
			// 버텍스의 노멀 값이 이미 입력 되었으면 버텍스를 추가한다
			else
			{
				// 노멀 값이 중복되는 버텍스가 이미 있는 경우에는 스킵한다
				// 같은 버텍스 좌표 값이 설정되어 있었으면 다음 처리를 스킵한다
				if (_nowVertex->m_normal == pMesh->m_meshface[i]->m_normalvertex[j])
				{
					continue;
				}

				// 새로운 버텍스 추가
				int _AddVertexIdx = pMesh->m_opt_vertex.size();
				Vertex* _addVertex = new Vertex();
				pMesh->m_opt_vertex.push_back(_addVertex);
				// 인덱스 교체
				pMesh->m_meshface[i]->m_vertexindex[j] = _AddVertexIdx;

				// 좌표상의 위치값
				_addVertex->m_pos = _nowVertex->m_pos;
				// 노말 값은 새로 갱신할 값을 넣는다
				//_addVertex->m_normal = pMesh->m_meshface[i]->m_normal;	// Face Normal 값
				_addVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];
				// 노말값이 셋팅됐는가?
				_addVertex->m_isnormalset = false;
				// 텍스쳐 좌표
				_addVertex->u = _nowVertex->u;
				_addVertex->v = _nowVertex->v;
				// 텍스쳐값이 셋팅됐는가?
				_addVertex->m_IsTextureSet = false;
				/// Skinned Mesh의 weight
				_addVertex->m_bw1 = _nowVertex->m_bw1;
				_addVertex->m_bw2 = _nowVertex->m_bw2;
				_addVertex->m_bw3 = _nowVertex->m_bw3;
				_addVertex->m_bw4 = _nowVertex->m_bw4;
				// 인덱스
				_addVertex->m_indices = _AddVertexIdx;
			}
		}
	}
#pragma endregion

#pragma region 텍스처 좌표 기준으로 버텍스 쪼개기
	// TFACE를 기준으로 버텍스에 텍스처 값 설정하기
	unsigned int _TFaceCount = pMesh->m_mesh_tvfaces;
	TRACE("TFaceCount : %d", _TFaceCount);

	// TFace의 정보는 Face 구조체에 있음
	// TFace의 수 == Face의 수이므로 TFace만큼 루프하면서
	// Face에서 텍스처 좌표를 획득해서 설정한다
	for (unsigned int i = 0; i < _TFaceCount; i++)
	{
		//TRACE("TFaceNum : %d / %d\n", i, _TFaceCount);

		// Face 구조체에 접근
		Face* _nowTFace = pMesh->m_meshface[i];

		for (int j = 0; j < 3; j++)
		{
			// TFace의 TVertext와 Face의 Vertext는 1 대 1 대응 구조
			// TFace를 이루는 TVertext 중 하나의 인덱스를 획득
			int _TVertexIdx = _nowTFace->m_TFace[j];
			// Face를 이루는 Vertext 중 하나의 인덱스를 획득
			int _nowVertexIdx = _nowTFace->m_vertexindex[j];

			// 획득한 Vertext의 인덱스로 최종 버텍스에 접근
			Vertex* _nowVertex = pMesh->m_opt_vertex[_nowVertexIdx];

			// 버텍스의 텍스처 좌표를 설정한다
			if (!_nowVertex->m_IsTextureSet)
			{
				_nowVertex->u = pMesh->m_mesh_tvertex[_TVertexIdx]->m_u;
				_nowVertex->v = pMesh->m_mesh_tvertex[_TVertexIdx]->m_v;
				_nowVertex->m_IsTextureSet = true;
			}
			// 버텍스의 텍스처 좌표 값이 이미 입력 되었으면 버텍스를 추가한다
			else
			{
				// 같은 버텍스 좌표 값이 설정되어 있었으면 다음 처리를 스킵한다
				if (_nowVertex->u == pMesh->m_mesh_tvertex[_TVertexIdx]->m_u
					&& _nowVertex->v == pMesh->m_mesh_tvertex[_TVertexIdx]->m_v)
				{
					continue;
				}

				// 새로운 버텍스 추가
				int _AddVertexIdx = pMesh->m_opt_vertex.size();
				pMesh->m_opt_vertex.emplace_back(new Vertex());
				Vertex* _addVertex = pMesh->m_opt_vertex.back();
				// 인덱스 교체
				pMesh->m_meshface[i]->m_vertexindex[j] = _AddVertexIdx;

				// 좌표상의 위치값
				_addVertex->m_pos = _nowVertex->m_pos;
				// 노말 값
				_addVertex->m_normal = _nowVertex->m_normal;
				// 노말값이 셋팅됐는가?
				_addVertex->m_isnormalset = false;
				// 텍스쳐 좌표
				_addVertex->u = pMesh->m_mesh_tvertex[_TVertexIdx]->m_u;
				_addVertex->v = pMesh->m_mesh_tvertex[_TVertexIdx]->m_v;
				// 텍스쳐값이 셋팅됐는가?
				_addVertex->m_IsTextureSet = false;
				/// Skinned Mesh의 weight
				_addVertex->m_bw1 = _nowVertex->m_bw1;
				_addVertex->m_bw2 = _nowVertex->m_bw2;
				_addVertex->m_bw3 = _nowVertex->m_bw3;
				// 인덱스
				_addVertex->m_indices = _AddVertexIdx;
			}
		}
	}
#pragma endregion

	// 인덱스는 그냥 복사
	// (2번째 3번째 교체는 MeshObject에서 해줌)
	if (pMesh->m_meshface.size() != 0)
	{
		pMesh->m_opt_index = new IndexList[pMesh->m_meshface.size()];
	}

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pMesh->m_opt_index[i].index[j] = pMesh->m_meshface[i]->m_vertexindex[j];
		}
	}

#pragma region 페이스의 탄젠트 스페이스 기저 계산하기
	// ==================== Normal Mapping 추가 코드 ====================
	for (int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		// 삼각형을 이루는 버텍스들의 인덱스
		int idx0 = pMesh->m_opt_index[i].index[0];
		int idx1 = pMesh->m_opt_index[i].index[1];
		int idx2 = pMesh->m_opt_index[i].index[2];

		// 삼각형을 이루는 버텍스들의 포지션
		Vector3 p0 = pMesh->m_opt_vertex[idx0]->m_pos;
		Vector3 p1 = pMesh->m_opt_vertex[idx1]->m_pos;
		Vector3 p2 = pMesh->m_opt_vertex[idx2]->m_pos;

		// 삼각형의 두 변의 벡터
		Vector3 e1 = p1 - p0;
		Vector3 e2 = p2 - p0;

		// 삼각형의 두 변의 텍스처에서의 벡터
		float x1 = pMesh->m_opt_vertex[idx1]->u - pMesh->m_opt_vertex[idx0]->u;
		float y1 = pMesh->m_opt_vertex[idx1]->v - pMesh->m_opt_vertex[idx0]->v;
		float x2 = pMesh->m_opt_vertex[idx2]->u - pMesh->m_opt_vertex[idx0]->u;
		float y2 = pMesh->m_opt_vertex[idx2]->v - pMesh->m_opt_vertex[idx0]->v;

		// 행렬식
		float det = (x1 * y2 - x2 * y1);
		// 행렬식이 0이면 작은 수 입력
		if (det == 0) det = 0.00001f;

		// 행렬식이 0이 아니면 역수를 구함
		float detInv = 1.0f / det;

		// 탄젠트 스페이스의 기저를 구함
		// (Normalize는 나중에 버텍스의 탄젠스 스페이스를 평균낼 때 함)
		Vector3 T = detInv * (y2 * e1 - y1 * e2);
		//Vector3 B = detInv * (y2 * e1 - y1 * e2);	// 셰이더에서 구함

		//
		//float tmp = T.y;
		//T.y = T.z;
		//T.z = tmp;

		pMesh->m_opt_vertex[idx0]->m_tangent += T;
		pMesh->m_opt_vertex[idx1]->m_tangent += T;
		pMesh->m_opt_vertex[idx2]->m_tangent += T;
	}
#pragma endregion 페이스의 탄젠트 스페이스 기저 계산하기

#pragma region 버텍스별 탄젠트 스페이스 기저 정규화(평균)하기
	for (auto& item : pMesh->m_opt_vertex)
	{
		// 그람 슈미트 알고리즘(Gram-Schmitdt orthonormalization)
		Vector3 N = item->m_normal;
		Vector3 T = item->m_tangent;
		
		N *= T.Dot(N);
		T -= N;
		item->m_tangent = XMVector3Normalize(T);
	}
#pragma endregion 버텍스별 탄젠트 스페이스 기저 정규화(평균)하기

	return TRUE;
}

bool CASEParser::ConvertAll(Mesh* pMesh)
{
	// 버텍스들은 일단 모두 복사
	for (unsigned int i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		Vertex* _pVertex = new Vertex;
		_pVertex->m_pos = pMesh->m_meshvertex[i]->m_pos;

		pMesh->m_opt_vertex.push_back(_pVertex);
	}

	// 나머지는 face를 기준으로 한 인덱스로 찾아야 함
	unsigned int _faceCount = pMesh->m_meshface.size();
	TRACE("FaceCount : %d", _faceCount);

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// face기준으로 버텍스의 노말을 넣어준다.
			// (예시용)
			//_nowVertex->m_normal.x = 0;
			//_nowVertex->m_normal.y = 0;
			//_nowVertex->m_normal.z = 0;
			_nowVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];
		}
	}

	// 인덱스는 그냥 복사
	pMesh->m_opt_index = new IndexList[pMesh->m_meshface.size()];

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		for (int j = 0; j < 3; j++)
		{
			pMesh->m_opt_index[i].index[j] = pMesh->m_meshface[i]->m_vertexindex[j];
		}
	}

	return FALSE;
}

ASEParser::Mesh* CASEParser::GetMesh(int index)
{
	return m_MeshList[index];
}

//----------------------------------------------------------------
// 재귀 호출됨을 전제로 하는 분기 함수이다.
//
// 일단 읽고, 토큰에 따라 동작한다.
//
// 재귀를 들어가는 조건은 { 을 만날 경우이다.
// 리턴하는 조건은 '}'를 만나거나 TOKEND_END를 만났을 때이다.
//
// 더불어, 재귀를 들어가기 전 토큰을 읽어, 지금 이 함수가 어떤 모드인지를 결정한다.
//
//----------------------------------------------------------------
void CASEParser::Parsing_DivergeRecursiveALL(int depth)
{
	//----------------------------------------------------------------------
	// 지역변수들 선언
	// 재귀 돌리다가 일정 이상 루프를 도는 경우(오류)를 체크하기 위한 변수
	int i = 0;
	// 리스트에 데이터를 넣어야 할 때 쓰이는 리스트 카운터.
	int listcount = 0;		// 매우 C스러운데 다른 해결책이 없을까?
	// 이것은 현 토큰이 어떤 종류인가를 판별해준다.
	// 이것을 멤버 변수로 하고 재귀를 돌리는것은 위험하다? -> 잘못된 재귀방식이다?
	LONG nowtoken;
	//----------------------------------------------------------------------

	Matrix TM1, TM2;
	Quaternion tempQT;
	Quaternion prevQT;
	Quaternion resultQT;

	/// 이것을 하면 한 개의 토큰을 읽고, 그 종류와 내용을 알 수 있다.
	while (nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END)
	{
		// 일단 한 개의 토큰을 읽고, 그것이 괄호 닫기가 아니라면.
		// 넘어온 토큰에 따라 처리해준다.

		static int iv = 0;

		switch (nowtoken)
		{
		case TOKEND_BLOCK_START:

			Parsing_DivergeRecursiveALL(depth++);
			break;

		case TOKENR_HELPER_CLASS:
			break;

			//--------------------
			// 3DSMAX_ASCIIEXPORT
			//--------------------

		case TOKENR_3DSMAX_ASCIIEXPORT:
			m_data_asciiexport = Parsing_NumberLong();
			break;

			//--------------------
			// COMMENT
			//--------------------

		case TOKENR_COMMENT:
			Parsing_String();	// 그냥 m_TokenString에 읽어버리는 역할 뿐.
			//AfxMessageBox( m_TokenString, NULL, NULL);		/// 임시로 코멘트를 출력해본다
			break;

			//--------------------
			// SCENE
			//--------------------

		case TOKENR_SCENE:
			//
			break;
		case TOKENR_SCENE_FILENAME:
			m_scenedata.m_filename = Parsing_String();		// 일관성 있는 함수의 사용을 위해 String과 Int도 만들어줬다.
			break;
		case TOKENR_SCENE_FIRSTFRAME:
			m_scenedata.m_firstframe = Parsing_NumberLong();
			break;
		case TOKENR_SCENE_LASTFRAME:
			m_scenedata.m_lastframe = Parsing_NumberLong();
			break;
		case TOKENR_SCENE_FRAMESPEED:
			m_scenedata.m_framespeed = Parsing_NumberLong();
			break;
		case TOKENR_SCENE_TICKSPERFRAME:
			m_scenedata.m_ticksperframe = Parsing_NumberLong();
			break;
		case TOKENR_SCENE_MESHFRAMESTEP:
			m_scenedata.m_meshframestep = Parsing_NumberLong();
			break;
		case TOKENR_SCENE_KEYFRAMESTEP:
			m_scenedata.m_keyframestep = Parsing_NumberLong();
			break;
		case TOKENR_SCENE_BACKGROUND_STATIC:
			m_scenedata.m_scene_background_static.x = Parsing_NumberFloat();
			m_scenedata.m_scene_background_static.y = Parsing_NumberFloat();
			m_scenedata.m_scene_background_static.z = Parsing_NumberFloat();
			break;
		case TOKENR_SCENE_AMBIENT_STATIC:
			m_scenedata.m_scene_ambient_static.x = Parsing_NumberFloat();
			m_scenedata.m_scene_ambient_static.y = Parsing_NumberFloat();
			m_scenedata.m_scene_ambient_static.z = Parsing_NumberFloat();
			break;

		case TOKENR_SCENE_ENVMAP:
		{
			// 그냥 안의 내용을 읽어버린다 }가 나올때까지
			while (nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END) {
			}
		}
		break;

		//--------------------
		// MATERIAL_LIST
		//--------------------
		case TOKENR_MATERIAL_COUNT:
		{
			m_MaterialCount = Parsing_NumberInt();
		}
		break;
		
		case TOKENR_MATERIAL:
		{
			Create_materialdata_to_list();
			m_materialdata->m_MaterialNumber = Parsing_NumberInt();
			m_materialdata->m_map_diffuse = nullptr;
			m_materialdata->m_map_generic = nullptr;
		}
		break;

		case TOKENR_MATERIAL_NAME:
		{
			m_materialdata->m_material_name = Parsing_String();
		}
		break;

		case TOKENR_MATERIAL_CLASS:
		{
			m_materialdata->m_material_class = Parsing_String();
		}
		break;

		case TOKENR_MATERIAL_AMBIENT:
		{
			// RGB 값이라 하나하나 값을 넣어준다
			m_materialdata->m_MaterialAmbient.x = Parsing_NumberFloat();
			m_materialdata->m_MaterialAmbient.y = Parsing_NumberFloat();
			m_materialdata->m_MaterialAmbient.z = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_DIFFUSE:
		{
			// RGB 값이라 하나하나 값을 넣어준다
			m_materialdata->m_MaterialDiffuse.x = Parsing_NumberFloat();
			m_materialdata->m_MaterialDiffuse.y = Parsing_NumberFloat();
			m_materialdata->m_MaterialDiffuse.z = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_SPECULAR:
		{
			// RGB 값이라 하나하나 값을 넣어준다
			m_materialdata->m_MaterialSpecular.x = Parsing_NumberFloat();
			m_materialdata->m_MaterialSpecular.y = Parsing_NumberFloat();
			m_materialdata->m_MaterialSpecular.z = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_SHINE:
		{
			m_materialdata->m_MaterialShine = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_SHINESTRENGTH:
		{
			m_materialdata->m_MaterialShineStrength = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_TRANSPARENCY:
		{
			m_materialdata->m_MaterialTransparency = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_WIRESIZE:
		{
			m_materialdata->m_MaterialWiresize = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_SHADING:
		{
			m_materialdata->m_MaterialShading = Parsing_String();
		}
		break;

		case TOKENR_MATERIAL_XP_FALLOFF:
		{
			m_materialdata->m_MaterialXPFalloff = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_SELFILLUM:
		{
			m_materialdata->m_MaterialSelfIllum = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_FALLOFF:
		{
			m_materialdata->m_MaterialFalloff = Parsing_String();
		}
		break;

		case TOKENR_MATERIAL_XP_TYPE:
		{
			m_materialdata->m_MaterialXPType = Parsing_String();
		}
		break;

		case TOKENR_MAP_DIFFUSE:
		{
			m_materialdata->m_istextureexist = true;
			m_materialdata->m_map_temp = new MaterialMap();
			m_materialdata->m_map_diffuse = m_materialdata->m_map_temp;
		}
		break;

		// TOKENR_MAP_BUMP는 현재 필요 없으므로 파싱을 생략함
		case TOKENR_MAP_GENERIC:
		{
			m_materialdata->m_istextureexist = true;
			m_materialdata->m_map_temp = new MaterialMap();
			m_materialdata->m_map_generic = m_materialdata->m_map_temp;
		}
		break;

		case TOKENR_MAP_NAME:
		{
			m_materialdata->m_map_temp->m_map_name = Parsing_String();
		}
		break;

		case TOKENR_MAP_CLASS:
		{
			m_materialdata->m_map_temp->m_map_class = Parsing_String();
		}
		break;

		case TOKENR_MAP_SUBNO:
		{
			m_materialdata->m_map_temp->m_subno = Parsing_NumberInt();
		}
		break;

		case TOKENR_MAP_AMOUNT:
		{
			m_materialdata->m_map_temp->m_map_amount = Parsing_NumberFloat();
		}
		break;

		case TOKENR_BITMAP:
		{
			// 파일명만 추출함
			string temp = Parsing_String();
			m_materialdata->m_map_temp->m_bitmap = std::filesystem::path(temp).filename().string();
		}
		break;

		case TOKENR_MAP_TYPE:
		{
			m_materialdata->m_map_temp->m_map_type = Parsing_String();
		}
		break;

		case TOKENR_UVW_U_OFFSET:
		{
			m_materialdata->m_map_temp->m_uvw_u_offset = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_V_OFFSET:
		{
			m_materialdata->m_map_temp->m_uvw_v_offset = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_U_TILING:
		{
			m_materialdata->m_map_temp->m_u_tiling = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_V_TILING:
		{
			m_materialdata->m_map_temp->m_v_tiling = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_ANGLE:
		{
			m_materialdata->m_map_temp->m_uvw_angle = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_BLUR:
		{
			m_materialdata->m_map_temp->m_uvw_blur = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_BLUR_OFFSET:
		{
			m_materialdata->m_map_temp->m_uvw_blur_offset = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_NOUSE_AMT:
		{
			m_materialdata->m_map_temp->m_uvw_noise_amt = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_NOISE_SIZE:
		{
			m_materialdata->m_map_temp->m_uvw_noise_size = Parsing_NumberFloat();
		}
		break;

		case TOKENR_UVW_NOISE_LEVEL:
		{
			m_materialdata->m_map_temp->m_uvw_noise_level = Parsing_NumberInt();
		}
		break;

		case TOKENR_UVW_NOISE_PHASE:
		{
			m_materialdata->m_map_temp->m_uvw_noise_phase = Parsing_NumberFloat();
		}
		break;

		case TOKENR_BITMAP_FILTER:
		{
			m_materialdata->m_map_temp->m_bitmap_filter = Parsing_String();
		}
		break;

		//--------------------
		// GEOMOBJECT
		//--------------------

		case TOKENR_GROUP:
			//	한 개의 그룹 시작. 이 다음에 이름이 스트링으로 나오기는 하는데.
			break;

		case TOKENR_HELPEROBJECT:
			// 일단 생성하고
			// 오브젝트의 타입 정해줌. 이것에 따라 서로 다른 파싱 모드 발동.
		{
			m_parsingmode = eHelperObject;	// 파싱 모드
			Create_onemesh_to_list();
			m_OneMesh->m_type = eObjectType_HelperObject;	// 헬퍼 오브젝트
		}
		break;

		case TOKENR_GEOMOBJECT:
			/// 이 토큰을 만났다는건 새로운 메시가 생겼다는 것이다. 지역 변수로 mesh를 하나 선언, 그 포인터를 리스트에 넣고, m_onemesh에 그 포인터를 복사, 그대로 쓰면 될까?
		{
			m_parsingmode = eGeomobject;
			Create_onemesh_to_list();
			m_OneMesh->m_type = eObjectType_Geomobject;		// Geom 오브젝트
		}
		break;

		case TOKENR_SHAPEOBJECT:
		{
			m_parsingmode = eShape;
			Create_onemesh_to_list();
			m_OneMesh->m_type = eObjectType_Shape;		// Geom 오브젝트
		}
		break;
		

		case TOKENR_NODE_NAME:
			// 어쩄든 지금은 오브젝트들을 구별 할 수 있는 유일한 값이다.
			// 모드에 따라 넣어야 할 곳이 다르다.
			switch (m_parsingmode)
			{
			case eParsingmode::eGeomobject:
				m_OneMesh->m_Nodename = Parsing_String();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_Nodename = Parsing_String();
				break;
			case eParsingmode::eAnimation:
				m_TempAnimation->m_nodename = Parsing_String();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_Nodename = Parsing_String();
				break;
			default:
				break;
			}
			break;
		case TOKENR_NODE_PARENT:
			// 현 노드의 부모 노드의 정보.
			// 일단 입력을 하고, 나중에 정리하자.
			switch (m_parsingmode)
			{
			case eParsingmode::eGeomobject:
				m_OneMesh->m_NodeParent = Parsing_String();
				m_OneMesh->m_isparentexist = true;
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_NodeParent = Parsing_String();
				m_OneMesh->m_isparentexist = true;
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_NodeParent = Parsing_String();
				m_OneMesh->m_isparentexist = true;
				break;
			default:
				break;
			}
			break;

			/// NODE_TM

		case TOKENR_NODE_TM:
			//m_parsingmode	=	eGeomobject;
			// (NODE_TM으로 진입 후 NODE_NAME이 한번 더 나온다.)
			// (Animation과도 구별을 해야 하기 때문에 이렇게 모드를 적어준다)

			/// 게다가,
			// 카메라는 NodeTM이 두번 나온다. 두번째라면 넣지 않는다.
			// 아예 이 재귀에서 리턴을 시키고 싶지만. 중간에 읽는것을 무시해야 하기 때문에...
			//if (m_onemesh->m_camera_isloadTarget) {
			//	'}'이 나올때까지 나오는건 무조건 무시! 뭐 이런거 안되나..
			// 재귀구조의 문제점이다....

			break;

		case TOKENR_INHERIT_POS:
			// 카메라는 NodeTM이 두번 나온다. 두번째라면 넣지 않는다.
			break;
		case TOKENR_INHERIT_ROT:
			break;
		case TOKENR_INHERIT_SCL:
			break;
		case TOKENR_TM_ROW0:
			switch (m_parsingmode)
			{
			case eParsingmode::eGeomobject:
				m_OneMesh->m_TMRow0 = Parsing_NumberVector3();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_TMRow0 = Parsing_NumberVector3();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_TMRow0 = Parsing_NumberVector3();
				break;
			default:
				break;
			}
			break;
		case TOKENR_TM_ROW1:
			switch (m_parsingmode)
			{
			// TM_ROW1과 TM_ROW2, 그리고 y, z를 바꿔야 한다
			case eParsingmode::eGeomobject:
				m_OneMesh->m_TMRow2 = Parsing_NumberVector3();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_TMRow2 = Parsing_NumberVector3();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_TMRow2 = Parsing_NumberVector3();
				break;
			default:
				break;
			}
			break;
		case TOKENR_TM_ROW2:
			switch (m_parsingmode)
			{
			// TM_ROW1과 TM_ROW2, 그리고 y, z를 바꿔야 한다
			case eParsingmode::eGeomobject:
				m_OneMesh->m_TMRow1 = Parsing_NumberVector3();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_TMRow1 = Parsing_NumberVector3();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_TMRow1 = Parsing_NumberVector3();
				break;
			default:
				break;
			}
			break;
		case TOKENR_TM_ROW3:
			switch (m_parsingmode)
			{
			case eParsingmode::eGeomobject:
				m_OneMesh->m_TMRow3 = Parsing_NumberVector3();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_TMRow3 = Parsing_NumberVector3();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_TMRow3 = Parsing_NumberVector3();
				break;
			default:
				break;
			}
			break;
		case TOKENR_TM_POS:
			switch (m_parsingmode)
			{
			case eParsingmode::eGeomobject:
				m_OneMesh->m_tm_pos = Parsing_NumberVector3();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_tm_pos = Parsing_NumberVector3();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_tm_pos = Parsing_NumberVector3();
				break;
			default:
				break;
			}
			break;
		case TOKENR_TM_ROTAXIS:
			switch (m_parsingmode)
			{
			case eParsingmode::eGeomobject:
				m_OneMesh->m_tm_rotaxis = Parsing_NumberVector3();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_tm_rotaxis = Parsing_NumberVector3();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_tm_rotaxis = Parsing_NumberVector3();
				break;
			default:
				break;
			}
			break;
		case TOKENR_TM_ROTANGLE:
			switch (m_parsingmode)
			{
			case eParsingmode::eGeomobject:
				m_OneMesh->m_tm_rotangle = Parsing_NumberFloat();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_tm_rotangle = Parsing_NumberFloat();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_tm_rotangle = Parsing_NumberFloat();
				break;
			default:
				break;
			}
			break;
		case TOKENR_TM_SCALE:
			switch (m_parsingmode)
			{
			case eParsingmode::eGeomobject:
				m_OneMesh->m_tm_scale = Parsing_NumberVector3();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_tm_scale = Parsing_NumberVector3();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_tm_scale = Parsing_NumberVector3();
				break;
			default:
				break;
			}
			break;
		case TOKENR_TM_SCALEAXIS:
			switch (m_parsingmode)
			{
			case eParsingmode::eGeomobject:
				m_OneMesh->m_tm_scaleaxis = Parsing_NumberVector3();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_tm_scaleaxis = Parsing_NumberVector3();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_tm_scaleaxis = Parsing_NumberVector3();
				break;
			default:
				break;
			}
			break;
		case TOKENR_TM_SCALEAXISANG:
			switch (m_parsingmode)
			{
			case eParsingmode::eGeomobject:
				m_OneMesh->m_tm_scaleaxisang = Parsing_NumberFloat();
				break;
			case eParsingmode::eHelperObject:
				m_OneMesh->m_tm_scaleaxisang = Parsing_NumberFloat();
				break;
			case eParsingmode::eShape:
				m_OneMesh->m_tm_scaleaxisang = Parsing_NumberFloat();
				break;
			default:
				break;
			}
			// 현재 카메라 상태였다면 이미 노드를 읽은 것으로 표시해준다.
			break;


			/// MESH

		case TOKENR_MESH:
			//
			break;
		case TOKENR_TIMEVALUE:
			m_OneMesh->m_timevalue = Parsing_NumberFloat();
			break;
		case TOKENR_MESH_NUMBONE:
		{
			// 이게 있다면 이것은 Skinned Mesh라고 단정을 짓는다.
			// 내용 입력
			m_OneMesh->m_numbone = Parsing_NumberInt();
			m_OneMesh->m_IsSkinningObject = true;
		}
		break;

		case TOKENR_MESH_NUMSKINWEIGHT:
			break;
		case TOKENR_MESH_NUMVERTEX:
			m_OneMesh->m_mesh_numvertex = Parsing_NumberInt();
			break;
		case TOKENR_MESH_NUMFACES:
			m_OneMesh->m_mesh_numfaces = Parsing_NumberInt();
			break;

			/// MESH_VERTEX_LIST

		case TOKENR_MESH_VERTEX_LIST:
			//
			// 버텍스의 값들을 집어넣어야 하는데
			// 이미 벡터로 선언이 돼 있으므로 그냥 넣으면 된다.

			break;
		case TOKENR_MESH_VERTEX:
		{
			if (m_parsingmode == eParsingmode::eGeomobject
				|| m_parsingmode == eParsingmode::eHelperObject
				|| m_parsingmode == eParsingmode::eShape)
			{
				// 데이터 입력
				Vertex* _MeshVertex = new Vertex();
				_MeshVertex->m_indices = Parsing_NumberFloat();
				_MeshVertex->m_pos = Parsing_NumberVector3();
				m_OneMesh->m_meshvertex.push_back(_MeshVertex);

				// ==================== Collision Detection 추가 코드 ====================
				// 겸사겸사 최소/최대 버텍스도 비교해둔다
				// 최소 버텍스 찾기
				if (_MeshVertex->m_pos.x < m_MinVertexObj.x)
					m_MinVertexObj.x = _MeshVertex->m_pos.x;
				if (_MeshVertex->m_pos.y < m_MinVertexObj.y)
					m_MinVertexObj.y = _MeshVertex->m_pos.y;
				if (_MeshVertex->m_pos.z < m_MinVertexObj.z)
					m_MinVertexObj.z = _MeshVertex->m_pos.z;
				// 최대 버텍스 찾기
				if (_MeshVertex->m_pos.x > m_MaxVertexObj.x)
					m_MaxVertexObj.x = _MeshVertex->m_pos.x;
				if (_MeshVertex->m_pos.y > m_MaxVertexObj.y)
					m_MaxVertexObj.y = _MeshVertex->m_pos.y;
				if (_MeshVertex->m_pos.z > m_MaxVertexObj.z)
					m_MaxVertexObj.z = _MeshVertex->m_pos.z;
			}
			break;
		}

			/// Bone

		case TOKENR_SKIN_INITTM:
			break;
		case TOKENR_BONE_LIST:
			break;
		case TOKENR_BONE:
		{
			/// 모드 체인지 해 주고, Bone을 소유하고 있다는 것은 이것은 스키닝 오브젝트라는 것이다.
			// 본 하나를 만들어서 임시 포인터 보관, 벡터에 넣고
			// Bone의 넘버를 읽어 주자
			m_parsingmode = eObjectType_SkinnedMesh;	// 파싱 모드
			m_OneMesh->m_bone = new Bone();
			m_OneMesh->m_bone->m_bone_number = Parsing_NumberInt();
			m_OneMesh->m_BoneList.push_back(m_OneMesh->m_bone);
		}
		break;

		//이 다음에 본의 이름을 넣어야 한다. 하지만 {를 한 개 더 열었으므로 임시 포인터 변수로서 보관해야겠지.
		case TOKENR_BONE_NAME:
		{
			m_OneMesh->m_bone->m_BoneName = Parsing_String();
		}
		break;

		case TOKENR_BONE_PROPERTY:
			// 이 다음 ABSOLUTE가 나오기는 하는데, 쓸 일이 없다.
			break;
			// 다음에는 TM_ROW0~3이 나오는데 역시 무시됨..

		case TOKENR_MESH_WVERTEXS:
			break;

		case TOKENR_MESH_WEIGHT:
		{
			// 버텍스 하나의 정보를 만들어서 리스트에 넣음
			m_OneMesh->m_wvertex = new VertexWeight();
			m_OneMesh->m_wvertex->m_wvertex_number = Parsing_NumberInt();
			m_OneMesh->m_WVertexs.emplace_back(m_OneMesh->m_wvertex);
		}
		break;

		case TOKENR_BONE_BLENGING_WEIGHT:
		{
			// 대체 몇 단계를 들어가는거야...
			// 가중치 한개를 만들어서 리스트에 넣는다
			/// 헥 헥....
			m_OneMesh->m_wvertex->m_temp_bone_blend_weight = new Weight();
			m_OneMesh->m_wvertex->m_temp_bone_blend_weight->m_bone_number = Parsing_NumberInt();
			m_OneMesh->m_wvertex->m_temp_bone_blend_weight->m_bone_weight = Parsing_NumberFloat();
			m_OneMesh->m_wvertex->m_bone_blending_weight.push_back(m_OneMesh->m_wvertex->m_temp_bone_blend_weight);
		}
		break;


		/// MESH_FACE_LIST
		case TOKENR_MESH_FACE_LIST:
			//
			break;
		case TOKENR_MESH_FACE:
		{
			Face* _Mesh_Face = new Face();
			// Face의 번호인데...
			// Face의 번호를 담을 곳이 없으니 버린다
			// (벡터에 넣는 순서 == Face번호로 취급)
			Parsing_NumberInt();

			// A:를 읽고
			Parsing_String();
			_Mesh_Face->m_vertexindex[0] = Parsing_NumberInt();
			// B:
			Parsing_String();
			_Mesh_Face->m_vertexindex[1] = Parsing_NumberInt();
			// C:
			Parsing_String();
			_Mesh_Face->m_vertexindex[2] = Parsing_NumberInt();

			/// (뒤에 정보가 더 있지만 default에 의해 스킵될 것이다.)
			/// ......

			// 벡터에 넣어준다.
			m_OneMesh->m_meshface.push_back(_Mesh_Face);
		}
		break;


		case TOKENR_MESH_NUMTVERTEX:
			m_OneMesh->m_mesh_numtvertex = Parsing_NumberInt();
			break;

		case TOKENR_MESH_FACENORMAL:
		{
			_FaceNum = Parsing_NumberInt();
			m_OneMesh->m_meshface.at(_FaceNum)->m_normal = Parsing_NumberVector3();
			break;
		}
		/// MESH_VERTEXNORMAL
		case TOKENR_MESH_VERTEXNORMAL:
		{
			Face* _Face =  m_OneMesh->m_meshface.at(_FaceNum);
			Parsing_NumberInt();

			static int _NormalVertexNum = 0;
			_Face->m_normalvertex[_NormalVertexNum] = Parsing_NumberVector3();
			_NormalVertexNum++;
			if (_NormalVertexNum >= (sizeof(_Face->m_normalvertex) / sizeof(_Face->m_normalvertex[0])))
			{
				_NormalVertexNum = 0;
			}
		}
		break;
		/// MESH_TVERTLIST
		case TOKENR_MESH_TVERTLIST:
			//
			break;
		case TOKENR_MESH_TVERT:
		{
			// 버텍스의 인덱스가 나오는데 어차피 순서와 같으므로 버린다.
			Parsing_NumberInt();
			
			// 새로운 TVertex를 만들어서 벡터에 넣는다
			COneTVertex* _TVertex = new COneTVertex();
			_TVertex->m_u = Parsing_NumberFloat();
			// UV 좌표계를 고려해서 V축을 반전시킨다
			_TVertex->m_v = 1.0f - Parsing_NumberFloat();
			m_OneMesh->m_mesh_tvertex.push_back(_TVertex);
		}
		break;
		case TOKENR_MESH_NUMTVFACES:
			m_OneMesh->m_mesh_tvfaces = Parsing_NumberInt();
			break;
		case TOKENR_MESH_TFACE:
		{
			int _TFaceInx = Parsing_NumberInt();
			
			m_OneMesh->m_meshface.at(_TFaceInx)->m_TFace[0] = Parsing_NumberInt();
			m_OneMesh->m_meshface.at(_TFaceInx)->m_TFace[1] = Parsing_NumberInt();
			m_OneMesh->m_meshface.at(_TFaceInx)->m_TFace[2] = Parsing_NumberInt();
		}
		break;

		case TOKENR_MATERIAL_REF:
		{
			m_OneMesh->m_MaterialRef = Parsing_NumberInt();
		}
		break;

		case TOKENR_TM_ANIMATION:
		{
			m_parsingmode = eAnimation;	// 파싱 모드
			m_OneMesh->m_isAnimated = true;
			
			Create_animationdata_to_list();
			m_TempAnimation->m_ticksperFrame = m_scenedata.m_ticksperframe;
		}
		break;

		case TOKENR_MESH_ANIMATION:
		{
			m_parsingmode = eAnimation;	// 파싱 모드
		}
		break;

		case TOKENR_CONTROL_POS_TRACK:
			break;

		case TOKENR_CONTROL_POS_SAMPLE:
		{
			// ASE에서 위치 값은 절대값이니 그냥 파싱한다
			CAnimation_pos* _tmp = new CAnimation_pos();
			_tmp->m_time = Parsing_NumberInt();
			_tmp->m_pos = Parsing_NumberVector3();
			m_TempAnimation->m_position.push_back(_tmp);
		}
		break;
		
		case TOKENR_CONTROL_ROT_TRACK:
			break;

		case TOKENR_CONTROL_ROT_SAMPLE:
		{
			// ASE에서 회전 값은 사원수이며 누적 값이다
			// 즉, 이전 회전 값에 현재의 회전 값을 누적해야 한다
			// CONTROL_ROT_SAMPLE의 뒤에 3값이 AXIS값, 그리고 마지막값이 앵글 값
			// 출처: https://pros2.tistory.com/20
			// todo: 누적이 왜 사원수 간의 곱인지는 알아볼 것!
			// todo: 왜 사원수를 읽어 올 때 사인 코사인 값이 필요한지 알아볼 것!

			CAnimation_rot* _NowRot = new CAnimation_rot();
			_NowRot->m_time = Parsing_NumberInt();
			_NowRot->m_rot = Parsing_NumberVector3(); // y, z 값을 반대로
			_NowRot->m_angle = Parsing_NumberFloat();

			float x, y, z, angle;
			x = _NowRot->m_rot.x;
			y = _NowRot->m_rot.y;
			z = _NowRot->m_rot.z;
			angle = _NowRot->m_angle;
			
			// todo: 왜 사원수를 읽어 올 때 사인 코사인 값이 필요한지 알아볼 것!
			// todo: 아래의 연산은 XMQuaternionToAxisAngle로 대체할 수 있나?
			tempQT.x = sinf(angle / 2.f) * x;
			tempQT.y = sinf(angle / 2.f) * y;
			tempQT.z = sinf(angle / 2.f) * z;
			tempQT.w = cosf(angle / 2.f);

			// 이전 회전 값이 있으면 누적 값을 구한다
			if (m_TempAnimation->m_rotation.size() == 0)
			{
				_NowRot->m_rotQT_accumulation = tempQT;
			}
			else
			{
				// todo: 누적이 왜 사원수 간의 곱인지는 알아볼 것!
				prevQT = m_TempAnimation->m_rotation.back()->m_rotQT_accumulation;
				_NowRot->m_rotQT_accumulation = prevQT * tempQT;
			}
			m_TempAnimation->m_rotation.push_back(_NowRot);
		}
		break;

		case TOKEND_END:
			// 아마도 이건 파일의 끝이 나타났을때인것 같은데. while을 탈출해야 하는데?

			//AfxMessageBox("파일의 끝을 본 것 같습니다!", MB_OK, NULL);
			TRACE("TRACE: 파싱중: 파일의 끝을 봤습니다!\n");
			return;

			break;

			/// 위의 아무것도 해당하지 않을때
		default:
			// 아무것도 하지 않는다.
			break;

		}	// switch()


		///-----------------------------------------------
		/// 안전 코드.
		i++;
		if (i > 1000000)
		{
			// 루프를 1000000번이상이나 돌 이유가 없다. (데이터가 100000개가 아닌이상)
			// 만약 1000000이상 돌았다면 확실히 뭔가 문제가 있는 것이므로
			TRACE("루프를 백만번 돌았습니다!");
			return;
		}
		/// 안전 코드.
		///-----------------------------------------------

	}		// while()

	// 여기까지 왔다면 while()을 벗어났다는 것이고 그 말은
	// 괄호를 닫았다는 것이므로
	// 리턴하자 (재귀함수)

	return;
}


///----------------------------------------------------------------------
/// parsing을 위한 단위별 함수들
///----------------------------------------------------------------------

// long을 읽어서 리턴해준다.
int CASEParser::Parsing_NumberLong()
{
	LONG			token;
	LONG			tempNumber;

	token = m_lexer->GetToken(m_TokenString);	//ASSERT(token == TOKEND_NUMBER);
	tempNumber = strtoul(m_TokenString, NULL, 10);

	return			tempNumber;
}

// float
float CASEParser::Parsing_NumberFloat()
{
	LONG			token;
	float			tempNumber;

	token = m_lexer->GetToken(m_TokenString);	//ASSERT(token == TOKEND_NUMBER);
	tempNumber = (float)atof(m_TokenString);

	return			tempNumber;
}

// String
LPSTR CASEParser::Parsing_String()
{
	/// ※m_TokenString ( char[255] ) 이기 때문에 CString에 넣으면 에러 날거라 생각했는데, 생각보다 CString은 잘 만들어진 것 같다. 알아서 받아들이는데?
	m_lexer->GetToken(m_TokenString);

	return m_TokenString;
}


// int
int CASEParser::Parsing_NumberInt() {

	LONG			token;
	int				tempNumber;

	token = m_lexer->GetToken(m_TokenString);	//ASSERT(token == TOKEND_NUMBER);
	tempNumber = (int)atoi(m_TokenString);

	return			tempNumber;
}


// 3개의 Float를 벡터 하나로
Vector3 CASEParser::Parsing_NumberVector3()
{
	LONG				token;
	Vector3			tempVector3;

	token = m_lexer->GetToken(m_TokenString);
	tempVector3.x = (float)atof(m_TokenString);
	token = m_lexer->GetToken(m_TokenString);
	tempVector3.z = (float)atof(m_TokenString);
	token = m_lexer->GetToken(m_TokenString);
	tempVector3.y = (float)atof(m_TokenString);

	return			tempVector3;		// 스태틱 변수의 레퍼런스보다는 값 전달을 하자.
}

///--------------------------------------------------
/// 내부에서 뭔가를 생성, 리스트에 넣는다
///--------------------------------------------------
// 메시를 하나 동적으로 생성하고, 그 포인터를 리스트에 넣는다.
void CASEParser::Create_onemesh_to_list()
{
	Mesh* temp = new Mesh;
	m_OneMesh = temp;
	m_OneMesh->m_scenedata = m_scenedata;		// 클래스간 값 복사
	m_MeshList.emplace_back(m_OneMesh);
}

// 메트리얼 하나를 동적으로 생성하고, 그 포인터를 리스트에 넣는다.
void CASEParser::Create_materialdata_to_list()
{
	ASEMaterial* temp = new ASEMaterial;
	m_materialdata = temp;
	m_MaterialList.emplace_back(m_materialdata);
}

// 애니메이션데이터 하나를 동적으로 생성하고, 그 포인터를 리스트에 넣는다.
void CASEParser::Create_animationdata_to_list()
{
	Animation* temp = new Animation;
	m_TempAnimation = temp;
	//m_AnimationList.emplace_back(m_TempAnimation);
	m_OneMesh->m_TMAnimation = m_TempAnimation;
}

// 정점 하나를..
void CASEParser::Create_onevertex_to_list()
{
	Vertex* temp = new Vertex;
	m_OneMesh->m_meshvertex.emplace_back(temp);
}

















