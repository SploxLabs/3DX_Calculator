#pragma once
#include "GraphicsCamera.h"

class OrbitCamera : public GraphicsCamera {
public:
	OrbitCamera();

	void OnMouseDown(WPARAM btnState); //externalize this?
	void OnMouseUp(WPARAM btnState); //externalize this?
	void OnMouseMove(WPARAM btnState); //externalize this?
	
	XMFLOAT3 m_orbit_point;

	float m_width = 1.f;
	float m_height = 1.f;
	float m_aspect_ratio = 16.f/9.f;

	float m_theta	= 0.f;
	float m_phi		= 0.f;
	float m_radius	= 2.0f;

	float m_horizontal_sensitivity	= 1.0f;
	float m_vertical_sensitivity	= 1.0f;
	float m_zooming_sensitivity		= 1.0f;

	float m_theta_min_limit = -std::numeric_limits<float>::max();
	float m_theta_max_limit =  std::numeric_limits<float>::max();

	float m_phi_min_limit = 0.001f;
	float m_phi_max_limit = 3.14f - 0.001f;
	
	float m_radius_min_limit = 0.1f;
	float m_radius_max_limit = 10.f;

	int m_anchor_point_x; //externalize this?
	int m_anchor_point_y; //externalize this?


	void SetTheta(float t_theta);
	void SetPhi(float t_phi);
	void SetRadius(float t_radius);

	XMFLOAT4X4 GetViewProj();

	XMVECTOR GetPos();
	XMVECTOR GetLookDir();
private:
	
	float m_horizontal_radians_per_pixel = 0.5f * 3.14f / 180.f ;
	float m_vertical_radians_per_pixel = 0.5f * 3.14f / 180.f;
};

