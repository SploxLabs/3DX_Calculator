#include "pch.h"
#include "GraphicsCamera.h"

GraphicsCamera::GraphicsCamera() {
}

GraphicsCamera::GraphicsCamera(XMFLOAT3 t_pos, XMFLOAT3 t_look_dir, XMFLOAT3 t_up_dir) :
	m_pos(t_pos),
	m_look_dir(t_look_dir),
	m_up_dir(t_up_dir)
{
	SetProjAsPerspective(3.14f * 0.25f, 16.f / 9.f, m_near, m_far);
}

void GraphicsCamera::SetProjAsPerspective(float t_fov_vertical, float t_aspect_ratio, float t_near_plane, float t_far_plane) {
	m_near = t_near_plane;
	m_far = t_far_plane;
	XMMATRIX proj = XMMatrixPerspectiveFovLH(t_fov_vertical, t_aspect_ratio, t_near_plane, t_far_plane);
	XMStoreFloat4x4(&m_proj, proj);
}

void GraphicsCamera::SetProjAsOrthographic(float t_view_width, float t_view_height, float t_near_plane, float t_far_plane) {
	m_near = t_near_plane;
	m_far = t_far_plane;
	XMMATRIX proj = XMMatrixOrthographicLH(t_view_width, t_view_height, t_near_plane, t_far_plane);
	XMStoreFloat4x4(&m_proj, proj);
}

void GraphicsCamera::LookAtTarget(XMFLOAT3 t_target) {
	XMVECTOR pos = XMLoadFloat3(&m_pos);
	XMVECTOR target = XMLoadFloat3(&t_target);
	XMVECTOR pos_to_target = XMVectorSubtract(target, pos);
	XMStoreFloat3(&m_look_dir, XMVector3Normalize(pos_to_target));
	m_view_dirty = true;
}

void GraphicsCamera::SetPos(XMFLOAT3 t_pos) {
	if (m_pos.x != t_pos.x || m_pos.y != t_pos.y || m_pos.z != t_pos.z) {
		m_pos = t_pos; 
		m_view_dirty = true;
	}
}

void GraphicsCamera::SetLookDir(XMFLOAT3 t_look_dir) {
	if (m_look_dir.x != t_look_dir.x || m_look_dir.y != t_look_dir.y || m_look_dir.z != t_look_dir.z) {
		m_look_dir = t_look_dir;
		m_view_dirty = true;
	}
}

void GraphicsCamera::SetUp(XMFLOAT3 t_up_dir) {
	if (m_up_dir.x != t_up_dir.x || m_up_dir.y != t_up_dir.y || m_up_dir.z != t_up_dir.z) {
		m_up_dir = t_up_dir;
		m_view_dirty = true;
	}
}

XMFLOAT4X4 GraphicsCamera::GetViewProj() {
	XMFLOAT4X4 out;

	XMMATRIX view;
	if (m_view_dirty) {
		view = XMMatrixLookToRH(XMLoadFloat3(&m_pos), XMLoadFloat3(&m_look_dir), XMLoadFloat3(&m_up_dir));
		XMStoreFloat4x4(&m_view, view);
	}
	else {
		view = XMLoadFloat4x4(&m_view);
	}
	
	XMMATRIX proj = XMLoadFloat4x4(&m_proj);
	XMMATRIX view_proj = view * proj;
	XMStoreFloat4x4(&out, view_proj);
	return out;
}
