//------------------------------------------------------------
//
// ASE Parser
// Ver 2.1.20080124
// 
// ��ü������ ������ ������ ������ 
// �̰��� D3D�� Ư�� ������Ʈ�� ��ȯ�ϴ� ������ Ŀ���Ѵ�
//
//------------------------------------------------------------
#pragma once

enum eObjectType
{
	eObjectType_ObjectRoot, eObjectType_Geomobject, eObjectType_Shape,
	eObjectType_Camera, eObjectType_Light, eObjectType_HelperObject,
	eObjectType_MarkerObject, eObjectType_SkinnedMesh, eObjectType_Bone,
	eObjectType_CMeshPlane, eObjectType_CBullet_Trace,
	//
	eObjectType_BasicSphere, eObjectType_Missile,
	eObjectType_MeshBillboard, eObjectType_PointSprite,			// CMesh���..
	eObjectType_End,
};

// �������� �������ֽ� Lexer
#include "ASEFile.h"

// �ļ� ���ο��� ���� ��������
#include "CParsingDataClass.h"
#include "Animation.h"

#include <list>
#include <memory>

using namespace std;
using namespace ASEParser;


// �ļ� ���ο��� Ư�� ��带 ������ �۵��Ѵ�. (�ļ��� ��ͱ��� ����)
enum eParsingmode
{
	eNone, eGeomobject, eAnimation, eShape, eCamera, eLight, eHelperObject,
	eSkin_InitTM, eBone_List,
	eEnd,
};

/// ASE�� �Ľ��ϱ� ���� Ŭ����. �⺻������ �־����� LexerŬ������ �̿��Ѵ�.

/// <summary>
/// �л� ���� ASE Parser�� ���ÿ����� �ణ �����ߴ�.
///
/// 2021.04.12 LeHideKGIT
/// </summary>
class CASEParser
{
public:
	CASEParser();
	~CASEParser();


	/// �ʱ�ȭ, �ε�
	bool Init();
	bool Load(LPSTR p_File);		// ASE������ �о, ��ü ����Ʈ�� �ִ´�.

	/// 1�� ��ȯ
	// CScenedata ��ȯ
	bool TranslateToD3DFormat_scene(Scenedata* pscene);

	bool Convert_Optimize(Mesh* pMesh);		// ���ؽ�(�븻), �ؽ��� ������ ����ȭ�� �� �ش�.
	bool ConvertAll(Mesh* pMesh);			// ��� �׳� ��ȯ (���ÿ�)


private:
	// ��ū�� �����س��� ���� ��Ʈ��
	char m_TokenString[256];
	int m_Token;
	int m_parsingmode;				// ���� ���. ��� ���� ���� ��������. nodename�̶����.. row123, mapname���..


public:
	/// �⺻���� �о���� �����
	int m_data_asciiexport;			// *3DSMAX_ASCIIEXPORT	200 - �� ó�� ������ ����ε� ���� �ǹ������� �𸣰ڰ�, �ϴ� ���ǻ� �о �������ش�.
	Scenedata m_scenedata;			// ���� Scene�� �����Ͱ� �� ����ü

	/// ��Ʈ���� ����
	int	m_MaterialCount;					// Material�� �� ��ΰ�?
	ASEMaterial* m_materialdata;			// Material�� �����Ͱ� �� ����ü
	vector<ASEMaterial*> m_MaterialList;	// Material���� ����Ʈ

	/// �� ���� N���� �޽ð� ������ �� �ִ�.
	Mesh* m_OneMesh;						// �޽� �ϳ��� �����Ͱ� �� ����ü
	vector<Mesh*> m_MeshList;				// �޽õ�

	/// ������ ������Ʈ�� �Ľ��� �� �ʿ��� �߰� ����
	//( �ļ� ���ο��� ������ ������Ʈ�� GeomObject�� �����ϰ� ��޵ȴ�)
	ShapeLine* m_nowshapeline;				// ���� �а� �ִ� Line
	ShapeVertex* m_nowshapevertex;			// ���� �а� �ִ� ShapeVertex

	// �޽��� �ű���
	///// �ִϸ��̼�
	Animation* m_TempAnimation;				// �� ������Ʈ�� �ִϸ��̼� ������(�Ľ̿�)
	//vector<Animation*> m_AnimationList;		// �� �������� ����Ʈ


	// ==================== Collision Detection �߰� �ڵ� ====================
	// ��ü ������Ʈ �ϳ��� ���� �ִ�/�ּ� ���ؽ�
	XMFLOAT3 m_MinVertexObj = { FLT_MAX, FLT_MAX, FLT_MAX };
	XMFLOAT3 m_MaxVertexObj = { FLT_MIN, FLT_MIN, FLT_MIN };

public:
	//--------------------------------------------------
	// Export
	//--------------------------------------------------
	Mesh* GetMesh(int index);


public:
	//--------------------------------------------------
	//	Lexcer
	//--------------------------------------------------
private:
	ASE::CASELexer* m_lexer;


	//--------------------------------------------------
	// Parsing_DivergeRecursiveALL() ���ο��� ���� ���� �Լ���.
	// (parsing�� ���� ������ �Լ���)
	//--------------------------------------------------
private:
	/// �Ľ��� �ð� �Ǵ� �Ŵ��� ����Լ�. ���⼭ ��� ��ū�� ó���� �Ѵ�.
	void Parsing_DivergeRecursiveALL(int depth);

	int Parsing_NumberLong();			// long�� �о �������ش�.
	float Parsing_NumberFloat();		// float
	LPSTR Parsing_String();				// string
	int Parsing_NumberInt();			// int
	Vector3 Parsing_NumberVector3();	// 3���� Float�� ���� �ϳ��� �о �������ش�.

	void Create_onemesh_to_list();		// �޽ø� �ϳ� �������� �����ϰ�, ����Ʈ�� �ִ´�.
	void Create_materialdata_to_list();	// ��Ʈ���� �ϳ��� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
	void Create_animationdata_to_list();// �ִϸ��̼� ������(�� ���� ������Ʈ ����) �ϳ��� �������� �����ϰ�, ����Ʈ�� �ִ´�.
	void Create_onevertex_to_list();	// ���ؽ� �ϳ��� �������� �����ϰ�, �� �����͸� ����Ʈ�� �ִ´�.
};



























