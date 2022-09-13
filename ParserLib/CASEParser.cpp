#pragma once
#include "CASEParser.h"

#include <filesystem>

// �̷� ������ �߰��ϴ� �� �³�...
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
// �ε��Ѵ�.
// �̰��� �����ٸ� ������ ���������� ��� �����͸� �о �� �־�� �Ѵ�.
//
// Parsing�� ����:
// �׻� ������ �������� parsing�̶�� ���� �����Ͱ� ���� �԰ݿ� �°� �� �ִٴ� ���� ������ �Ѵ�.
// ������, ���� ���ο� ������ �ٲ�� �� �ִ°��� �����ϴٴ��� �ϴ� ��Ģ�� �����
// �˻��ϴ� ��ƾ�� ���������� ����. �ϴ��� ������� ���� �Ծ����� �� �ִٴ°��� ��������.
// -> �׷���, ��� ȣ���� �ϴ� �ϳ��� ū �Լ��� �ذ�ô�.
// -> depth�� �������� �����ۿ� ���� ���� �ڵ带 �־�߰ڴ�
//---------------------------------------------------------------------------------------------------
bool CASEParser::Load(LPSTR p_File)
{
	/// 0) ������ �ε��Ѵ�.
	if (!m_lexer->Open(p_File))
	{
		TRACE("������ ���� �߿� ������ �߻��߽��ϴ�!");
		return FALSE;
	}

	/// 1) �ٷ��̾ƺ����豳���� �Ŀ�Ǯ�� ����Լ� �Ѱ��� ó�� ��!
	Parsing_DivergeRecursiveALL(0);

	m_lexer->Close();

	// ����ȭ�� ������
	for (auto& item : m_MeshList)
	{
		this->Convert_Optimize(item);
	}

	return TRUE;
}

/// 1�� ��ȯ

// CScenedata ����
bool CASEParser::TranslateToD3DFormat_scene(Scenedata* pscene)
{
	// �� ����
	*pscene = this->m_scenedata;
	return TRUE;
}

/// <summary>
/// �޽��� ����ȭ�� �� �ش�.
/// �븻��, �ؽ��� ��ǥ�� ���� ���ؽ��� �ø���, ��ø�Ǵ°��� �����ϰ�..
/// </summary>
bool CASEParser::Convert_Optimize(Mesh* pMesh)
{
	// ����ȭ�� ���� Mesh�� ���� ��ŵ�Ѵ�
	if (pMesh->m_opt_vertex.size() != 0)
		return true;

	// ==================== Animation �߰� �ڵ� ====================
	// �װ�Ƽ�� ������(Negative Scale)�� �ذ�����
	// �װ�Ƽ�� ������ �Ǻ� ���
	//   ���1. ȸ�� ����� ��Ľ��� -1�̸� �װ�Ƽ�� �������̴�
	//   ���2. TM_ROW�� ������ ����� 1��(x��), 2��(y��), 3��(z��) �� �� ���� ������ ����
	//          ������ �� ��� �ݴ� �����̸� �װ�Ƽ�� �������̴�(�������� �Ǻ� ����)
	//   ���3. ASE�� Decompose�� �̵�, ȸ��, ũ�� �����ͷ� ��ȯ ����� �����ϰ�,
	//          TM_ROW�� ������ ����� ���� ���� �����ؼ� 0���� ���� ���� ������ �װ�Ƽ�� �������̴�
	// 
	// �� ������Ʈ���� ���2�� ����ϱ�� �Ѵ�
	// ���2�� �װ�Ƽ�� �������� �Ǻ����� ���(��Ȯ���� TM_ROW�� ������ ����� ���)
	//   ��İ� ��� ���� -1�� ���ؾ� �Ѵ�
	// ����� Decompose�� �����ͷ� ��ȯ ����� ������ ���� ��� ������ -1�� ���ϸ� �ȴ�

	// 1��(x��), 2��(y��)�� ����
	XMVECTOR _tmpRes = XMVector3Cross(pMesh->m_TMRow0, pMesh->m_TMRow1);
	// 1��(x��), 2��(y��)�� ���� ���� 3��(z��)�� ����
	_tmpRes = XMVector3Dot(_tmpRes, pMesh->m_TMRow2);

	// ���� ���� 0���� ���� ���� �װ�Ƽ�� �������̴�
	if (_tmpRes.m128_f32[0] < 0)
	{
		// ��Ŀ� -1�� ���Ѵ�
		pMesh->m_TMRow0 = pMesh->m_TMRow0 * -1;
		pMesh->m_TMRow1 = pMesh->m_TMRow1 * -1;
		pMesh->m_TMRow2 = pMesh->m_TMRow2 * -1;

		// ��� ���� -1�� ���Ѵ�
		for (auto& item : pMesh->m_meshface)
		{
			item->m_normal *= -1;
			item->m_normalvertex[0] *= -1;
			item->m_normalvertex[1] *= -1;
			item->m_normalvertex[2] *= -1;
		}
	}

	// 2021.04.12
	// ���ؽ����� �ϴ� ��� ����
	for (unsigned int i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		// ==================== Skinnig �߰� �ڵ� ====================
		// �Ľ��� �� ����ġ ������ �ִ� ��� Vertext�� �־��ش�
		if (pMesh->m_WVertexs.size() != 0)
		{
			VertexWeight* _tmp = pMesh->m_WVertexs[i];
			// ����ġ�� �ش� ���� ������ ����
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

		// ==================== Collision Detection �߰� �ڵ� ====================
		// ����� �ּ�/�ִ� ���ؽ��� ���صд�
		// �ּ� ���ؽ� ã��
		if (_pVertex->m_pos.x < pMesh->m_MinVertexOneMesh.x)
			pMesh->m_MinVertexOneMesh.x = _pVertex->m_pos.x;
		if (_pVertex->m_pos.y < pMesh->m_MinVertexOneMesh.y)
			pMesh->m_MinVertexOneMesh.y = _pVertex->m_pos.y;
		if (_pVertex->m_pos.z < pMesh->m_MinVertexOneMesh.z)
			pMesh->m_MinVertexOneMesh.z = _pVertex->m_pos.z;
		// �ִ� ���ؽ� ã��
		if (_pVertex->m_pos.x > pMesh->m_MaxVertexOneMesh.x)
			pMesh->m_MaxVertexOneMesh.x = _pVertex->m_pos.x;
		if (_pVertex->m_pos.y > pMesh->m_MaxVertexOneMesh.y)
			pMesh->m_MaxVertexOneMesh.y = _pVertex->m_pos.y;
		if (_pVertex->m_pos.z > pMesh->m_MaxVertexOneMesh.z)
			pMesh->m_MaxVertexOneMesh.z = _pVertex->m_pos.z;
	}

	// �Ľ� �� ã�Ƶ� ��ü ������Ʈ�� �ּ�/�ִ� ���ؽ��� �Ѱ��ش�
	pMesh->m_MinVertexObj = m_MinVertexObj;
	pMesh->m_MaxVertexObj = m_MaxVertexObj;

	// �������� face�� �������� �� �ε����� ã�ƾ� ��
	unsigned int _faceCount = pMesh->m_meshface.size();
	TRACE("FaceCount : %d", _faceCount);

#pragma region ��� �� �������� ���ؽ� �ɰ���
	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		//TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// face�������� ���ؽ��� �븻�� �־��ش�.
			if (!_nowVertex->m_isnormalset)
			{
				_nowVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];
				_nowVertex->m_isnormalset = true;
			}
			// ���ؽ��� ��� ���� �̹� �Է� �Ǿ����� ���ؽ��� �߰��Ѵ�
			else
			{
				// ��� ���� �ߺ��Ǵ� ���ؽ��� �̹� �ִ� ��쿡�� ��ŵ�Ѵ�
				// ���� ���ؽ� ��ǥ ���� �����Ǿ� �־����� ���� ó���� ��ŵ�Ѵ�
				if (_nowVertex->m_normal == pMesh->m_meshface[i]->m_normalvertex[j])
				{
					continue;
				}

				// ���ο� ���ؽ� �߰�
				int _AddVertexIdx = pMesh->m_opt_vertex.size();
				Vertex* _addVertex = new Vertex();
				pMesh->m_opt_vertex.push_back(_addVertex);
				// �ε��� ��ü
				pMesh->m_meshface[i]->m_vertexindex[j] = _AddVertexIdx;

				// ��ǥ���� ��ġ��
				_addVertex->m_pos = _nowVertex->m_pos;
				// �븻 ���� ���� ������ ���� �ִ´�
				//_addVertex->m_normal = pMesh->m_meshface[i]->m_normal;	// Face Normal ��
				_addVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];
				// �븻���� ���õƴ°�?
				_addVertex->m_isnormalset = false;
				// �ؽ��� ��ǥ
				_addVertex->u = _nowVertex->u;
				_addVertex->v = _nowVertex->v;
				// �ؽ��İ��� ���õƴ°�?
				_addVertex->m_IsTextureSet = false;
				/// Skinned Mesh�� weight
				_addVertex->m_bw1 = _nowVertex->m_bw1;
				_addVertex->m_bw2 = _nowVertex->m_bw2;
				_addVertex->m_bw3 = _nowVertex->m_bw3;
				_addVertex->m_bw4 = _nowVertex->m_bw4;
				// �ε���
				_addVertex->m_indices = _AddVertexIdx;
			}
		}
	}
#pragma endregion

#pragma region �ؽ�ó ��ǥ �������� ���ؽ� �ɰ���
	// TFACE�� �������� ���ؽ��� �ؽ�ó �� �����ϱ�
	unsigned int _TFaceCount = pMesh->m_mesh_tvfaces;
	TRACE("TFaceCount : %d", _TFaceCount);

	// TFace�� ������ Face ����ü�� ����
	// TFace�� �� == Face�� ���̹Ƿ� TFace��ŭ �����ϸ鼭
	// Face���� �ؽ�ó ��ǥ�� ȹ���ؼ� �����Ѵ�
	for (unsigned int i = 0; i < _TFaceCount; i++)
	{
		//TRACE("TFaceNum : %d / %d\n", i, _TFaceCount);

		// Face ����ü�� ����
		Face* _nowTFace = pMesh->m_meshface[i];

		for (int j = 0; j < 3; j++)
		{
			// TFace�� TVertext�� Face�� Vertext�� 1 �� 1 ���� ����
			// TFace�� �̷�� TVertext �� �ϳ��� �ε����� ȹ��
			int _TVertexIdx = _nowTFace->m_TFace[j];
			// Face�� �̷�� Vertext �� �ϳ��� �ε����� ȹ��
			int _nowVertexIdx = _nowTFace->m_vertexindex[j];

			// ȹ���� Vertext�� �ε����� ���� ���ؽ��� ����
			Vertex* _nowVertex = pMesh->m_opt_vertex[_nowVertexIdx];

			// ���ؽ��� �ؽ�ó ��ǥ�� �����Ѵ�
			if (!_nowVertex->m_IsTextureSet)
			{
				_nowVertex->u = pMesh->m_mesh_tvertex[_TVertexIdx]->m_u;
				_nowVertex->v = pMesh->m_mesh_tvertex[_TVertexIdx]->m_v;
				_nowVertex->m_IsTextureSet = true;
			}
			// ���ؽ��� �ؽ�ó ��ǥ ���� �̹� �Է� �Ǿ����� ���ؽ��� �߰��Ѵ�
			else
			{
				// ���� ���ؽ� ��ǥ ���� �����Ǿ� �־����� ���� ó���� ��ŵ�Ѵ�
				if (_nowVertex->u == pMesh->m_mesh_tvertex[_TVertexIdx]->m_u
					&& _nowVertex->v == pMesh->m_mesh_tvertex[_TVertexIdx]->m_v)
				{
					continue;
				}

				// ���ο� ���ؽ� �߰�
				int _AddVertexIdx = pMesh->m_opt_vertex.size();
				pMesh->m_opt_vertex.emplace_back(new Vertex());
				Vertex* _addVertex = pMesh->m_opt_vertex.back();
				// �ε��� ��ü
				pMesh->m_meshface[i]->m_vertexindex[j] = _AddVertexIdx;

				// ��ǥ���� ��ġ��
				_addVertex->m_pos = _nowVertex->m_pos;
				// �븻 ��
				_addVertex->m_normal = _nowVertex->m_normal;
				// �븻���� ���õƴ°�?
				_addVertex->m_isnormalset = false;
				// �ؽ��� ��ǥ
				_addVertex->u = pMesh->m_mesh_tvertex[_TVertexIdx]->m_u;
				_addVertex->v = pMesh->m_mesh_tvertex[_TVertexIdx]->m_v;
				// �ؽ��İ��� ���õƴ°�?
				_addVertex->m_IsTextureSet = false;
				/// Skinned Mesh�� weight
				_addVertex->m_bw1 = _nowVertex->m_bw1;
				_addVertex->m_bw2 = _nowVertex->m_bw2;
				_addVertex->m_bw3 = _nowVertex->m_bw3;
				// �ε���
				_addVertex->m_indices = _AddVertexIdx;
			}
		}
	}
#pragma endregion

	// �ε����� �׳� ����
	// (2��° 3��° ��ü�� MeshObject���� ����)
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

#pragma region ���̽��� ź��Ʈ �����̽� ���� ����ϱ�
	// ==================== Normal Mapping �߰� �ڵ� ====================
	for (int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		// �ﰢ���� �̷�� ���ؽ����� �ε���
		int idx0 = pMesh->m_opt_index[i].index[0];
		int idx1 = pMesh->m_opt_index[i].index[1];
		int idx2 = pMesh->m_opt_index[i].index[2];

		// �ﰢ���� �̷�� ���ؽ����� ������
		Vector3 p0 = pMesh->m_opt_vertex[idx0]->m_pos;
		Vector3 p1 = pMesh->m_opt_vertex[idx1]->m_pos;
		Vector3 p2 = pMesh->m_opt_vertex[idx2]->m_pos;

		// �ﰢ���� �� ���� ����
		Vector3 e1 = p1 - p0;
		Vector3 e2 = p2 - p0;

		// �ﰢ���� �� ���� �ؽ�ó������ ����
		float x1 = pMesh->m_opt_vertex[idx1]->u - pMesh->m_opt_vertex[idx0]->u;
		float y1 = pMesh->m_opt_vertex[idx1]->v - pMesh->m_opt_vertex[idx0]->v;
		float x2 = pMesh->m_opt_vertex[idx2]->u - pMesh->m_opt_vertex[idx0]->u;
		float y2 = pMesh->m_opt_vertex[idx2]->v - pMesh->m_opt_vertex[idx0]->v;

		// ��Ľ�
		float det = (x1 * y2 - x2 * y1);
		// ��Ľ��� 0�̸� ���� �� �Է�
		if (det == 0) det = 0.00001f;

		// ��Ľ��� 0�� �ƴϸ� ������ ����
		float detInv = 1.0f / det;

		// ź��Ʈ �����̽��� ������ ����
		// (Normalize�� ���߿� ���ؽ��� ź���� �����̽��� ��ճ� �� ��)
		Vector3 T = detInv * (y2 * e1 - y1 * e2);
		//Vector3 B = detInv * (y2 * e1 - y1 * e2);	// ���̴����� ����

		//
		//float tmp = T.y;
		//T.y = T.z;
		//T.z = tmp;

		pMesh->m_opt_vertex[idx0]->m_tangent += T;
		pMesh->m_opt_vertex[idx1]->m_tangent += T;
		pMesh->m_opt_vertex[idx2]->m_tangent += T;
	}
#pragma endregion ���̽��� ź��Ʈ �����̽� ���� ����ϱ�

#pragma region ���ؽ��� ź��Ʈ �����̽� ���� ����ȭ(���)�ϱ�
	for (auto& item : pMesh->m_opt_vertex)
	{
		// �׶� ����Ʈ �˰���(Gram-Schmitdt orthonormalization)
		Vector3 N = item->m_normal;
		Vector3 T = item->m_tangent;
		
		N *= T.Dot(N);
		T -= N;
		item->m_tangent = XMVector3Normalize(T);
	}
#pragma endregion ���ؽ��� ź��Ʈ �����̽� ���� ����ȭ(���)�ϱ�

	return TRUE;
}

bool CASEParser::ConvertAll(Mesh* pMesh)
{
	// ���ؽ����� �ϴ� ��� ����
	for (unsigned int i = 0; i < pMesh->m_meshvertex.size(); i++)
	{
		Vertex* _pVertex = new Vertex;
		_pVertex->m_pos = pMesh->m_meshvertex[i]->m_pos;

		pMesh->m_opt_vertex.push_back(_pVertex);
	}

	// �������� face�� �������� �� �ε����� ã�ƾ� ��
	unsigned int _faceCount = pMesh->m_meshface.size();
	TRACE("FaceCount : %d", _faceCount);

	for (unsigned int i = 0; i < pMesh->m_meshface.size(); i++)
	{
		TRACE("FaceNum : %d / %d\n", i, _faceCount);

		for (int j = 0; j < 3; j++)
		{
			Vertex* _nowVertex = pMesh->m_opt_vertex[pMesh->m_meshface[i]->m_vertexindex[j]];

			// face�������� ���ؽ��� �븻�� �־��ش�.
			// (���ÿ�)
			//_nowVertex->m_normal.x = 0;
			//_nowVertex->m_normal.y = 0;
			//_nowVertex->m_normal.z = 0;
			_nowVertex->m_normal = pMesh->m_meshface[i]->m_normalvertex[j];
		}
	}

	// �ε����� �׳� ����
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
// ��� ȣ����� ������ �ϴ� �б� �Լ��̴�.
//
// �ϴ� �а�, ��ū�� ���� �����Ѵ�.
//
// ��͸� ���� ������ { �� ���� ����̴�.
// �����ϴ� ������ '}'�� �����ų� TOKEND_END�� ������ ���̴�.
//
// ���Ҿ�, ��͸� ���� �� ��ū�� �о�, ���� �� �Լ��� � ��������� �����Ѵ�.
//
//----------------------------------------------------------------
void CASEParser::Parsing_DivergeRecursiveALL(int depth)
{
	//----------------------------------------------------------------------
	// ���������� ����
	// ��� �����ٰ� ���� �̻� ������ ���� ���(����)�� üũ�ϱ� ���� ����
	int i = 0;
	// ����Ʈ�� �����͸� �־�� �� �� ���̴� ����Ʈ ī����.
	int listcount = 0;		// �ſ� C����� �ٸ� �ذ�å�� ������?
	// �̰��� �� ��ū�� � �����ΰ��� �Ǻ����ش�.
	// �̰��� ��� ������ �ϰ� ��͸� �����°��� �����ϴ�? -> �߸��� ��͹���̴�?
	LONG nowtoken;
	//----------------------------------------------------------------------

	Matrix TM1, TM2;
	Quaternion tempQT;
	Quaternion prevQT;
	Quaternion resultQT;

	/// �̰��� �ϸ� �� ���� ��ū�� �а�, �� ������ ������ �� �� �ִ�.
	while (nowtoken = m_lexer->GetToken(m_TokenString), nowtoken != TOKEND_BLOCK_END)
	{
		// �ϴ� �� ���� ��ū�� �а�, �װ��� ��ȣ �ݱⰡ �ƴ϶��.
		// �Ѿ�� ��ū�� ���� ó�����ش�.

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
			Parsing_String();	// �׳� m_TokenString�� �о������ ���� ��.
			//AfxMessageBox( m_TokenString, NULL, NULL);		/// �ӽ÷� �ڸ�Ʈ�� ����غ���
			break;

			//--------------------
			// SCENE
			//--------------------

		case TOKENR_SCENE:
			//
			break;
		case TOKENR_SCENE_FILENAME:
			m_scenedata.m_filename = Parsing_String();		// �ϰ��� �ִ� �Լ��� ����� ���� String�� Int�� ��������.
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
			// �׳� ���� ������ �о������ }�� ���ö�����
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
			// RGB ���̶� �ϳ��ϳ� ���� �־��ش�
			m_materialdata->m_MaterialAmbient.x = Parsing_NumberFloat();
			m_materialdata->m_MaterialAmbient.y = Parsing_NumberFloat();
			m_materialdata->m_MaterialAmbient.z = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_DIFFUSE:
		{
			// RGB ���̶� �ϳ��ϳ� ���� �־��ش�
			m_materialdata->m_MaterialDiffuse.x = Parsing_NumberFloat();
			m_materialdata->m_MaterialDiffuse.y = Parsing_NumberFloat();
			m_materialdata->m_MaterialDiffuse.z = Parsing_NumberFloat();
		}
		break;

		case TOKENR_MATERIAL_SPECULAR:
		{
			// RGB ���̶� �ϳ��ϳ� ���� �־��ش�
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

		// TOKENR_MAP_BUMP�� ���� �ʿ� �����Ƿ� �Ľ��� ������
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
			// ���ϸ� ������
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
			//	�� ���� �׷� ����. �� ������ �̸��� ��Ʈ������ ������� �ϴµ�.
			break;

		case TOKENR_HELPEROBJECT:
			// �ϴ� �����ϰ�
			// ������Ʈ�� Ÿ�� ������. �̰Ϳ� ���� ���� �ٸ� �Ľ� ��� �ߵ�.
		{
			m_parsingmode = eHelperObject;	// �Ľ� ���
			Create_onemesh_to_list();
			m_OneMesh->m_type = eObjectType_HelperObject;	// ���� ������Ʈ
		}
		break;

		case TOKENR_GEOMOBJECT:
			/// �� ��ū�� �����ٴ°� ���ο� �޽ð� ����ٴ� ���̴�. ���� ������ mesh�� �ϳ� ����, �� �����͸� ����Ʈ�� �ְ�, m_onemesh�� �� �����͸� ����, �״�� ���� �ɱ�?
		{
			m_parsingmode = eGeomobject;
			Create_onemesh_to_list();
			m_OneMesh->m_type = eObjectType_Geomobject;		// Geom ������Ʈ
		}
		break;

		case TOKENR_SHAPEOBJECT:
		{
			m_parsingmode = eShape;
			Create_onemesh_to_list();
			m_OneMesh->m_type = eObjectType_Shape;		// Geom ������Ʈ
		}
		break;
		

		case TOKENR_NODE_NAME:
			// ��� ������ ������Ʈ���� ���� �� �� �ִ� ������ ���̴�.
			// ��忡 ���� �־�� �� ���� �ٸ���.
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
			// �� ����� �θ� ����� ����.
			// �ϴ� �Է��� �ϰ�, ���߿� ��������.
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
			// (NODE_TM���� ���� �� NODE_NAME�� �ѹ� �� ���´�.)
			// (Animation���� ������ �ؾ� �ϱ� ������ �̷��� ��带 �����ش�)

			/// �Դٰ�,
			// ī�޶�� NodeTM�� �ι� ���´�. �ι�°��� ���� �ʴ´�.
			// �ƿ� �� ��Ϳ��� ������ ��Ű�� ������. �߰��� �д°��� �����ؾ� �ϱ� ������...
			//if (m_onemesh->m_camera_isloadTarget) {
			//	'}'�� ���ö����� �����°� ������ ����! �� �̷��� �ȵǳ�..
			// ��ͱ����� �������̴�....

			break;

		case TOKENR_INHERIT_POS:
			// ī�޶�� NodeTM�� �ι� ���´�. �ι�°��� ���� �ʴ´�.
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
			// TM_ROW1�� TM_ROW2, �׸��� y, z�� �ٲ�� �Ѵ�
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
			// TM_ROW1�� TM_ROW2, �׸��� y, z�� �ٲ�� �Ѵ�
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
			// ���� ī�޶� ���¿��ٸ� �̹� ��带 ���� ������ ǥ�����ش�.
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
			// �̰� �ִٸ� �̰��� Skinned Mesh��� ������ ���´�.
			// ���� �Է�
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
			// ���ؽ��� ������ ����־�� �ϴµ�
			// �̹� ���ͷ� ������ �� �����Ƿ� �׳� ������ �ȴ�.

			break;
		case TOKENR_MESH_VERTEX:
		{
			if (m_parsingmode == eParsingmode::eGeomobject
				|| m_parsingmode == eParsingmode::eHelperObject
				|| m_parsingmode == eParsingmode::eShape)
			{
				// ������ �Է�
				Vertex* _MeshVertex = new Vertex();
				_MeshVertex->m_indices = Parsing_NumberFloat();
				_MeshVertex->m_pos = Parsing_NumberVector3();
				m_OneMesh->m_meshvertex.push_back(_MeshVertex);

				// ==================== Collision Detection �߰� �ڵ� ====================
				// ����� �ּ�/�ִ� ���ؽ��� ���صд�
				// �ּ� ���ؽ� ã��
				if (_MeshVertex->m_pos.x < m_MinVertexObj.x)
					m_MinVertexObj.x = _MeshVertex->m_pos.x;
				if (_MeshVertex->m_pos.y < m_MinVertexObj.y)
					m_MinVertexObj.y = _MeshVertex->m_pos.y;
				if (_MeshVertex->m_pos.z < m_MinVertexObj.z)
					m_MinVertexObj.z = _MeshVertex->m_pos.z;
				// �ִ� ���ؽ� ã��
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
			/// ��� ü���� �� �ְ�, Bone�� �����ϰ� �ִٴ� ���� �̰��� ��Ű�� ������Ʈ��� ���̴�.
			// �� �ϳ��� ���� �ӽ� ������ ����, ���Ϳ� �ְ�
			// Bone�� �ѹ��� �о� ����
			m_parsingmode = eObjectType_SkinnedMesh;	// �Ľ� ���
			m_OneMesh->m_bone = new Bone();
			m_OneMesh->m_bone->m_bone_number = Parsing_NumberInt();
			m_OneMesh->m_BoneList.push_back(m_OneMesh->m_bone);
		}
		break;

		//�� ������ ���� �̸��� �־�� �Ѵ�. ������ {�� �� �� �� �������Ƿ� �ӽ� ������ �����μ� �����ؾ߰���.
		case TOKENR_BONE_NAME:
		{
			m_OneMesh->m_bone->m_BoneName = Parsing_String();
		}
		break;

		case TOKENR_BONE_PROPERTY:
			// �� ���� ABSOLUTE�� ������� �ϴµ�, �� ���� ����.
			break;
			// �������� TM_ROW0~3�� �����µ� ���� ���õ�..

		case TOKENR_MESH_WVERTEXS:
			break;

		case TOKENR_MESH_WEIGHT:
		{
			// ���ؽ� �ϳ��� ������ ���� ����Ʈ�� ����
			m_OneMesh->m_wvertex = new VertexWeight();
			m_OneMesh->m_wvertex->m_wvertex_number = Parsing_NumberInt();
			m_OneMesh->m_WVertexs.emplace_back(m_OneMesh->m_wvertex);
		}
		break;

		case TOKENR_BONE_BLENGING_WEIGHT:
		{
			// ��ü �� �ܰ踦 ���°ž�...
			// ����ġ �Ѱ��� ���� ����Ʈ�� �ִ´�
			/// �� ��....
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
			// Face�� ��ȣ�ε�...
			// Face�� ��ȣ�� ���� ���� ������ ������
			// (���Ϳ� �ִ� ���� == Face��ȣ�� ���)
			Parsing_NumberInt();

			// A:�� �а�
			Parsing_String();
			_Mesh_Face->m_vertexindex[0] = Parsing_NumberInt();
			// B:
			Parsing_String();
			_Mesh_Face->m_vertexindex[1] = Parsing_NumberInt();
			// C:
			Parsing_String();
			_Mesh_Face->m_vertexindex[2] = Parsing_NumberInt();

			/// (�ڿ� ������ �� ������ default�� ���� ��ŵ�� ���̴�.)
			/// ......

			// ���Ϳ� �־��ش�.
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
			// ���ؽ��� �ε����� �����µ� ������ ������ �����Ƿ� ������.
			Parsing_NumberInt();
			
			// ���ο� TVertex�� ���� ���Ϳ� �ִ´�
			COneTVertex* _TVertex = new COneTVertex();
			_TVertex->m_u = Parsing_NumberFloat();
			// UV ��ǥ�踦 ����ؼ� V���� ������Ų��
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
			m_parsingmode = eAnimation;	// �Ľ� ���
			m_OneMesh->m_isAnimated = true;
			
			Create_animationdata_to_list();
			m_TempAnimation->m_ticksperFrame = m_scenedata.m_ticksperframe;
		}
		break;

		case TOKENR_MESH_ANIMATION:
		{
			m_parsingmode = eAnimation;	// �Ľ� ���
		}
		break;

		case TOKENR_CONTROL_POS_TRACK:
			break;

		case TOKENR_CONTROL_POS_SAMPLE:
		{
			// ASE���� ��ġ ���� ���밪�̴� �׳� �Ľ��Ѵ�
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
			// ASE���� ȸ�� ���� ������̸� ���� ���̴�
			// ��, ���� ȸ�� ���� ������ ȸ�� ���� �����ؾ� �Ѵ�
			// CONTROL_ROT_SAMPLE�� �ڿ� 3���� AXIS��, �׸��� ���������� �ޱ� ��
			// ��ó: https://pros2.tistory.com/20
			// todo: ������ �� ����� ���� �������� �˾ƺ� ��!
			// todo: �� ������� �о� �� �� ���� �ڻ��� ���� �ʿ����� �˾ƺ� ��!

			CAnimation_rot* _NowRot = new CAnimation_rot();
			_NowRot->m_time = Parsing_NumberInt();
			_NowRot->m_rot = Parsing_NumberVector3(); // y, z ���� �ݴ��
			_NowRot->m_angle = Parsing_NumberFloat();

			float x, y, z, angle;
			x = _NowRot->m_rot.x;
			y = _NowRot->m_rot.y;
			z = _NowRot->m_rot.z;
			angle = _NowRot->m_angle;
			
			// todo: �� ������� �о� �� �� ���� �ڻ��� ���� �ʿ����� �˾ƺ� ��!
			// todo: �Ʒ��� ������ XMQuaternionToAxisAngle�� ��ü�� �� �ֳ�?
			tempQT.x = sinf(angle / 2.f) * x;
			tempQT.y = sinf(angle / 2.f) * y;
			tempQT.z = sinf(angle / 2.f) * z;
			tempQT.w = cosf(angle / 2.f);

			// ���� ȸ�� ���� ������ ���� ���� ���Ѵ�
			if (m_TempAnimation->m_rotation.size() == 0)
			{
				_NowRot->m_rotQT_accumulation = tempQT;
			}
			else
			{
				// todo: ������ �� ����� ���� �������� �˾ƺ� ��!
				prevQT = m_TempAnimation->m_rotation.back()->m_rotQT_accumulation;
				_NowRot->m_rotQT_accumulation = prevQT * tempQT;
			}
			m_TempAnimation->m_rotation.push_back(_NowRot);
		}
		break;

		case TOKEND_END:
			// �Ƹ��� �̰� ������ ���� ��Ÿ�������ΰ� ������. while�� Ż���ؾ� �ϴµ�?

			//AfxMessageBox("������ ���� �� �� �����ϴ�!", MB_OK, NULL);
			TRACE("TRACE: �Ľ���: ������ ���� �ý��ϴ�!\n");
			return;

			break;

			/// ���� �ƹ��͵� �ش����� ������
		default:
			// �ƹ��͵� ���� �ʴ´�.
			break;

		}	// switch()


		///-----------------------------------------------
		/// ���� �ڵ�.
		i++;
		if (i > 1000000)
		{
			// ������ 1000000���̻��̳� �� ������ ����. (�����Ͱ� 100000���� �ƴ��̻�)
			// ���� 1000000�̻� ���Ҵٸ� Ȯ���� ���� ������ �ִ� ���̹Ƿ�
			TRACE("������ �鸸�� ���ҽ��ϴ�!");
			return;
		}
		/// ���� �ڵ�.
		///-----------------------------------------------

	}		// while()

	// ������� �Դٸ� while()�� ����ٴ� ���̰� �� ����
	// ��ȣ�� �ݾҴٴ� ���̹Ƿ�
	// �������� (����Լ�)

	return;
}


///----------------------------------------------------------------------
/// parsing�� ���� ������ �Լ���
///----------------------------------------------------------------------

// long�� �о �������ش�.
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
	/// ��m_TokenString ( char[255] ) �̱� ������ CString�� ������ ���� ���Ŷ� �����ߴµ�, �������� CString�� �� ������� �� ����. �˾Ƽ� �޾Ƶ��̴µ�?
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


// 3���� Float�� ���� �ϳ���
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

	return			tempVector3;		// ����ƽ ������ ���۷������ٴ� �� ������ ����.
}

///--------------------------------------------------
/// ���ο��� ������ ����, ����Ʈ�� �ִ´�
///--------------------------------------------------
// �޽ø� �ϳ� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
void CASEParser::Create_onemesh_to_list()
{
	Mesh* temp = new Mesh;
	m_OneMesh = temp;
	m_OneMesh->m_scenedata = m_scenedata;		// Ŭ������ �� ����
	m_MeshList.emplace_back(m_OneMesh);
}

// ��Ʈ���� �ϳ��� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
void CASEParser::Create_materialdata_to_list()
{
	ASEMaterial* temp = new ASEMaterial;
	m_materialdata = temp;
	m_MaterialList.emplace_back(m_materialdata);
}

// �ִϸ��̼ǵ����� �ϳ��� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
void CASEParser::Create_animationdata_to_list()
{
	Animation* temp = new Animation;
	m_TempAnimation = temp;
	//m_AnimationList.emplace_back(m_TempAnimation);
	m_OneMesh->m_TMAnimation = m_TempAnimation;
}

// ���� �ϳ���..
void CASEParser::Create_onevertex_to_list()
{
	Vertex* temp = new Vertex;
	m_OneMesh->m_meshvertex.emplace_back(temp);
}

















