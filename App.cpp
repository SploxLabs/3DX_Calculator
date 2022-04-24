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
    return DefWindowProc(t_hwnd, t_msg, t_wparam, t_lparam);
}

LRESULT App::OnVisualizerWindowEvent(HWND t_hwnd, UINT t_msg, WPARAM t_wparam, LPARAM t_lparam) {
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

	int visualizer_to_calculator_window_width_ratio = 3;

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
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
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
