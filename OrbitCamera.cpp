#include "pch.h"
#include "OrbitCamera.h"

OrbitCamera::OrbitCamera() : GraphicsCamera()
{
	m_orbit_point = { 0.f, 0.f, 0.f };
}

void OrbitCamera::OnMouseDown(WPARAM t_wparam) {
	POINT pt;
	GetCursorPos(&pt);
	m_anchor_point_x = pt.x;
	m_anchor_point_y = pt.y;
}

void OrbitCamera::OnMouseUp(WPARAM t_wparam) {
}

void OrbitCamera::OnMouseMove(WPARAM t_wparam) {
	POINT pt;
	int dx = 0;
	int dy = 0;

	if ((t_wparam & MK_LBUTTON) != 0 || (t_wparam & MK_RBUTTON) != 0) {
		GetCursorPos(&pt);
		dx = pt.x - m_anchor_point_x;
		dy = pt.y - m_anchor_point_y;
		SetCursorPos(m_anchor_point_x, m_anchor_point_y);
	}

	if (dx != 0 || dy != 0) {
		if ((t_wparam & MK_LBUTTON) != 0) {
			float dtheta = static_cast<float>(dx) * m_horizontal_radians_per_pixel * m_horizontal_sensitivity;
			float dphi = static_cast<float>(dy) * m_vertical_radians_per_pixel * m_vertical_sensitivity;	
			SetTheta(m_theta + dtheta);
			SetPhi(m_phi + dphi);
		}
		else if ((t_wparam & MK_RBUTTON) != 0) {
			float dx_r = 0.005f * static_cast<float>(dx) * m_zooming_sensitivity;
			float dy_r = 0.005f * static_cast<float>(dy) * m_zooming_sensitivity;
			SetRadius(m_radius + (dx_r - dy_r));
		}
	}

}

void OrbitCamera::SetTheta(float t_theta) {
	if (m_theta != t_theta) {
		float old_m_theta = m_theta;
		m_theta = t_theta;

		if (m_theta > m_theta_max_limit) {
			m_theta = m_theta_max_limit;
		}
		else if (m_theta < m_theta_min_limit) {
			m_theta = m_theta_min_limit;
		}

		if (m_theta != old_m_theta) {
			m_view_dirty = true;
		}
	}
}

void OrbitCamera::SetPhi(float t_phi) {
	if (m_phi != t_phi) {
		float old_m_phi = m_phi;
		m_phi = t_phi;

		if (m_phi > m_phi_max_limit) {
			m_phi = m_phi_max_limit;
		}
		else if (m_phi < m_phi_min_limit) {
			m_phi = m_phi_min_limit;
		};

		if (m_phi != old_m_phi) {
			m_view_dirty = true;
		}
	}
}

void OrbitCamera::SetRadius(float t_radius) {
	if (m_radius != t_radius) {
		float old_m_radius = m_radius;
		m_radius = t_radius;

		if (m_radius > m_radius_max_limit) {
			m_radius = m_radius_max_limit;
		}
		else if (m_radius < m_radius_min_limit) {
			m_radius = m_radius_min_limit;
		};

		if (m_radius != old_m_radius) {
			m_view_dirty = true;
			m_width = m_radius;
			m_height = m_radius;
			if (m_using_orthograpic) {
				SetProjAsOrthographic(m_width * m_aspect_ratio, m_height , m_near, m_far);
			}
		}
	}
}

XMFLOAT4X4 OrbitCamera::GetViewProj() {
	XMFLOAT4X4 out;

	XMMATRIX view;
	if (m_view_dirty) {

		float x = m_orbit_point.x + m_radius * sin(m_phi) * cos(m_theta);
		float z = m_orbit_point.y + m_radius * sin(m_phi) * sin(m_theta);
		float y = m_orbit_point.z + m_radius * cos(m_phi);

		XMVECTOR pos = XMVectorSet(x, y, z, 1.f);
		XMVECTOR target = XMLoadFloat3(&m_orbit_point);
		XMVECTOR up = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		//DirectX::XMVECTOR pos_to_orbit_point = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&m_orbit_point), pos));

		view = XMMatrixLookAtLH(pos, target, up);
		XMStoreFloat4x4(&m_view, view);
		m_view_dirty = false;
	}
	else {
		view = XMLoadFloat4x4(&m_view);
	}

	XMMATRIX proj = XMLoadFloat4x4(&m_proj);
	XMMATRIX view_proj = view * proj;
	XMStoreFloat4x4(&out, XMMatrixTranspose(view_proj));

	return out;
}

XMVECTOR OrbitCamera::GetPos() {
	float x = m_orbit_point.x + m_radius * sin(m_phi) * cos(m_theta);
	float z = m_orbit_point.y + m_radius * sin(m_phi) * sin(m_theta);
	float y = m_orbit_point.z + m_radius * cos(m_phi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.f);
	return pos;
}

XMVECTOR OrbitCamera::GetLookDir() {
	XMVECTOR pos = GetPos();
	XMVECTOR target = XMLoadFloat3(&m_orbit_point);
	return XMVector3Normalize(XMVectorSubtract(target, pos));
}
