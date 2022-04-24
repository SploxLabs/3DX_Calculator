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
	LRESULT	OnCalculatorWindowEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
	LRESULT OnVisualizerWindowEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
	void	OnUpdate();
	void	OnRender();
	void	OnDestroy();

#pragma region Settings
	//void LoadConfigFile(string fname); //future feature
	bool using_wireframe = false;

#pragma endregion

#pragma region FileIO

#pragma endregion

#pragma region Win32
	static App* self;
	static LRESULT CallOnCalculatorWindowEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
	static LRESULT CallOnVisualizerWindowEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam);
	
	void InitWindows();
	
	#pragma region VisualizerWindow
	HWND visualizer_window;
	#pragma endregion
	

	#pragma region CalculatorWindow
	HWND calculator_window;

		#pragma region CalculatorWindowElements
		void CreateCalculatorWindowElements();
		UINT vertical_spacing;
		UINT horizontal_spacing;
		UINT element_height;

		HWND xmin_label;
		HWND xmin_edit_box;
		HWND xmax_label;
		HWND xmax_edit_box;

		HWND ymin_label;
		HWND ymin_edit_box;
		HWND ymax_label;
		HWND ymax_edit_box;

		HWND xresolution_label;
		HWND xresolution_edit_box;

		HWND xresolution_label;
		HWND xresolution_edit_box;

		HWND point1_label;
		HWND point1_selection_box; //we want to switch cursor and begin tracking mouse clicks for hit detection
		HWND point2_label;
		HWND point2_selection_box; //we want to switch cursor and begin tracking mouse clicks for hit detection

		#pragma endregion


	#pragma endregion

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
		void InitDX12();
#pragma endregion

#pragma region Math

#pragma endregion
};