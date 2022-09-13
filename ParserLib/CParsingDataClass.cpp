#include "CParsingDataClass.h"

namespace ASEParser
{
	Vertex::Vertex()
		: m_pos(), m_normal(), m_isnormalset(false)
		, u(0.0f), v(0.0f), m_IsTextureSet(false)
		, m_bw1({0, 0.0f}), m_bw2({ 0, 0.0f }), m_bw3({ 0, 0.0f }), m_bw4({ 0, 0.0f }), m_indices(0), m_indicesPointer(nullptr)
	{

	}

	Vertex::Vertex(const Vertex& oth)
	{
		m_pos = oth.m_pos;

		m_isnormalset = m_isnormalset;
		m_normal = m_normal;

		m_IsTextureSet = oth.m_IsTextureSet;
		u = oth.u;
		v = oth.v;

		m_bw1 = oth.m_bw1;
		m_bw2 = oth.m_bw2;
		m_bw3 = oth.m_bw3;
		m_bw4 = oth.m_bw4;

		m_indices = oth.m_indices;
		m_indicesPointer = oth.m_indicesPointer;	// 얕은 복사라도 상관 없다

		m_tangent = oth.m_tangent;
	}

	Vertex& Vertex::operator=(const Vertex& oth)
	{
		m_pos = oth.m_pos;

		m_isnormalset = m_isnormalset;
		m_normal = m_normal;

		m_IsTextureSet = oth.m_IsTextureSet;
		u = oth.u;
		v = oth.v;

		m_bw1 = oth.m_bw1;
		m_bw2 = oth.m_bw2;
		m_bw3 = oth.m_bw3;
		m_bw4 = oth.m_bw4;

		m_indices = oth.m_indices;
		m_indicesPointer = oth.m_indicesPointer;	// 얕은 복사라도 상관 없다

		m_tangent = oth.m_tangent;

		return *this;
	}

	Vertex::Vertex(Vertex&& oth) noexcept
	{
		m_pos = oth.m_pos;

		m_isnormalset = m_isnormalset;
		m_normal = m_normal;

		m_IsTextureSet = oth.m_IsTextureSet;
		u = oth.u;
		v = oth.v;

		m_bw1 = oth.m_bw1;
		m_bw2 = oth.m_bw2;
		m_bw3 = oth.m_bw3;
		m_bw4 = oth.m_bw4;

		m_indices = oth.m_indices;
		m_indicesPointer = oth.m_indicesPointer;	// 얕은 복사라도 상관 없다
		oth.m_indicesPointer = nullptr;

		m_tangent = oth.m_tangent;
	}

	Vertex& Vertex::operator=(Vertex&& oth)
	{
		m_pos = oth.m_pos;

		m_isnormalset = m_isnormalset;
		m_normal = m_normal;

		m_IsTextureSet = oth.m_IsTextureSet;
		u = oth.u;
		v = oth.v;

		m_bw1 = oth.m_bw1;
		m_bw2 = oth.m_bw2;
		m_bw3 = oth.m_bw3;
		m_bw4 = oth.m_bw4;

		m_indices = oth.m_indices;
		m_indicesPointer = oth.m_indicesPointer;	// 얕은 복사라도 상관 없다
		oth.m_indicesPointer = nullptr;

		m_tangent = oth.m_tangent;

		return *this;
	}

	Face::Face()
	{
		m_vertexindex[0] = 0;
		m_vertexindex[1] = 0;
		m_vertexindex[2] = 0;

		m_normal = Vector3(0, 0, 0);
	}

	COneTVertex::COneTVertex()
	{
		m_u = 0;
		m_v = 0;
		m_w = 0;
	}

	ShapeLine::ShapeLine()
	{

		m_line_number = 0;		// 몇번째 라인인가?
		m_shape_closed = false;	// 닫힌 Shape인가?
		m_shape_vertexcount = 0;		// 몇 개의 Vertex가 있는가?
		//m_shapevertex;					// 버텍스들의 vector
	}

	Scenedata::Scenedata()
	{
		//m_filename;		CString 초기화 필요없음?
		m_firstframe = 0;
		m_lastframe = 0;
		m_framespeed = 0;
		m_ticksperframe = 0;
		m_meshframestep = 0;
		m_keyframestep = 0;
		m_scene_background_static = Vector3(0, 0, 0);
		m_scene_ambient_static = Vector3(0, 0, 0);
	}

	Scenedata::~Scenedata()
	{

	}

	MaterialMap::MaterialMap()
	{
		// 	CString		m_map_name;
		// 	CString		m_map_class;
		m_subno = 0;
		m_map_amount = 0;
		//	CString		m_bitmap;
		m_map_type = "";
		m_uvw_u_offset = 0;
		m_uvw_v_offset = 0;
		m_u_tiling = 0;
		m_v_tiling = 0;
		m_uvw_angle = 0;
		m_uvw_blur = 0;
		m_uvw_blur_offset = 0;
		m_uvw_noise_amt = 0;
		m_uvw_noise_size = 0;
		m_uvw_noise_level = 0;
		m_uvw_noise_phase = 0;
		m_bitmap_filter = "";
	}

	MaterialMap::~MaterialMap()
	{
	}

	ASEMaterial::ASEMaterial()
	{
		m_MaterialNumber = 0;	// Material의 번호. 이것을 기준으로 오브젝트에서 참조한다.
		//CString					m_material_name;
		//CString					m_material_class;
		m_MaterialAmbient = Vector3(0, 0, 0);
		m_MaterialDiffuse = Vector3(0, 0, 0);
		m_MaterialSpecular = Vector3(0, 0, 0);
		m_MaterialShine = 0.0f;
		m_MaterialShineStrength = 0.0f;
		m_MaterialTransparency = 0.0f;
		m_MaterialWiresize = 0;
		m_MaterialShading = "";
		m_MaterialXPFalloff = 0.0f;
		m_MaterialSelfIllum = 0.0f;
		m_MaterialFalloff = 0.0f;
		m_MaterialXPType = "";

		m_numsubmtls = 0;

		m_istextureexist = false;
	}

	ASEMaterial::~ASEMaterial()
	{
		//delete m_map_temp;
		delete m_map_diffuse;
		delete m_map_generic;
	}

	Light_AnimationData::Light_AnimationData()
	{
		// Light Setting
		m_timevalue = -1;// 이건 애니메이션이 없는경우의 셋팅이다. 그런데 시간이 -1인 경우가 있으면 어쩌지?
		m_light_color = Vector3(1.0f, 1.0f, 1.0f);
		m_light_intens = 0.0f;
		m_light_aspect = 0.0f;
		m_light_hotspot = 0.0f;		///
		m_light_falloff = 0.0f;		/// 이 두가지는 SpotLight인 경우
		m_light_tdist = 0.0f;
		m_light_mapbias = 0.0f;
		m_light_maprange = 0.0f;
		m_light_mapsize = 0;
		m_light_raybias = 0.0f;
	}

	Mesh::Mesh()
	{
		m_type = 0;
		m_isAnimated = false;
		//CString				m_nodename;				// CString은 자동으로 초기화 시켜주는가?
		//CString				m_nodeparent;
		m_isparentexist = false;					// 기본적으로 부모 노드는 존재치 않는다.

		///----------------------------------
		/// *NODE_TM (Transform Matrix)
		///----------------------------------
		//CString				m_meshname;
		m_inherit_pos = Vector3(0, 0, 0);
		m_inherit_rot = Vector3(0, 0, 0);
		m_inherit_scl = Vector3(0, 0, 0);
		m_TMRow0 = Vector3(0, 0, 0);
		m_TMRow1 = Vector3(0, 0, 0);
		m_TMRow2 = Vector3(0, 0, 0);
		m_TMRow3 = Vector3(0, 0, 0);
		m_tm_pos = Vector3(0, 0, 0);
		m_tm_rotaxis = Vector3(0, 0, 0);
		m_tm_rotangle = 0;
		m_tm_scale = Vector3(0, 0, 0);
		m_tm_scaleaxis = Vector3(0, 0, 0);
		m_tm_scaleaxisang = 0;


		///----------------------------------
		/// *MESH
		///----------------------------------
		m_timevalue = 0;
		m_numbone = 0;
		m_numskinweight = 0;
		m_mesh_numvertex = 0;
		m_mesh_numfaces = 0;

		//Vector3*		m_meshvertex;			// 이건 버텍스
		//Vector3*		m_meshface;				// 이게 페이스(버텍스 인덱스, 노말)

		m_mesh_numtvertex = 0;

		//m_mesh_tverlist		// 텍스쳐의 버텍스 리스트
		m_mesh_tvfaces = 0;
		//m_mesh_tfacelist	=	// 텍스쳐의 페이스 리스트
		m_mesh_numcvertex = 0;

		m_mesh_facenormal = nullptr;
		m_mesh_vertexnormal = nullptr;
		
		m_opt_index = nullptr;


		///----------------------------------
		/// Shape Object
		///----------------------------------
		m_shape_linecount = 0;


		///----------------------------------
		/// TM_ANIMATION
		///----------------------------------
		m_TMAnimation = nullptr;


		/// 나머지 정보들
		m_prop_motionblur = 0;
		m_prop_castshadow = 0;
		m_prop_recvshadow = 0;
		m_MaterialRef = 0;
		m_wireframe_color = Vector3(0, 0, 0);


		///----------------------------------
		/// Camera
		///----------------------------------
		//CString				m_camera_type;
		// CAMERA_SETTINGS
		m_camera_near = 0.0f;
		m_camera_far = 0.0f;
		m_camera_fov = 0.0f;
		m_camera_tdist = 0.0f;
		m_camera_isloadTarget = false;


		///----------------------------------
		/// Light
		///----------------------------------
		// Light Property
		m_light_uselight = false;
		m_light_useglobal = false;
		m_light_absmapbias = false;
		m_light_overshoot = false;


		///----------------------------------
		/// Light Setting & Animation
		///----------------------------------
		m_lightanimation = nullptr;
		m_light_isloadTarget = false;
		m_light_isAnimate = false;


		///----------------------------------
		/// Skinning Object
		///----------------------------------
		m_IsSkinningObject = false;		// 스키닝 오브젝트인가?
		m_bone = nullptr;
		m_wvertex = nullptr;
		//vector<CBone*>			m_BoneList;
		//vector<CWeight_Vertex*>	m_vector_wvertexs;

	}

	Mesh::~Mesh()
	{
		// 소멸자에서 생성되었던 메시데이터 (버텍스리스트, 페이스 리스트) 삭제해야함.
		// m_meshvertex는 이동 생성자로 m_opt_vertex에 넘기는 걸로 변경해서 이제 안 해도 됨
		//for (auto& item : m_meshvertex)	
		//	delete item;
		for (auto& item : m_meshface)
			delete item;
		for (auto& item : m_mesh_tvertex)
			delete item;
		for (auto& item : m_opt_vertex)
			delete item;
		for (auto& item : m_vector_shape_line)
			delete item;
		//for (auto& item : m_AnimationList)
		//	delete item;
		for (auto& item : m_list_lightanimation)
			delete item;
		for (auto& item : m_BoneList)
			delete item;
		for (auto& item : m_WVertexs)
			delete item;

		delete m_opt_index;
		delete m_TMAnimation;

		m_meshvertex.clear();
		m_meshface.clear();
		m_mesh_tvertex.clear();
		m_opt_vertex.clear();
		m_vector_shape_line.clear();
		//m_AnimationList.clear();
		m_list_lightanimation.clear();
		m_BoneList.clear();
		m_WVertexs.clear();
	}

	VertexWeight::~VertexWeight()
	{
		for (auto& item : m_bone_blending_weight)
			delete item;
	}

}