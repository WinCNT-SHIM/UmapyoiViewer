#pragma once

///--------------------------------------------------
/// Animation Data�� �����ϱ� ���� Ŭ����
///--------------------------------------------------

class CAnimation_pos 
{
public:
	int		m_time;
	Vector3	m_pos;
};

class CAnimation_rot 
{
public:
	int		m_time;
	Vector3	m_rot;
	float	m_angle;
	// ��뺯���̱� ������ ���� ����� �����ؼ� ������ �ִ´�
	// (�� ���ݱ����� ���뷮���� ��ȯ�Ѵ�)
	Quaternion	m_rotQT_accumulation;
};

class CAnimation_scl {

public:
	int			m_time;
	Vector3		m_scale;
	Vector3		m_scaleaxis;
	float		m_scaleaxisang;
	Quaternion	m_scalerotQT_accumulation;
};


///--------------------------------------------------
/// �� Ŭ������ ������ Animation Ŭ����
///--------------------------------------------------
class Animation
{
public:
	~Animation()
	{
		for (auto& item : m_position)
			delete item;
		for (auto& item : m_rotation)
			delete item;
		for (auto& item : m_scale)
			delete item;

		m_position.clear();
		m_rotation.clear();
		m_scale.clear();
	}


public:
	// �ִϸ��̼� �������� �̸�. ������ ���������̴�
	std::string					m_nodename;
	vector<CAnimation_pos*>		m_position;
	vector<CAnimation_rot*>		m_rotation;
	vector<CAnimation_scl*>		m_scale;

	// �� �������� ƽ ũ��. �̰��� AnimationTable�� ������ ������ ������Ʈ���� �ٸ� �� �ִ�.
	int		m_ticksperFrame;
	// Ű �����Ӱ��� �ּ�ũ��. �̰��� ���ǰͰ� �� ��ġ������ �ʴ´�.
	int		m_minKeyframe;
};
