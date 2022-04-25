#include "pch.h"
#include "App.h"

App* App::self = nullptr;

App::App() :
	calculator_window(nullptr),
	visualizer_window(nullptr) 
{
	self = this;
}

int App::Start() {
    OnInit();
    ShowWindow(calculator_window, SW_SHOW);
    ShowWindow(visualizer_window, SW_SHOW);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            OnUpdate();
            OnRender();
        }
    }
    OnDestroy();
    return 0;
}

void App::Pause() {
}

void App::Resume() {
}

void App::End() {
}

void App::OnInit() {
	InitWindows();
	InitDX12();
}

LRESULT App::OnCalculatorWindowEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
    switch (t_msg) {

    case WM_SYSCOMMAND: {
        switch (t_wparam) {
        case SC_MINIMIZE:
            calculator_window_paused = true;
            break;
        case SC_RESTORE:
            calculator_window_paused = false;
            break;
        default:
            break;
        }
        break;
    }

#pragma region MenuAndButtonEvents
    case WM_COMMAND: {
        switch (LOWORD(t_wparam)) {
        case ID_FILE_NEW:
        case ID_FILE_OPEN:
        case ID_FILE_SAVE:
        case ID_FILE_SAVEAS:
            //...
            break;

        case ID_SELECT_POINT1: {
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            break;
        }
        case ID_SELECT_POINT2: {
            SetCursor(LoadCursor(NULL, IDC_CROSS));
            break;
        }
        case ID_REVALUATE: {
            break;
        }
        break;
        }
        break;
    }
    //case WM_SETCURSOR: { return true; }

#pragma endregion
        
    case WM_SIZE: {
        DefWindowProc(t_hwnd, t_msg, t_wparam, t_lparam);
        CreateCalculatorWindowElements();
        break;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default:
        break;
    }
    
    return DefWindowProc(t_hwnd, t_msg, t_wparam, t_lparam);
}

LRESULT App::OnVisualizerWindowEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
    switch (t_msg) {

    #pragma region MouseEvents
    case WM_MOUSEMOVE: {
        OnMouseMove(t_hwnd, t_msg, t_wparam, t_lparam);
        break;
    }
    case WM_LBUTTONDOWN: {
        OnLMouseDown(t_hwnd, t_msg, t_wparam, t_lparam);
        break;
    }
    case WM_LBUTTONUP: {
        OnLMouseUp(t_hwnd, t_msg, t_wparam, t_lparam);
        break;
    }
    case WM_RBUTTONDOWN: {
        OnRMouseDown(t_hwnd, t_msg, t_wparam, t_lparam);
        break;
    }
    case WM_RBUTTONUP: {
        OnRMouseUp(t_hwnd, t_msg, t_wparam, t_lparam);
        break;
    }
    case WM_MBUTTONDOWN: {
        OnMMouseDown(t_hwnd, t_msg, t_wparam, t_lparam);
        break;
    }
    case WM_MBUTTONUP: {
        OnMMouseUp(t_hwnd, t_msg, t_wparam, t_lparam);
        break;
    }
    case WM_MOUSEHWHEEL: {
        OnMouseScroll(t_hwnd, t_msg, t_wparam, t_lparam);
        break;
    }
    #pragma endregion



    case WM_SYSCOMMAND: {
        switch (t_wparam) {
        case SC_MINIMIZE:
            visualizer_window_paused = true;
            break;
        case SC_RESTORE:
            visualizer_window_paused = false;
            break;
        default:
            break;
        }
        break;
    }

    case WM_SIZE: {
        DefWindowProc(t_hwnd, t_msg, t_wparam, t_lparam);
        OnVizualizerWindowResize();
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default:
        break;
    }
    return DefWindowProc(t_hwnd, t_msg, t_wparam, t_lparam);
}

void App::OnUpdate() {
    camera_cpu_data.matrix = camera.GetViewProj();
    memcpy(camera_data_begins[current_frame_index], &camera_cpu_data, sizeof(camera_cpu_data));
}

void App::OnRender() {
    TIF(transformed_object_command_allocators[current_frame_index]->Reset());

    if (use_wireframe) {
        TIF(transformed_object_pipeline_command_list->Reset(transformed_object_command_allocators[frame_buffer_count].Get(), transformed_object_wireframe_pipeline_state.Get()));
        
    }
    else {
        TIF(transformed_object_pipeline_command_list->Reset(transformed_object_command_allocators[frame_buffer_count].Get(), transformed_object_opaque_pipeline_state.Get()));
    }

    transformed_object_pipeline_command_list->SetGraphicsRootSignature(transformed_object_root_signature.Get());

    ID3D12DescriptorHeap* ppHeaps[] = { camera_cbv_heap.Get() };
    transformed_object_pipeline_command_list->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    CD3DX12_GPU_DESCRIPTOR_HANDLE camera_cbv_handle(camera_cbv_heap->GetGPUDescriptorHandleForHeapStart());
    
    camera_cbv_handle.Offset(current_frame_index, cbv_srv_uav_inc_size);
    transformed_object_pipeline_command_list->SetGraphicsRootDescriptorTable(1, camera_cbv_handle);

    ID3D12DescriptorHeap* object_heaps[] = { transformed_object_cbv_heap.Get() };
    transformed_object_pipeline_command_list->SetDescriptorHeaps(_countof(object_heaps), object_heaps);

    transformed_object_pipeline_command_list->RSSetViewports(1, &viewport);

    transformed_object_pipeline_command_list->RSSetScissorRects(1, &scissor_rect);

    transformed_object_pipeline_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(msaa_back_buffers[current_frame_index].Get(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET));

    transformed_object_pipeline_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(back_buffers[current_frame_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RESOLVE_DEST));


    //command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(depth_stencil_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
    //line_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(depth_stencil_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
    //object_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(depth_stencil_buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

    CD3DX12_CPU_DESCRIPTOR_HANDLE msaa_rtv_handle(rtv_heap->GetCPUDescriptorHandleForHeapStart());
    msaa_rtv_handle.Offset(current_frame_index, rtv_inc_size);


    transformed_object_pipeline_command_list->OMSetRenderTargets(1, &msaa_rtv_handle, true, &dsv_heap->GetCPUDescriptorHandleForHeapStart());

    FLOAT color[4] = { 0.f, 0.f, 0.f, 1.f };
    transformed_object_pipeline_command_list->ClearRenderTargetView(msaa_rtv_handle, color, 0, nullptr);
    transformed_object_pipeline_command_list->ClearDepthStencilView(dsv_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1, 0, 0, nullptr);
    transformed_object_pipeline_command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    CD3DX12_GPU_DESCRIPTOR_HANDLE object_cbv_handle(transformed_object_cbv_heap->GetGPUDescriptorHandleForHeapStart());
    object_cbv_handle.Offset(current_frame_index, cbv_srv_uav_inc_size);
    transformed_object_pipeline_command_list->SetGraphicsRootDescriptorTable(1, object_cbv_handle);
    transformed_object_pipeline_command_list->IASetVertexBuffers(0, 1, &transformed_object_vertex_buffer_views[current_frame_index]);
    transformed_object_pipeline_command_list->IASetIndexBuffer(&transformed_object_index_buffer_views[current_frame_index]);
    transformed_object_pipeline_command_list->DrawIndexedInstanced(3, 1, 0, 0, 0);


    transformed_object_pipeline_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(msaa_back_buffers[current_frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE));

    transformed_object_pipeline_command_list->ResolveSubresource(back_buffers[current_frame_index].Get(), 0, msaa_back_buffers[current_frame_index].Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);

    transformed_object_pipeline_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(back_buffers[current_frame_index].Get(), D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_PRESENT));

    transformed_object_pipeline_command_list->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(depth_stencil_buffer.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON));

    TIF(transformed_object_pipeline_command_list->Close());

    ID3D12CommandList* ppCommandLists[] = { transformed_object_pipeline_command_list.Get() };
    command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    TIF(swap_chain->Present(1, 0));
    //timer.Tick();
    //m_frames_rendered++;

    const UINT64 currentFenceValue = fence_values[current_frame_index];
    TIF(command_queue->Signal(fence.Get(), currentFenceValue));
    current_frame_index = swap_chain->GetCurrentBackBufferIndex();

    if (fence->GetCompletedValue() < fence_values[current_frame_index])
    {
        TIF(fence->SetEventOnCompletion(fence_values[current_frame_index], fence_event)); //set fence point
        WaitForSingleObjectEx(fence_event, INFINITE, FALSE); //wait till we hit the fence point
    }
    fence_values[current_frame_index] = currentFenceValue + 1;
}

void App::OnDestroy() {
    FlushCommandQueue();
    swap_chain->SetFullscreenState(false, nullptr);
    CloseHandle(fence_event);
}

LRESULT App::CallOnCalculatorWindowEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
    return self->OnCalculatorWindowEvent(t_hwnd, t_msg, t_wparam, t_lparam);
}

LRESULT App::CallOnVisualizerWindowEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
    return self->OnVisualizerWindowEvent(t_hwnd, t_msg, t_wparam, t_lparam);
}

void App::InitWindows() {

	int visualizer_to_calculator_window_width_ratio = 5;

    /* Get Main Monitor Resolution (minus task bar) */
    RECT monitor;
    SystemParametersInfo(SPI_GETWORKAREA, NULL, &monitor, NULL);
    UINT monitor_width = monitor.right - monitor.left;
    UINT monitor_height = monitor.bottom - monitor.top;
    /* Define calculator window dimensions */
    UINT calculator_window_width = monitor_width / visualizer_to_calculator_window_width_ratio; //allocate fraction of available screen real-estate
    UINT calculator_window_height = monitor_height;
    /* Define visualizer window dimensions */
    UINT visualizer_window_width = monitor_width - calculator_window_width; //allocate remaining space
    UINT visualizer_window_height = monitor_height;
    /* Define Calculator Window Class, Register Class, and Create Calculator Window */
    {
        WNDCLASS wc{};
        wc.lpszClassName = L"CalculatorWindowClass";
        wc.lpfnWndProc = CallOnCalculatorWindowEvent;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 3);
        if (!RegisterClass(&wc)) {
            MessageBox(NULL, L"CalculatorWindowClass Failed to Register!", L"Initialization Error!", MB_ICONWARNING);
            exit(1);
        }

        calculator_window = CreateWindow(
            wc.lpszClassName,
            L"Calculator Window",
            WS_OVERLAPPEDWINDOW,
            0, 0,
            calculator_window_width, calculator_window_height,
            nullptr,
            nullptr,
            GetModuleHandle(NULL),
            nullptr
        );

        if (!calculator_window) {
            MessageBox(NULL, L"CalculatorWindow Creation Failed!", L"Initialization Error!", MB_ICONWARNING);
            exit(1);
        }
    }
    
    /* Define Visualizer Window Class, Register Class, and Create Visualizer Window */
    {
        WNDCLASS wc{};
        wc.lpszClassName = L"VisualizaerWindowClass";
        wc.lpfnWndProc = CallOnVisualizerWindowEvent;
        wc.hInstance = GetModuleHandle(NULL);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 3);
        if (!RegisterClass(&wc)) {
            MessageBox(NULL, L"VisualizaerWindowClass Failed to Register!", L"Initialization Error!", MB_ICONWARNING);
            exit(1);
        }

        visualizer_window = CreateWindow(
            wc.lpszClassName,
            L"Visualizer Window",
            WS_OVERLAPPEDWINDOW,
            calculator_window_width, 0,
            visualizer_window_width, visualizer_window_height,
            nullptr,
            nullptr,
            GetModuleHandle(NULL),
            nullptr
        );

        if (!calculator_window) {
            MessageBox(NULL, L"CalculatorWindow Creation Failed!", L"Initialization Error!", MB_ICONWARNING);
            exit(1);
        }
    }
}

void App::OnVizualizerWindowResize() {
    /* Get Vizualizer Window Size */
    RECT vizualizer_client_rect;
    GetClientRect(calculator_window, &vizualizer_client_rect);
}

void App::CreateCalculatorWindowElements() {
    /* Get Window Size */
    RECT calculator_client_rect;
    GetClientRect(calculator_window, &calculator_client_rect);
    UINT calculator_client_width = calculator_client_rect.right - calculator_client_rect.left;
    UINT calculator_client_height = calculator_client_rect.bottom - calculator_client_rect.top;
    UINT charactor_width = 7;

    /* Settings here */
    UINT vertical_spacing = 5;
    UINT horizontal_spacing = 5;
    UINT element_height = 20;
    
    UINT current_vertical_offset = 0;
    UINT current_horizontal_offset = 0;

    /* Copy Data, Destroy, and Recreate all calculator window elements */
    
    wchar_t str[360] = {};

    /* function label and edit box, they are on the same line*/ 
    {
        DestroyWindow(function_label);
        function_label = CreateWindowEx(NULL, L"STATIC", L"Function: ", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
            //top_left_position
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            //width and height
            charactor_width * 10, element_height,
            calculator_window, NULL, NULL, NULL);

        current_horizontal_offset += charactor_width * 10;


        ZeroMemory(str, sizeof(str));
        if (function_edit_box) {
            GetWindowText(function_edit_box, str, 360);
            DestroyWindow(function_edit_box);
        }
        std::wstring expression(str);
        function_edit_box = CreateWindowEx(NULL, L"EDIT", str, WS_VISIBLE | WS_CHILD | WS_BORDER,
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            calculator_client_width - (current_horizontal_offset + horizontal_spacing), element_height, //potentially negative value here
            calculator_window, NULL, GetModuleHandle(NULL), NULL);

        //increment the vertical offset
        current_vertical_offset += element_height + vertical_spacing;
        current_horizontal_offset = 0; //reset
    }

    /* xmin, xmax, ymin, ymax - on same line*/
    {
        DestroyWindow(xmin_label);
        xmin_label = CreateWindowEx(NULL, L"STATIC", L"Xmin: ", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
            //top_left_position
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            //width and height
            charactor_width * 6, element_height,
            calculator_window, NULL, NULL, NULL);

        current_horizontal_offset = charactor_width * 6;

        {
            ZeroMemory(str, sizeof(str));
            if (xmin_edit_box) {
                GetWindowText(xmin_edit_box, str, 360);
                DestroyWindow(xmin_edit_box);
            }
            std::wstring expression(str);
            xmin_edit_box = CreateWindowEx(NULL, L"EDIT", str, WS_VISIBLE | WS_CHILD | WS_BORDER,
                current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
                charactor_width * 10, element_height, //potentially negative value here
                calculator_window, NULL, GetModuleHandle(NULL), NULL);

            current_horizontal_offset += charactor_width * 10;
        }

        DestroyWindow(xmax_label);
        xmax_label = CreateWindowEx(NULL, L"STATIC", L"Xmax: ", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
            //top_left_position
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            //width and height
            charactor_width * 6, element_height,
            calculator_window, NULL, NULL, NULL);

        current_horizontal_offset += charactor_width * 6;

        {
            ZeroMemory(str, sizeof(str));
            if (xmax_edit_box) {
                GetWindowText(xmax_edit_box, str, 360);
                DestroyWindow(xmax_edit_box);
            }
            std::wstring expression(str);
            xmax_edit_box = CreateWindowEx(NULL, L"EDIT", str, WS_VISIBLE | WS_CHILD | WS_BORDER,
                current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
                charactor_width * 10, element_height, //potentially negative value here
                calculator_window, NULL, GetModuleHandle(NULL), NULL);

            current_horizontal_offset += charactor_width * 10;
        }

        DestroyWindow(ymin_label);
        ymin_label = CreateWindowEx(NULL, L"STATIC", L"Ymin: ", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
            //top_left_position
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            //width and height
            charactor_width * 6, element_height,
            calculator_window, NULL, NULL, NULL);

        current_horizontal_offset += charactor_width * 6;
        
        {
            ZeroMemory(str, sizeof(str));
            if (ymin_edit_box) {
                GetWindowText(ymin_edit_box, str, 360);
                DestroyWindow(ymin_edit_box);
            }
            std::wstring expression(str);
            ymin_edit_box = CreateWindowEx(NULL, L"EDIT", str, WS_VISIBLE | WS_CHILD | WS_BORDER,
                current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
                charactor_width * 10, element_height, //potentially negative value here
                calculator_window, NULL, GetModuleHandle(NULL), NULL);

            current_horizontal_offset += charactor_width * 10;
        }

        DestroyWindow(ymax_label);
        ymax_label = CreateWindowEx(NULL, L"STATIC", L"Ymax: ", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
            //top_left_position
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            //width and height
            charactor_width * 6, element_height,
            calculator_window, NULL, NULL, NULL);

        current_horizontal_offset += charactor_width * 6;

        {
            ZeroMemory(str, sizeof(str));
            if (ymax_edit_box) {
                GetWindowText(ymax_edit_box, str, 360);
                DestroyWindow(ymax_edit_box);
            }
            std::wstring expression(str);
            ymax_edit_box = CreateWindowEx(NULL, L"EDIT", str, WS_VISIBLE | WS_CHILD | WS_BORDER,
                current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
                charactor_width * 10, element_height, //potentially negative value here
                calculator_window, NULL, GetModuleHandle(NULL), NULL);

            current_horizontal_offset += charactor_width * 10;
        }

        current_vertical_offset += element_height + vertical_spacing;
        current_horizontal_offset = 0; //reset
    }

    /* Resolution labels and boxes */
    {
        DestroyWindow(xresolution_label);
        xresolution_label = CreateWindowEx(NULL, L"STATIC", L"Resolution X-Axis: ", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
            //top_left_position
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            //width and height
            charactor_width * 19, element_height,
            calculator_window, NULL, NULL, NULL);

        current_horizontal_offset += charactor_width * 19;

        {
            ZeroMemory(str, sizeof(str));
            if (xresolution_edit_box) {
                GetWindowText(xresolution_edit_box, str, 360);
                DestroyWindow(xresolution_edit_box);
            }
            std::wstring expression(str);
            xresolution_edit_box = CreateWindowEx(NULL, L"EDIT", str, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
                charactor_width * 10, element_height,
                calculator_window, NULL, GetModuleHandle(NULL), NULL);

            current_horizontal_offset += charactor_width * 10;
        }

        DestroyWindow(yresolution_label);
        yresolution_label = CreateWindowEx(NULL, L"STATIC", L"Resolution Y-Axis: ", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
            //top_left_position
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            //width and height
            charactor_width * 19, element_height,
            calculator_window, NULL, NULL, NULL);

        current_horizontal_offset += charactor_width * 19;

        {
            ZeroMemory(str, sizeof(str));
            if (yresolution_edit_box) {
                GetWindowText(yresolution_edit_box, str, 360);
                DestroyWindow(yresolution_edit_box);
            }
            std::wstring expression(str);
            yresolution_edit_box = CreateWindowEx(NULL, L"EDIT", str, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
                current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
                charactor_width * 10, element_height,
                calculator_window, NULL, GetModuleHandle(NULL), NULL);

            current_horizontal_offset += charactor_width * 10;
        }
        current_vertical_offset += element_height + vertical_spacing;
        current_horizontal_offset = 0; //reset
    }

    /* points */
    {
        DestroyWindow(point1_label);
        point1_label = CreateWindowEx(NULL, L"STATIC", L"Point1:", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
            //top_left_position
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            //width and height
            charactor_width * 8, element_height,
            calculator_window, NULL, NULL, NULL);
        current_horizontal_offset += charactor_width * 8;

        {
            ZeroMemory(str, sizeof(str));
            if (point1_information_box) {
                GetWindowText(point1_information_box, str, 360);
                DestroyWindow(point1_information_box);
            }
            std::wstring expression(str);
            point1_information_box = CreateWindowEx(NULL, L"STATIC", str, WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
                current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
                calculator_client_width - current_horizontal_offset, element_height,
                calculator_window, NULL, GetModuleHandle(NULL), NULL);

            current_vertical_offset += element_height + vertical_spacing;
            current_horizontal_offset = 0; //reset
        }
        

        DestroyWindow(point2_label);
        point2_label = CreateWindowEx(NULL, L"STATIC", L"Point2:", WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
            //top_left_position
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            //width and height
            charactor_width * 8, element_height,
            calculator_window, NULL, NULL, NULL);
        current_horizontal_offset += charactor_width * 8;

        {
            ZeroMemory(str, sizeof(str));
            if (point2_information_box) {
                GetWindowText(point2_information_box, str, 360);
                DestroyWindow(point2_information_box);
            }
            std::wstring expression(str);
            point2_information_box = CreateWindowEx(NULL, L"STATIC", str, WS_VISIBLE | WS_CHILD | SS_CENTER | WS_BORDER,
                current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
                calculator_client_width - current_horizontal_offset, element_height,
                calculator_window, NULL, GetModuleHandle(NULL), NULL);

            current_vertical_offset += element_height + vertical_spacing;
            current_horizontal_offset = 0; //reset
        }
        
    }

    /* point selection and reapply settings buttons */
    {

        DestroyWindow(select_point1_button);
        select_point1_button = CreateWindowEx(NULL, L"BUTTON", L"Select Point 1", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing, 
            charactor_width * 16, element_height,
            calculator_window, (HMENU)ID_SELECT_POINT1, NULL, NULL);
        current_horizontal_offset += charactor_width * 16;

        DestroyWindow(select_point2_button);
        select_point2_button = CreateWindowEx(NULL, L"BUTTON", L"Select Point 2", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            charactor_width * 16, element_height,
            calculator_window, (HMENU)ID_SELECT_POINT2, NULL, NULL);
        current_horizontal_offset += charactor_width * 16;

        DestroyWindow(apply_changes_button);
        apply_changes_button = CreateWindowEx(NULL, L"BUTTON", L"Apply / Revaluate", WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            calculator_client_width - current_horizontal_offset, element_height,
            calculator_window, (HMENU)ID_REVALUATE, NULL, NULL);
        current_vertical_offset += element_height + vertical_spacing;
        current_horizontal_offset = 0; //reset
    }

    /* output window */
    {
        wchar_t ostr[9999] = {};
        if (output_window) {
            GetWindowText(output_window, ostr, 9999);
            DestroyWindow(output_window);
        }
        output_window = CreateWindowEx(NULL, L"STATIC", ostr, WS_VISIBLE | WS_CHILD | SS_LEFT | WS_BORDER,
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            calculator_client_width - current_horizontal_offset, calculator_client_height - current_vertical_offset,
            calculator_window, NULL, GetModuleHandle(NULL), NULL);
    }

}

void App::InitDX12() {
    InitDeviceAndCoreObjs();
    InitSwapChain();
    InitTransformedObjectPipeline();

    vector<VertexC> test_triangle_verts = {
        {{0,0,0},{1,1,1,1}},
        {{0,1,0},{1,1,1,1}},
        {{1,1,0},{1,1,1,1}}
    };
    vector<uint32_t> test_triangle_indicies = {
        0,1,2
    };
    TransformationData test_triangle_transformation_data;
    test_triangle_transformation_data.matrix = GetIdentity4x4();
    vector<TransformationData> test_triangle_transformation_datas = { test_triangle_transformation_data };

    RecreateTransformedObjectVertexBuffersFor(test_triangle_verts);
    RecreateTransformedObjectIndexBuffersFor(test_triangle_indicies);
    RecreateTransformObjectConstantBuffersFor(test_triangle_transformation_datas);

    InitCamera();
}

void App::InitDeviceAndCoreObjs() {
    /* Enable Debug Layer and Create Factory*/
    UINT factory_flag;
#ifdef _DEBUG
    factory_flag = DXGI_CREATE_FACTORY_DEBUG;
    ComPtr<ID3D12Debug> debug;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)))) {
        debug->EnableDebugLayer();
    }
    else {
        TIF(S_FALSE);
    }
#else
    factory_flag = 0;
#endif 
    TIF(CreateDXGIFactory2(factory_flag, IID_PPV_ARGS(&factory)));

    /* Create Device */
    TIF(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));
    rtv_inc_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    cbv_srv_uav_inc_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    
    /* Create Synchornization Objs */
    TIF(device->CreateFence(fence_values[current_frame_index], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
    fence_values[current_frame_index]++;

    // Create an event handle to use for frame synchronization.
    fence_event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fence_event == nullptr) {
        TIF(HRESULT_FROM_WIN32(GetLastError()));
    }

    /* Create Command Queue */
    D3D12_COMMAND_QUEUE_DESC command_queue_desc;
    command_queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    command_queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    command_queue_desc.NodeMask = 0;
    command_queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    TIF(device->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(&command_queue)));

}

void App::FlushCommandQueue() {
    TIF(command_queue->Signal(fence.Get(), fence_values[current_frame_index]));

    TIF(fence->SetEventOnCompletion(fence_values[current_frame_index], fence_event)); //set fence point
    WaitForSingleObjectEx(fence_event, INFINITE, FALSE); //wait till we hit the fence point
    fence_values[current_frame_index]++;
}

void App::InitSwapChain() {
    /* Create Swap Chain for vizualizer window */
    RECT vizualizer_window_client_rect;
    GetClientRect(visualizer_window, &vizualizer_window_client_rect);
    UINT vizualizer_window_client_width = vizualizer_window_client_rect.right - vizualizer_window_client_rect.left;
    UINT vizualizer_window_client_height = vizualizer_window_client_rect.bottom - vizualizer_window_client_rect.top;

    DXGI_SWAP_CHAIN_DESC1 swap_chain_desc = {};
    swap_chain_desc.Width = vizualizer_window_client_width;
    swap_chain_desc.Height = vizualizer_window_client_height;
    swap_chain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swap_chain_desc.Stereo;
    swap_chain_desc.SampleDesc.Count = 1;
    swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swap_chain_desc.BufferCount = frame_buffer_count;
    swap_chain_desc.Scaling;
    swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swap_chain_desc.AlphaMode;
    swap_chain_desc.Flags;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swap_chain_fullscreen_desc = {};
    swap_chain_fullscreen_desc.RefreshRate.Numerator = 60;
    swap_chain_fullscreen_desc.RefreshRate.Denominator = 1;
    swap_chain_fullscreen_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swap_chain_fullscreen_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swap_chain_fullscreen_desc.Windowed = true;
    ComPtr<IDXGISwapChain1> swap_chain_tmp;
    TIF(factory->CreateSwapChainForHwnd(
        command_queue.Get(),
        visualizer_window,
        &swap_chain_desc,
        &swap_chain_fullscreen_desc,
        nullptr,
        &swap_chain_tmp));
    TIF(swap_chain_tmp.As(&swap_chain));

    /* Create Render Target Heap */
    D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
    rtv_heap_desc.NumDescriptors = frame_buffer_count;
    rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    TIF(device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_heap)));
    
    /* Create Back Buffers */
    for (int i = 0; i < frame_buffer_count; i++) {
        TIF(swap_chain->GetBuffer(i, IID_PPV_ARGS(&back_buffers[i])));
    }

    /* Create MSAA back buffers and store views in rtv heap*/
    D3D12_RESOURCE_DESC msaa_resource_desc;
    msaa_resource_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    msaa_resource_desc.Alignment = 0;
    msaa_resource_desc.Width = vizualizer_window_client_width; //Buffer size in bytes
    msaa_resource_desc.Height = vizualizer_window_client_height;
    msaa_resource_desc.DepthOrArraySize = 1;
    msaa_resource_desc.MipLevels = 1;
    msaa_resource_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    msaa_resource_desc.SampleDesc.Count = 4;
    msaa_resource_desc.SampleDesc.Quality = 0;
    msaa_resource_desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    msaa_resource_desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    D3D12_CLEAR_VALUE clear_value;
    clear_value.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    clear_value.Color[0] = background_color[0];
    clear_value.Color[1] = background_color[0];
    clear_value.Color[2] = background_color[0];
    clear_value.Color[3] = background_color[1];
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_heap->GetCPUDescriptorHandleForHeapStart());
    for (int i = 0; i < frame_buffer_count; i++) {
        TIF(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &msaa_resource_desc,
            D3D12_RESOURCE_STATE_COPY_SOURCE,
            &clear_value,
            IID_PPV_ARGS(&msaa_back_buffers[i])));

        device->CreateRenderTargetView(msaa_back_buffers[i].Get(), nullptr, rtv_handle);

        rtv_handle.Offset(1, rtv_inc_size);
    }

    /* Create Depth Stencil Resources */
    //Create Depth Stencil View Heap
    D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
    dsv_heap_desc.NumDescriptors = 1;
    dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsv_heap_desc.NodeMask = 0;
    TIF(device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(dsv_heap.GetAddressOf())));

    //Create Depth Stencil Buffer
    D3D12_CLEAR_VALUE opt_clear;
    opt_clear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    opt_clear.DepthStencil.Depth = 1;
    opt_clear.DepthStencil.Stencil = 0;

    TIF(device->CreateCommittedResource(
        &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
        D3D12_HEAP_FLAG_NONE,
        &CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D24_UNORM_S8_UINT, vizualizer_window_client_width, vizualizer_window_client_height, 1, 1, 4, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &opt_clear,
        IID_PPV_ARGS(depth_stencil_buffer.GetAddressOf())));

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc{};
    dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
    dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
    dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsv_desc.Texture2D.MipSlice = 0;
    device->CreateDepthStencilView(depth_stencil_buffer.Get(), &dsv_desc, dsv_heap->GetCPUDescriptorHandleForHeapStart());

    /* Set Viewport and Scissor Rect*/
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = vizualizer_window_client_width;
    viewport.Height = vizualizer_window_client_height;
    viewport.MinDepth = D3D12_MIN_DEPTH;
    viewport.MaxDepth = D3D12_MAX_DEPTH;

    scissor_rect.left = 0;
    scissor_rect.top = 0;
    scissor_rect.right = vizualizer_window_client_width;
    scissor_rect.bottom = vizualizer_window_client_height;

}

void App::InitTransformedObjectPipeline() {
    /* Create Root Signature */
    CD3DX12_DESCRIPTOR_RANGE camera_matrix_desc_range;
    camera_matrix_desc_range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    CD3DX12_DESCRIPTOR_RANGE object_matrix_desc_range;
    object_matrix_desc_range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);

    CD3DX12_ROOT_PARAMETER slot_root_params[2];
    slot_root_params[0].InitAsDescriptorTable(1, &camera_matrix_desc_range);
    slot_root_params[1].InitAsDescriptorTable(1, &object_matrix_desc_range);

    CD3DX12_ROOT_SIGNATURE_DESC root_signature_desc(2, slot_root_params, 0, nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> serialized_root_signature;
    ComPtr<ID3DBlob> error_blob;
    HRESULT hr = D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1,
        serialized_root_signature.GetAddressOf(), error_blob.GetAddressOf());

    if (error_blob != nullptr) {
        OutputDebugStringA((char*)error_blob->GetBufferPointer());
    }
    TIF(hr);
    
    TIF(device->CreateRootSignature(
        0,
        serialized_root_signature->GetBufferPointer(),
        serialized_root_signature->GetBufferSize(),
        IID_PPV_ARGS(transformed_object_root_signature.GetAddressOf())));

    /* Compile Shaders */
    ComPtr<ID3DBlob> vertex_shader;
    ComPtr<ID3DBlob> pixel_shader;
#if defined(_DEBUG)
    // Enable better shader debugging with the graphics debugging tools.
    UINT compile_flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compile_flags = 0;
#endif
    TIF(D3DCompileFromFile(L"color_object.hlsl", nullptr, nullptr, "VS", "vs_5_0", compile_flags, 0, &vertex_shader, nullptr));
    TIF(D3DCompileFromFile(L"color_object.hlsl", nullptr, nullptr, "PS", "ps_5_0", compile_flags, 0, &pixel_shader, nullptr));

    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
    };

    // Describe and create the graphics pipeline state object (PSO).
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {};
        pso_desc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        pso_desc.pRootSignature = transformed_object_root_signature.Get();
        pso_desc.VS.pShaderBytecode = vertex_shader->GetBufferPointer();
        pso_desc.VS.BytecodeLength = vertex_shader->GetBufferSize();
        pso_desc.PS.pShaderBytecode = pixel_shader->GetBufferPointer();
        pso_desc.PS.BytecodeLength = pixel_shader->GetBufferSize();
        pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        pso_desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);;
        pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        pso_desc.SampleMask = UINT_MAX;
        pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        pso_desc.NumRenderTargets = 1;
        pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        pso_desc.SampleDesc.Count = 4;
        pso_desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
        TIF(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&transformed_object_opaque_pipeline_state)));
        pso_desc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
        TIF(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&transformed_object_wireframe_pipeline_state)));

    }

    /* Create Command Allocators */
    for (int i = 0; i < frame_buffer_count; i++) {
        TIF(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&transformed_object_command_allocators[i])));
    }

    /* Create Command List */
    TIF(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, transformed_object_command_allocators[current_frame_index].Get(), transformed_object_opaque_pipeline_state.Get(), IID_PPV_ARGS(&transformed_object_pipeline_command_list)));
    transformed_object_pipeline_command_list->Close();
}

XMFLOAT4X4 App::GetIdentity4x4() {
    return XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

void App::RecreateTransformedObjectVertexBuffersFor(const vector<VertexC>& verts) {
    UINT size = verts.size() * sizeof(verts[0]);

    for (int i = 0; i < frame_buffer_count; i++) {
        if (transformed_object_vertex_buffers[i]) {
            transformed_object_vertex_buffers[i]->Unmap(0, NULL);
            transformed_object_vertex_data_begins[i] = nullptr;
            transformed_object_vertex_buffers[i].Reset();
            transformed_object_vertex_buffers[i] = nullptr;

            TIF(device->CreateCommittedResource(
                &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
                D3D12_HEAP_FLAG_NONE,
                &CD3DX12_RESOURCE_DESC::Buffer(size),
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&transformed_object_vertex_buffers[i])));

            D3D12_RANGE read_range;
            read_range.Begin = 0;
            read_range.End = 0;

            TIF(transformed_object_vertex_buffers[i]->Map(0, &read_range, &transformed_object_vertex_data_begins[i]));

            memcpy(transformed_object_vertex_data_begins[i], verts.data(), size);

            transformed_object_vertex_buffer_views[i].BufferLocation = transformed_object_vertex_buffers[i]->GetGPUVirtualAddress();
            transformed_object_vertex_buffer_views[i].SizeInBytes = size;
            transformed_object_vertex_buffer_views[i].StrideInBytes = sizeof(VertexC);

        }
    }
}

void App::RecreateTransformedObjectIndexBuffersFor(const vector<uint32_t>& indicies) {
    FlushCommandQueue();

    UINT size = indicies.size() * sizeof(indicies[0]);

    for (int i = 0; i < frame_buffer_count; i++) {
        if (transformed_object_index_buffers[i]) {
            transformed_object_index_buffers[i]->Unmap(0, NULL);
            transformed_object_index_data_begins[i] = nullptr;
            transformed_object_index_buffers[i].Reset();
        }
        TIF(device->CreateCommittedResource(
             &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
             D3D12_HEAP_FLAG_NONE,
             &CD3DX12_RESOURCE_DESC::Buffer(size),
             D3D12_RESOURCE_STATE_GENERIC_READ,
             nullptr,
             IID_PPV_ARGS(&transformed_object_index_buffers[i])));

         D3D12_RANGE read_range;
         read_range.Begin = 0;
         read_range.End = 0;

         TIF(transformed_object_index_buffers[i]->Map(0, &read_range, &transformed_object_index_data_begins[i]));

         memcpy(transformed_object_index_data_begins[i], indicies.data(), size);

         transformed_object_index_buffer_views[i].BufferLocation = transformed_object_index_buffers[i]->GetGPUVirtualAddress();
         transformed_object_index_buffer_views[i].SizeInBytes = size;
        
    }
}

void App::RecreateTransformObjectConstantBuffersFor(const vector<TransformationData>& transformationdata) {
    if (transformed_object_cbv_heap) {
        transformed_object_cbv_heap.Reset();
    }
    D3D12_DESCRIPTOR_HEAP_DESC object_cbv_heap_desc;
    object_cbv_heap_desc.NumDescriptors = frame_buffer_count * transformationdata.size(); //correct
    object_cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    object_cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    object_cbv_heap_desc.NodeMask = 0;
    TIF(device->CreateDescriptorHeap(&object_cbv_heap_desc, IID_PPV_ARGS(&transformed_object_cbv_heap)));

    UINT buffer_size = transformationdata.size() * sizeof(TransformationData);

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_view_desc;
    CD3DX12_CPU_DESCRIPTOR_HANDLE cbv_handle(transformed_object_cbv_heap->GetCPUDescriptorHandleForHeapStart());
    for (int i = 0; i < frame_buffer_count; i++) {
        
        if (transformed_object_constant_buffers[i]) {
            transformed_object_constant_buffers[i]->Unmap(0, NULL);
            transformed_object_constant_data_begins[i] = nullptr;
            transformed_object_constant_buffers[i].Reset();
        }
        
        TIF(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(buffer_size),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&transformed_object_constant_buffers[i])));

        D3D12_RANGE read_range{ 0,0 };
        TIF(transformed_object_constant_buffers[i]->Map(0, &read_range, &transformed_object_constant_data_begins[i]));

        for (int j = 0; j < transformationdata.size(); j++) {
            D3D12_GPU_VIRTUAL_ADDRESS cbaddress = transformed_object_constant_buffers[i]->GetGPUVirtualAddress();
            cbv_view_desc.BufferLocation = cbaddress + (j * sizeof(TransformationData));
            cbv_view_desc.SizeInBytes = sizeof(TransformationData);

            device->CreateConstantBufferView(&cbv_view_desc, cbv_handle);
            cbv_handle.Offset(1, cbv_srv_uav_inc_size);
        }
    }
}

void App::InitCamera() {
    RECT visualizer_window_client_rect;
    GetClientRect(visualizer_window, &visualizer_window_client_rect);
    UINT visualizer_window_client_width = visualizer_window_client_rect.right - visualizer_window_client_rect.left;
    UINT visualizer_window_client_height = visualizer_window_client_rect.bottom - visualizer_window_client_rect.top;

    camera.SetWidth(2.0f);
    camera.SetAspectRatio(visualizer_window_client_width / (float)visualizer_window_client_height);
    camera.SetPos({ 0.f, 0.f, -1.f });

    /* Create CBV desc heap */
    D3D12_DESCRIPTOR_HEAP_DESC camera_cbv_heap_desc;
    camera_cbv_heap_desc.NumDescriptors = frame_buffer_count;
    camera_cbv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    camera_cbv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    camera_cbv_heap_desc.NodeMask = 0;
    TIF(device->CreateDescriptorHeap(&camera_cbv_heap_desc, IID_PPV_ARGS(&camera_cbv_heap)));
    
    /* Create Resources, Copy Data To Them, and Store CBV's in the CBV buffer */
    UINT camera_data_buffers_size = sizeof(CameraData);
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbv_view_desc;
    CD3DX12_CPU_DESCRIPTOR_HANDLE cbv_handle(camera_cbv_heap->GetCPUDescriptorHandleForHeapStart());
    for (int i = 0; i < frame_buffer_count; i++) {
        TIF(device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
            D3D12_HEAP_FLAG_NONE,
            &CD3DX12_RESOURCE_DESC::Buffer(camera_data_buffers_size),
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&camera_data_buffers[i])));

        D3D12_RANGE read_range{ 0,0 };
        TIF(camera_data_buffers[i]->Map(0, &read_range, reinterpret_cast<void**>(&camera_data_begins[i])));
        memcpy(camera_data_begins[i], &camera_cpu_data, sizeof(camera_cpu_data));

        cbv_view_desc.BufferLocation = camera_data_buffers[i]->GetGPUVirtualAddress();
        cbv_view_desc.SizeInBytes = camera_data_buffers_size;

        device->CreateConstantBufferView(&cbv_view_desc, cbv_handle);
        cbv_handle.Offset(1, cbv_srv_uav_inc_size);
    }

}

void App::OnMouseMove(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
    camera.OnMouseMove(t_wparam);
}

void App::OnLMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
    camera.OnMouseDown(t_wparam);
}

void App::OnLMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
    camera.OnMouseUp(t_wparam);
}

void App::OnRMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
    camera.OnMouseDown(t_wparam);
}

void App::OnRMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
    camera.OnMouseUp(t_wparam);
}

void App::OnMMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {

}

void App::OnMMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
}

void App::OnMouseScroll(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {

}
