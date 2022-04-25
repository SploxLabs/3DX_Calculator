#pragma once
#include "pch.h"

using namespace DirectX;

class GraphicsCamera {
public:
	GraphicsCamera();
	GraphicsCamera(XMFLOAT3 t_pos, XMFLOAT3 t_look_dir, XMFLOAT3 t_up_dir);

	//lenses
	void SetProjAsPerspective(float t_fov_vertical, float t_aspect_ratio, float t_near_plane, float t_far_plane);
	void SetProjAsOrthographic(float t_view_width, float t_view_height, float t_near_plane, float t_far_plane);

	//positioning and orientation
	void SetPos(XMFLOAT3 t_pos);
	void SetLookDir(XMFLOAT3 t_look_dir);
	void SetUp(XMFLOAT3 t_up);
	void LookAtTarget(XMFLOAT3 t_target);
	
	//main functionality
	XMFLOAT4X4 GetViewProj();

	bool m_using_orthograpic = false;
protected:
	//in order to only update view matrix when something has changed.
	bool m_view_dirty = true;

	XMFLOAT4X4 GetIdentity4x4() {
		return XMFLOAT4X4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);
	}

	XMFLOAT4X4 m_view = GetIdentity4x4();
	XMFLOAT4X4 m_proj = GetIdentity4x4();

	XMFLOAT3 m_pos			= { 0.f, 0.f, -2.f };
	XMFLOAT3 m_look_dir		= { 0.f, 0.f, -1.f };
	XMFLOAT3 m_up_dir		= { 0.f, 1.f,  0.f };

	float m_near = 0.01f;
	float m_far = 1000.f;
};

