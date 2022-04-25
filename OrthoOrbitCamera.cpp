#include "pch.h"
#include "OrthoOrbitCamera.h"

void OrthoOrbitCamera::SetAspectRatio(float t_aspect_ratio) {
    aspect_ratio = t_aspect_ratio;
}

void OrthoOrbitCamera::SetWidth(float t_width) {
    if (t_width > width_max) {
        width = width_max;
    } else if (t_width < width_min) {
        width = width_min;
    } else {
        width = t_width;
    }
}

void OrthoOrbitCamera::SetRadius(float t_radius) {
    if (t_radius > radius_max) {
        radius = radius_max;
    }
    else if (t_radius < radius_min) {
        radius = radius_min;
    }
    else {
        radius = t_radius;
    }
}

void OrthoOrbitCamera::SetPos(XMFLOAT3 pos) {
    //radius unchanged;
    float radius = sqrt(pow(pos.x, 2) + pow(pos.y, 2) + pow(pos.z, 2));
    float conv_theta = atan(pos.y / pos.x);
    float conv_phi = acos(pos.z / radius);

    if (conv_theta > theta_max) {
        theta = theta_max;
    } else if (conv_theta < theta_min) {
        theta = theta_min;
    } else {
        theta = conv_theta;
    }

    if (conv_phi > phi_max) {
        phi = phi_max;
    } else if (conv_phi < phi_min) {
        phi = phi_min;
    } else {
        phi = conv_phi;
    }
}

void OrthoOrbitCamera::SetPos(float x, float y, float z) {
    //radius unchanged;
    float radius = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
    float conv_theta = atan(y / x);
    float conv_phi = acos(z / radius);

    SetTheta(conv_theta);
    SetPhi(conv_phi);

    if (conv_phi > phi_max) {
        phi = phi_max;
    }
    else if (conv_phi < phi_min) {
        phi = phi_min;
    }
    else {
        phi = conv_phi;
    }
}

void OrthoOrbitCamera::SetTheta(float t_theta) {
    if (t_theta > theta_max) {
        theta = theta_max;
    } else if (t_theta < theta_min) {
        theta = theta_min;
    } else {
        theta = t_theta;
    }
}

void OrthoOrbitCamera::SetPhi(float t_phi) {
    if (t_phi > phi_max) {
        phi = phi_max;
    } else if (t_phi < phi_min) {
        phi = phi_min;
    } else {
        phi = t_phi;
    }
}

void OrthoOrbitCamera::OnMouseDown(WPARAM t_wparam) {
    POINT pt;
    GetCursorPos(&pt);
    m_anchor_point_x = pt.x;
    m_anchor_point_y = pt.y;
}

void OrthoOrbitCamera::OnMouseUp(WPARAM t_wparam) {

}

void OrthoOrbitCamera::OnMouseMove(WPARAM t_wparam) {
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
            float dtheta = static_cast<float>(dx) * m_horizontal_radians_per_pixel * horizontal_sensitivity;
            float dphi = static_cast<float>(dy) * m_vertical_radians_per_pixel * vertical_sensitivity;
            SetTheta(theta + dtheta);
            SetPhi(phi + dphi);
        }
        else if ((t_wparam & MK_RBUTTON) != 0) {
            float dx_r = 0.005f * static_cast<float>(dx) * zooming_sensitivity;
            float dy_r = 0.005f * static_cast<float>(dy) * zooming_sensitivity;
            SetWidth(width + (dx_r - dy_r));
        }
    }
}

void OrthoOrbitCamera::OnMouseScroll(WPARAM t_wparam) {
}

XMFLOAT4X4 OrthoOrbitCamera::GetViewProj() {
    XMFLOAT4X4 out;
   
    float x = orbit_point.x + radius * sin(phi) * cos(theta);
    float z = orbit_point.y + radius * sin(phi) * sin(theta);
    float y = orbit_point.z + radius * cos(phi);

    XMVECTOR pos = DirectX::XMVectorSet(x, y, z, 1.f);
    XMVECTOR target = DirectX::XMLoadFloat3(&orbit_point);
    XMVECTOR up = DirectX::XMVectorSet(0.f, 1.f, 0.f, 0.f);

    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMMATRIX proj = XMMatrixOrthographicLH(width, width / aspect_ratio, 0.f, FLT_MAX);
    
    XMStoreFloat4x4(&out, XMMatrixTranspose(view * proj));

    return out;
}

XMVECTOR OrthoOrbitCamera::GetPos() {
    return XMVECTOR();
}

XMVECTOR OrthoOrbitCamera::GetLookDir() {
    return XMVECTOR();
}
