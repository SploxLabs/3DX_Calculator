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
	void OnVizualizerWindowResize();

	HWND visualizer_window;

	bool visualizer_window_paused;
	#pragma endregion
	

	#pragma region CalculatorWindow
	HWND calculator_window;
	bool calculator_window_paused;
		#pragma region CalculatorWindowElements
		//creates or recreates window elements to the calculator window's current dimension
		void CreateCalculatorWindowElements();

		HWND function_label;
		HWND function_edit_box;
		
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

		HWND yresolution_label;
		HWND yresolution_edit_box;

		HWND point1_label;
		HWND point1_information_box;
		HWND point1_selection_box; //we want to switch cursor and begin tracking mouse clicks for hit detection
		
		HWND point2_label;
		HWND point2_information_box;
		HWND point2_selection_box; //we want to switch cursor and begin tracking mouse clicks for hit detection

		HWND select_point1_button;
		HWND select_point2_button;

		HWND apply_changes_button;

		HWND output_window;
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
		bool point1_selected;
		double point1_x;
		double point1_y;
		double point1_z;

		bool point2_selected;
		double point2_x;
		double point2_y;
		double point2_z;
#pragma endregion
};