#pragma once
#include "pch.h"
#include "HrDebugger.h"
#include "OrbitCamera.h"
using namespace std;
using namespace DirectX;
using Microsoft::WRL::ComPtr;

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
		/* Settings */
		D3D_FEATURE_LEVEL minimum_feature_level = D3D_FEATURE_LEVEL_11_0; //accepting dx11 and more capable cards
		static const UINT frame_buffer_count = 2; //2-double_buffering 3-triple_buffering, i'm hoping that moving from 3 to 2 will cut our memory usage by a third!
		float background_color[4] = { 0,0,0,1 };
		/* Device and Device Specifics */
		void InitDeviceAndCoreObjs();
		ComPtr<IDXGIFactory5> factory;
		ComPtr<ID3D12Device> device;
		UINT rtv_inc_size;
		UINT cbv_srv_uav_inc_size;
		ComPtr<ID3D12CommandQueue> command_queue;

		UINT current_frame_index;
		ComPtr<ID3D12Fence> fence;
		UINT64 fence_values[frame_buffer_count];
		HANDLE fence_event;

		/* Swap Chain */
		void InitSwapChain();
		ComPtr<IDXGISwapChain3> swap_chain;
		ComPtr<ID3D12DescriptorHeap> rtv_heap;
		ComPtr<ID3D12DescriptorHeap> dsv_heap;
		ComPtr<ID3D12Resource> back_buffers[frame_buffer_count];
		ComPtr<ID3D12Resource> msaa_back_buffers[frame_buffer_count];
		ComPtr<ID3D12Resource> depth_stencil_buffer;
		D3D12_VIEWPORT viewport;
		D3D12_RECT scissor_rect;

		/* Transformed Indexed Triangle Object Pipeline */
		void InitTransformedObjectPipeline();
		ComPtr<ID3D12GraphicsCommandList>	transformed_object_pipeline_command_list;
		ComPtr<ID3D12RootSignature>			transformed_object_root_signature;
		ComPtr<ID3D12PipelineState>			transformed_object_opaque_pipeline_state;
		ComPtr<ID3D12PipelineState>			transformed_object_wireframe_pipeline_state;

		void RecreateTransformedObjectVertexBuffersFor(vector<XMFLOAT3> verts);
		D3D12_VERTEX_BUFFER_VIEW	transformed_object_vertex_buffer_views[frame_buffer_count];
		ComPtr<ID3D12Resource>		transformed_object_vertex_buffers[frame_buffer_count];
		void*						transformed_object_vertex_data_begins[frame_buffer_count];

		void RecreateTransformedObjectIndexBuffersFor(vector<uint32_t> indicies);
		D3D12_INDEX_BUFFER_VIEW		transformed_object_index_buffer_views[frame_buffer_count];
		ComPtr<ID3D12Resource>		transformed_object_index_buffers[frame_buffer_count];
		void*						transformed_object_index_data_begins[frame_buffer_count];

		struct TransformationData {
			XMFLOAT4X4 matrix;
			XMFLOAT4 color;
			float padding[44]; //usable padding
		};
		static_assert((sizeof(TransformationData) % 256) == 0, "Constant Buffer item must be 256-byte aligned");

		void RecreateTransformObjectConstantBuffersFor(vector<TransformationData> transformationdatas);
		ComPtr<ID3D12DescriptorHeap>	transformed_object_cbv_heap;
		ComPtr<ID3D12Resource>			transformed_object_constant_buffers[frame_buffer_count];
		void*							transformed_object_constant_data_begins[frame_buffer_count];
		
		/* Camera */
		void InitCamera();
		OrbitCamera camera;
		struct CameraData {
			XMFLOAT4X4 matrix;
			float padding[48]; //usable padding
		};
		static_assert((sizeof(TransformationData) % 256) == 0, "Constant Buffer item must be 256-byte aligned");
		ComPtr<ID3D12DescriptorHeap>	camera_cbv_heap;
		ComPtr<ID3D12Resource>			camera_data_buffers[frame_buffer_count];
		void*							camera_data_begins[frame_buffer_count];

		XMFLOAT4X4 GetIdentity4x4();
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