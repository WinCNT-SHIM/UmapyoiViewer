/// <summary>
/// CParsingDataClass.h
/// 
/// Ver 1.0
/// ASE������ �Ľ������� ������ �����͵��� ����ü�� �������.
/// 
/// �ϴ� �� �� �о���̴� ���̰�, �������� ���������� ������ �Ѵ�.
/// D3DX���ξ, XM���ξ �������� ���ڴ�.
/// 
/// (���� �ּ�)
/// �˴� struct�� public���� �ߴµ� ������ �ƹ�����
/// ���� ���⿡�� ���� �����ؾ��ϱ� ����.
/// 
/// ��� �� �ڷᱸ���� �Ѱܹ޴� D3D�� ������������
/// ĸ��ȭ�� �Ǿ��ִ�. ��� ��ȯ �Լ����� �������� �� ���� �ִ�.
/// (�׷��� �ʴٸ� ������ GetSet�޼ҵ���� ������ �Ѵ�)
/// (���� �ּ�)
/// 
/// 2021.04.11 LeHideOffice
/// </summary>

#pragma once


#include <windows.h>
#include <vector>
#include <string>
//#include <DirectXMath.h>
//#include <SimpleMath.h>
#include "../Inc/SimpleMath.h"

using namespace std;
using namespace DirectX;
using namespace SimpleMath;

class Animation;

namespace ASEParser
{
	struct IndexList
	{
		int index[3];
	};

	///--------------------------------------------------
	/// �� ���� Vertex
	///
	/// 3DSMAX�� ASE����� �״�� �������� ���� ���̴�.
	///--------------------------------------------------
	struct Vertex
	{
		Vertex();
		Vertex(const Vertex& oth);
		Vertex& operator=(const Vertex& oth);
		Vertex(Vertex&& oth) noexcept;
		Vertex& operator=(Vertex&& oth);

		// ��ǥ���� ��ġ��
		Vector3 m_pos;

		// �븻��
		Vector3 m_normal;

		// �븻���� ���õƴ°�?
		bool m_isnormalset;

		// �ؽ��� ��ǥ
		float u, v;

		// �ؽ��İ��� ���õƴ°�?
		bool m_IsTextureSet;

		/// Skinned Mesh ���� �߰�
		// weight 3��
		//float m_bw1, m_bw2, m_bw3;
		// ==================== Skinnig �߰� �ڵ� ====================
		// ����ġ�� �ش� ���� ������ �����ϱ� ���� Ÿ���� ����
		pair<int, float> m_bw1, m_bw2, m_bw3, m_bw4;

		// �ε���
		int m_indices;

		int* m_indicesPointer;

		// ==================== Normal Mapping �߰� �ڵ� ====================
		Vector3 m_tangent;
	};


	///--------------------------------------------------
	/// �� ���� Texture - Vertex
	///
	/// �̰��� Raw�������̸� ����ȭ�� �̷����� �ᱹ��
	/// COneVertex�� ���� �ȴ�.
	///--------------------------------------------------
	struct COneTVertex
	{
		COneTVertex();

		float m_u, m_v, m_w;
	};


	///--------------------------------------------------
	/// �� ���� Face
	///
	/// 3DSMAX�� ASE����� �״�� �������� ���� ���̴�.
	///
	/// �Ľ� �ʱ⿡ Face�� ������ ������ ������
	/// �ߺ��Ǵ� VertexNormal��, �ؽ��� �ڵ����Ʈ ����
	/// ���� �� �ִ�. �װ��� ������� �Ѵ�.
	///--------------------------------------------------
	struct Face
	{
		// �� Face�� �̷�� ���ؽ��� �ε���
		int				m_vertexindex[3];
		// Face�� Normal��
		Vector3			m_normal;
		// Vertex�� Normal��
		Vector3			m_normalvertex[3];
		// Texture Coordinate
		int				m_TFace[3];

		// �ʱ�ȭ�� ���� ������.
		Face();
	};


	///--------------------------------------------------
	/// �� ���� Shape Vertex�� ������ �����ϱ� ���� ����ü
	///
	/// 3DSMAX�� ASE����� �״�� �������� ���� ���̴�.
	///--------------------------------------------------
	struct ShapeVertex
	{
		int				m_indexnum;			// �ε��� �ѹ�. �׳� �Ϸù�ȣ��
		bool				m_isknot;			// KNOT�ΰ� INTERP�ΰ�?
		Vector3			m_pos;				// ������ ������
	};


	///--------------------------------------------------
	/// �� ���� Shape Object�� ������ �����ϱ� ���� ����ü
	///
	/// 3DSMAX�� ASE����� �״�� �������� ���� ���̴�.
	///--------------------------------------------------
	struct ShapeLine
	{
		int	m_line_number;		// ���° �����ΰ�?
		bool m_shape_closed;		// ���� Shape�ΰ�?
		int	m_shape_vertexcount;// �� ���� Vertex�� �ִ°�?
		vector<ShapeVertex*> m_shapevertex;	// ���ؽ����� vector

		// �ʱ�ȭ�� ���� ������
		ShapeLine();
	};


	/// <summary>
	/// �� ���� Scene�� �����ϱ� ���� ����ü
	/// ASE���Ͽ��� ���, �ڸ�Ʈ ���� ������ SCENE�κ��� �б� ���� ����ü.
	/// </summary>
	struct Scenedata
	{
	public:
		std::string		m_filename;					// ���� �̸�
		int				m_firstframe;
		int				m_lastframe;
		int				m_framespeed;
		int				m_ticksperframe;
		int				m_meshframestep;
		int				m_keyframestep;
		Vector3			m_scene_background_static;
		Vector3			m_scene_ambient_static;

	public:
		Scenedata();
		~Scenedata();
	};


	///--------------------------------------------------
	/// �� ���� Map�� �����ϱ� ���� Ŭ����
	///--------------------------------------------------
	struct MaterialMap
	{
		std::string		m_map_name;
		std::string		m_map_class;
		int		m_subno;
		float		m_map_amount;
		std::string		m_bitmap;
		string		m_map_type;
		float		m_uvw_u_offset;
		float		m_uvw_v_offset;
		float		m_u_tiling;
		float		m_v_tiling;
		float		m_uvw_angle;
		float		m_uvw_blur;
		float		m_uvw_blur_offset;
		float		m_uvw_noise_amt;
		float		m_uvw_noise_size;
		float		m_uvw_noise_level;
		float		m_uvw_noise_phase;
		string		m_bitmap_filter;

		//
		MaterialMap();
		~MaterialMap();
	};


	///--------------------------------------------------
	/// �� ���� Material�� �����ϱ� ���� Ŭ����
	///--------------------------------------------------
	struct ASEMaterial
	{
		//--------------------
		//	��� ������
		//--------------------
	public:
		// ������ *MATERIAL_COUNT 0 �ϳ��ۿ� ������
		int					m_MaterialNumber;			// Material�� ��ȣ. �̰��� �������� ������Ʈ���� �����Ѵ�.
		std::string			m_material_name;
		std::string			m_material_class;
		Vector3				m_MaterialAmbient;
		Vector3				m_MaterialDiffuse;
		Vector3				m_MaterialSpecular;
		float				m_MaterialShine;
		float				m_MaterialShineStrength;
		float				m_MaterialTransparency;
		float				m_MaterialWiresize;
		string				m_MaterialShading;
		float				m_MaterialXPFalloff;
		float				m_MaterialSelfIllum;
		string				m_MaterialFalloff;
		string				m_MaterialXPType;

		int					m_numsubmtls;
		ASEMaterial*		m_submaterial;				// �����Ʈ������ �����͸� ������ ����

		MaterialMap*		m_map_temp;					// �ӽ÷� ���̴� ����
		MaterialMap*		m_map_diffuse;				// �ϴ� ������ map_diffuse�ۿ� ����
		MaterialMap*		m_map_generic;
		//CMaterial_Map*	m_map_diffuse;
		//CMaterial_Map*	m_map_diffuse;

		bool				m_istextureexist;			// �ؽ��İ� �����ϴ°�?

		//--------------------
		//	Method
		//--------------------
	public:
		// ������ �Ҹ���
		ASEMaterial();
		~ASEMaterial();
	};


	// Light�� Animation ( Setting�� ��ȭ�� Animation )
	struct Light_AnimationData
	{
		// Light Setting
		float		m_timevalue;		// ���� mesh�� �ִµ�.. �̰� ��� ���� �̷����̸�.
		Vector3		m_light_color;
		float		m_light_intens;
		float		m_light_aspect;
		float		m_light_hotspot;	///
		float		m_light_falloff;	/// �� �ΰ����� SpotLight�� ���
		float		m_light_tdist;
		float		m_light_mapbias;
		float		m_light_maprange;
		int			m_light_mapsize;
		float		m_light_raybias;

	public:
		Light_AnimationData();

	};

	// Bone �ϳ��� ����ü
	struct Bone
	{
		int				m_bone_number;		// ������ ���ͷ� ���������� ���߿� ������ �ʿ�, �˻��� �ʿ�?
		std::string		m_BoneName;
		// ��¼�� ���⿡�� ��Ʈ������ �־�� �ϴ����� ����. ������ ��ư ���ܵ���

		// �̳༮�� ó���� NodeTM�̴� - offset�� ���ϱ� ���ؼ� �����Ѵ�.
		Matrix*			m_boneTM_NodeTM;
		// �̳༮�� �ִϸ��̼��� �����, ������ WorldTM�̴�
		Matrix*			m_boneTM_WorldTM;
	};


	// bone�ϳ��� ����� ����ġ �� ��
	struct Weight
	{
		int		m_bone_number;		// weight�� ����� Bone�� IndexNumber;
		float		m_bone_weight;		// ����ġ
	};


	// Vertex �ϳ��� �־��� ����ġ���� �����ϴ� ����ü (����ü �ϳ��� ���ؽ� �ϳ��� �ϴ��� �����̴�)
	struct VertexWeight
	{
		~VertexWeight();

		int		m_wvertex_number;	// ���ؽ��� ��ȣ. ���� ���ͷ� �����ϹǷ� �� �� ������...

		// ���������� �Ʒ� ����Ʈ�� �ӽ� ����
		Weight* m_temp_bone_blend_weight;
		// ���ؽ� �ϳ��� ���� ���� Bone-Weight�� ���� �� �ִ�.
		vector<Weight*>	m_bone_blending_weight;	// bone�ϳ��� ����� ����ġ
	};



	///--------------------------------------------------
	/// �� ���� �޽ø� �Ľ� - �����ϱ� ���� Ŭ����
	/// ���� ���� �޽ð� ���� �� �ֱ� ������ ���� �� �κи� ����ü�� �������.
	///--------------------------------------------------
	struct Mesh
	{
	public:
		///----------------------------------
		/// Ÿ��, �ִϸ��̼��� �ִ°�? ���
		///----------------------------------
		int	m_type;					// Ÿ��
		bool m_isAnimated;			// �ִϸ��̼��� ������ �ִ°�? (������Ʈ�� ���� �ٸ��� �ؼ��� �� ����)
		Scenedata m_scenedata;		// (3DS Max��) �� ������

		///----------------------------------
		/// *NODE_NAME
		///----------------------------------
		std::string	m_Nodename;		// � �������� ������ �Ʒ��� m_meshname�� ���� �̸��� �ѹ� �� ����. �Ƹ��� �޽��� �̸� ����Ʈ�����... �� �׷��� �ƴұ� ��ư ������ ���� 1�̹Ƿ� �������� ������ ����.
		std::string	m_NodeParent;	// �θ� ����� �̸�. �̰��� �ִٸ� �θ� �ִ� ���̴�.
		bool m_isparentexist;		// �׷��� Ȯ���� bool������ ������ ����.

		///----------------------------------
		/// *NODE_TM (Transform Matrix)
		///----------------------------------
		Vector3	m_inherit_pos;
		Vector3	m_inherit_rot;
		Vector3	m_inherit_scl;
		Vector3	m_TMRow0;
		Vector3	m_TMRow1;
		Vector3	m_TMRow2;
		Vector3	m_TMRow3;
		Vector3	m_tm_pos;
		Vector3	m_tm_rotaxis;
		float m_tm_rotangle;
		Vector3	m_tm_scale;
		Vector3	m_tm_scaleaxis;
		float m_tm_scaleaxisang;

		///----------------------------------
		/// *MESH
		///----------------------------------
		float m_timevalue;			// ���� ������ ���������� ���δ�. ��ư �ð���.
		int	m_numbone;				/// ���� ����. �̰��� �ִٸ� ��Ű�� ������Ʈ�� �����Ѵ�.
		int	m_numskinweight;		/// ��Ų ����ġ. ���Ҹ���?
		int	m_mesh_numvertex;		// (���ؽ��� ��ǥ������)
		int	m_mesh_numfaces;		// (���ؽ��� �ε���)

		// �޽� ���� vertex �迭�� �����ϱ� ���� ������
		/// *MESH_VERTEX_LIST
		vector<Vertex*>	m_meshvertex;	// ���ؽ�
		/// *MESH_FACE_LIST
		vector<Face*> m_meshface;		// �ε���

		// �ؽ��Ŀ� �����͵�
		/// *MESH_NUMTVERTEX
		int	m_mesh_numtvertex;		// �ؽ��Ŀ� ���ؽ� ����
		vector<COneTVertex*> m_mesh_tvertex;		// �ؽ��Ŀ� ���ؽ�
		/// *MESH_RVERTLIST
		int	m_mesh_tvfaces;			// �ؽ��Ŀ� ���̽� ���� - (������ ���� Face�� ���� �̹� ������ ��������Ƿ� ���븸 �־��ָ� �ȴ�)
		int	m_mesh_numcvertex;		// �� �̰� ��� ���°�?

		// �븻 ������
		Vector3* m_mesh_facenormal;			// �� ���̽� �븻��
		Vector3* m_mesh_vertexnormal;		// ���ؽ� �븻�� 3����

		vector<Vertex*> m_opt_vertex;	/// ����ȭ ���� ���ؽ���
		IndexList* m_opt_index;			/// ����ȭ ���� �ε�����

		///----------------------------------
		/// Shape Object
		///----------------------------------
		int				m_shape_linecount;			// ������ ��
		vector<ShapeLine*>	m_vector_shape_line;	// ������ ������ ������

		///----------------------------------
		/// TM_ANIMATION
		///----------------------------------
		Animation* m_TMAnimation;					// �� ������Ʈ�� �ִϸ��̼� ������

		/// ������ ������
		int				m_prop_motionblur;			//*PROP_MOTIONBLUR 0
		int				m_prop_castshadow;			//*PROP_CASTSHADOW 1
		int				m_prop_recvshadow;			//*PROP_RECVSHADOW 1
		int				m_MaterialRef;				//*MATERIAL_REF			// �� �޽��� ��Ʈ������ ��ȣ
		Vector3			m_wireframe_color;			//*WIREFRAME_COLOR 0.89803922	0.65098041	0.84313726


		///----------------------------------
		/// Camera
		///----------------------------------
		// �̷��� ������ �ѹ��� �о������ ���� ���α� �ѵ�..
		// �ϴ� �̷��� �����������ϱ�. �׸��� �ļ��� ��ͱ����� ����� �Ǹ� �̷��� �ϴ°� ���ϴ�
		// ��� D3D�� Ŭ������ ������ �� ��ȯ�� �Ǵϱ�
		std::string			m_camera_type;
		// CAMERA_SETTINGS
		//float				m_timevalue;		// mesh�� �̹� �ִ�.
		float				m_camera_near;
		float				m_camera_far;
		float				m_camera_fov;
		float				m_camera_tdist;
		bool				m_camera_isloadTarget;	// ī�޶�� NodeTM�� �ΰ��� �ִµ�, ��ͱ��������� ������ ����� 
													// (�׻� ������ Target�� �о���� �� �ִ� �׷��� �װ��� ���ذ��� �Ѵ�)

		///----------------------------------
		/// Light
		///----------------------------------
		std::string				m_light_type;
		// Omni�ΰ� SpotLight�ΰ��� ó���� �� ��� �Ѵ�.

		// Light Property
		std::string			m_light_shadows;
		bool				m_light_uselight;
		std::string			m_light_spotshape;
		bool				m_light_useglobal;
		bool				m_light_absmapbias;
		bool				m_light_overshoot;

		///----------------------------------
		/// Light Setting & Animation
		///----------------------------------
		// Light Setting
		Light_AnimationData* m_lightanimation;
		vector<Light_AnimationData*>	m_list_lightanimation;	// �װ��� Animation

		// ���� NodeTM�� �� �� �ְ�, ī�޶�� ���� ������ �� ������ �ִ�.
		// ��(�ƹ����� ���� ���� ������. �ٲ���.)
		bool				m_light_isloadTarget;
		// Animation�� �Ǵ°�?
		bool				m_light_isAnimate;

		///----------------------------------
		/// Skinning Object
		///----------------------------------
		// SKIN_INITTM�� �����ϰ�
		// TM_ROW0~3
		// BONE_LIST
		bool m_IsSkinningObject;		// ��Ű�� ������Ʈ�ΰ�?
		Bone* m_bone;					// Bone �Ѱ��� ������ (�ļ� ���ο��� �ӽ� �������� ����)
		VertexWeight* m_wvertex;		// Weight_Vertex �Ѱ��� ������ (���������� �ӽð���)
		vector<Bone*> m_BoneList;		// �̰� �׳� ��ȣ�� �̸� �����̴�. �� �ȿ� �����͸� �־���Ƶ� �����ڱ���(�����ϱ� ���ϰ� - �׷��� ������ �Ź� �̸����� �˻��� �ؾߵ�����. �ƴϸ� �ε����� ������ ��� �ð���� ������ �Ҽ���?)
		vector<VertexWeight*>	m_WVertexs;		// ���ؽ� �ϳ��� �޶�پ��ִ� ����ġ��

		
		// ==================== Collision Detection �߰� �ڵ� ====================
		// �޽��� �ּ�/�ִ� ���ؽ��� �����ϱ� ���� ����
		XMFLOAT3 m_MinVertexOneMesh = { FLT_MAX, FLT_MAX, FLT_MAX };
		XMFLOAT3 m_MaxVertexOneMesh = { FLT_MIN, FLT_MIN, FLT_MIN };
		// ��ü ������Ʈ �ϳ��� ���� �ִ�/�ּ� ���ؽ�(����� ��)
		XMFLOAT3 m_MinVertexObj = { FLT_MAX, FLT_MAX, FLT_MAX };
		XMFLOAT3 m_MaxVertexObj = { FLT_MIN, FLT_MIN, FLT_MIN };

	public:
		Mesh();
		~Mesh();

	};
}



