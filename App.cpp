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
            Sleep(100);
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
    }


    default:
        break;
    }
    
    
    return DefWindowProc(t_hwnd, t_msg, t_wparam, t_lparam);
}

void App::OnUpdate() {
}

void App::OnRender() {
}

void App::OnDestroy() {
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

    vector<XMFLOAT3> test_triangle_verts = {
        {0,0,0},
        {0,1,0},
        {1,1,0}
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
    /* Create Depth Stencil View Heap */
    D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc;
    dsv_heap_desc.NumDescriptors = 1;
    dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsv_heap_desc.NodeMask = 0;
    TIF(device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(dsv_heap.GetAddressOf())));

    /* Create Depth Stencil Buffer */
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

XMFLOAT4X4 App::GetIdentity4x4() {
    return XMFLOAT4X4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

void App::RecreateTransformedObjectVertexBuffersFor(vector<XMFLOAT3> verts) {

}

void RecreateTransformedObjectIndexBuffersFor(vector<uint32_t> indicies) {

}

void App::RecreateTransformObjectConstantBuffersFor(vector<TransformationData>) {

}

void App::InitCamera() {


}

void App::OnMouseMove(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
}

void App::OnLMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
}

void App::OnLMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
}

void App::OnRMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
}

void App::OnRMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
}

void App::OnMMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
}

void App::OnMMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
}

void App::OnMouseScroll(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
}
