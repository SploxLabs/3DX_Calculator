#pragma once
#include "pch.h"
using namespace DirectX;

class OrthoOrbitCamera {

public:
	void SetAspectRatio(float t_aspect_ratio);

	void SetWidth(float t_width);
	void SetRadius(float t_radius);
	void SetTheta(float t_theta);
	void SetPhi(float t_phi);

	
	void SetPos(XMFLOAT3 pos);
	void SetPos(float x, float y, float z);

	void OnMouseDown(WPARAM t_wparam);
	void OnMouseUp(WPARAM t_wparam);
	void OnMouseMove(WPARAM t_wparam);
	void OnMouseScroll(WPARAM t_wparam);//to-do

	XMFLOAT4X4	GetViewProj();
	XMVECTOR	GetPos();
	XMVECTOR	GetLookDir();

private:

	XMFLOAT3 orbit_point;

	float aspect_ratio = 1.f;
	
	float width = 2.f;
	float width_min = 0.1f;
	float width_max = FLT_MAX;

	float radius = 10000.f;
	float radius_min = 1.f;
	float radius_max = FLT_MAX;

	float theta = 0.f;
	float theta_min = -FLT_MAX;
	float theta_max = FLT_MAX;

	float phi = 0.f;
	float phi_min = 0.001f; //no back flip
	float phi_max = 3.14f - 0.001f; //no front flips

	float horizontal_sensitivity = 1.0f; //theta
	float vertical_sensitivity = 1.0f; //phi
	float zooming_sensitivity = 1.0f; //width

	int m_anchor_point_x;
	int m_anchor_point_y;

	float m_horizontal_radians_per_pixel = 0.5f * 3.14f / 180.f;
	float m_vertical_radians_per_pixel = 0.5f * 3.14f / 180.f;
};

