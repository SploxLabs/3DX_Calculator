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
        if (output_window) {
            wchar_t ostr[9999] = {};
            GetWindowText(output_window, ostr, 9999);
            DestroyWindow(output_window);
        }
        output_window = CreateWindowEx(NULL, L"EDIT", str, WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
            current_horizontal_offset + horizontal_spacing, current_vertical_offset + vertical_spacing,
            calculator_client_width - current_horizontal_offset, calculator_client_height - current_vertical_offset,
            calculator_window, NULL, GetModuleHandle(NULL), NULL);
    }

}

void App::InitDX12() {

}

void App::OnMouseMove(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam)
{
}

void App::OnLMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam)
{
}

void App::OnLMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam)
{
}

void App::OnRMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam)
{
}

void App::OnRMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam)
{
}

void App::OnMMouseDown(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam)
{
}

void App::OnMMouseUp(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam)
{
}

void App::OnMouseScroll(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam)
{
}
