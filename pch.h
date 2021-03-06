// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

#include "resource.h"

#include<stdexcept>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include<d3d12.h>
#include<dxgi1_6.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#include<DirectXCollision.h>
#include<DirectXColors.h>
#include<wrl.h>

#include "d3dx12.h"

#endif //PCH_H
