#pragma once
#include "pch.h"
class App {
public:
	App();
	int		Start();
	void	Pause();
	void	Resume();
	void	End();
private:
	void	OnInit();
	LRESULT	OnEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
	void	OnUpdate();
	void	OnRender();
	void	OnDestroy();

#pragma region Win32
	static App* self;
	static LRESULT CallOnEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
#pragma endregion

#pragma region DirectX12

#pragma endregion

#pragma region Math

#pragma endregion
};