// ��� ���۴� 16����Ʈ ������ �ǵ��� pad�� ���� ��
struct CameraInfo
{
	float3 gEyePosW;
	float pad;
	float4x4 gViewMatrix;
	float4x4 gProjMatrix;
	float4x4 gPrevViewMatrix;
	float4x4 gPrevProjMatrix;
};

struct SSAOInfo
{
	float4 _offsetVectors[14];
	float4 _viewFrustumCorners[4];
	float _occlusionRadius;
	float _occlusionFadeStart;
	float _occlusionFadeEnd;
	float _surfaceEpsilon;
	uint _sampleCount;
	float3 _pad;
};