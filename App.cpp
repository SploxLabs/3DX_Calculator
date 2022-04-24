#include "pch.h"
#include "App.h"

App* App::self = nullptr;

App::App() :
	calculator_window(nullptr),
	visualizer_window(nullptr) 
{
	self = this;
}

void App::OnInit() {
	InitWindows();
	InitDX12();
}

void App::InitWindows() {

}

void App::InitDX12() {

}
