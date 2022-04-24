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

#pragma region Settings
	#pragma region Current
	bool using_wireframe;
	#pragma endregion

	#pragma region Default
	bool def_using_wireframe = false;
	

	#pragma endregion
#pragma endregion

#pragma region FileIO

#pragma endregion

#pragma region Win32
	static App* self;
	static LRESULT CallOnEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);

	HWND calculator_window;
	HWND visualizer_window;

	#pragma region EventHandlers

		#pragma region MouseEvents
		void OnMouseMove(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
		void OnLMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
		void OnLMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
		void OnRMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
		void OnRMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
		void OnMMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
		void OnMMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
		void OnMouseScroll(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
		#pragma endregion
	#pragma endregion
#pragma endregion

#pragma region DirectX12
	
#pragma endregion

#pragma region Math

#pragma endregion
};