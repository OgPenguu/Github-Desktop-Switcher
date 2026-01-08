#include "overlay.h"

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void gui::Create()
{
	WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Sheldon", nullptr };
	::RegisterClassExW(&wc);

	hwnd = ::CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		wc.lpszClassName,
		L"Github Account Switcher",
		WS_POPUP,
		gui::x,
		gui::y,
		gui::width,
		gui::height,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	DWORD baseStyle = GetWindowLong(hwnd, GWL_EXSTYLE);

	SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(hwnd, &margin);

	if (!CreateDeviceD3D(hwnd))
	{
		CleanupDeviceD3D();
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);
		return;
	}

	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	::UpdateWindow(hwnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = nullptr;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);

	bool done = false;

	float clearColorAlpha = 0;
	ImVec4 clearColor = ImVec4(35 / 255, 35 / 255, 35 / 255, 0);
	int fadeDirection = 0;

	while (done == false)
	{
		MSG msg;
		while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				done = true;
			}
		}
		if (done == true)
		{
			break;
		}

		if (g_ResizeWidth != 0 && g_ResizeHeight != 0)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
			g_ResizeWidth = g_ResizeHeight = 0;
			CreateRenderTarget();
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		RECT workArea = {};
		SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
		gui::screenSize.x = workArea.right - workArea.left;
		gui::screenSize.y = workArea.bottom - workArea.top;

		DrawGui();

		ImGui::Render();
		float arrayClearColor[4]{};
		arrayClearColor[0] = clearColor.x * clearColorAlpha / 2;
		arrayClearColor[1] = clearColor.y * clearColorAlpha / 2;
		arrayClearColor[2] = clearColor.z * clearColorAlpha / 2;
		arrayClearColor[3] = clearColorAlpha / 2;

		context->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
		context->ClearRenderTargetView(g_mainRenderTargetView, arrayClearColor);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

bool CreateDeviceD3D(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT createDeviceFlags = 0;
	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &gui::g_pSwapChain, &gui::device, &featureLevel, &gui::context);
	if (res == DXGI_ERROR_UNSUPPORTED)
		res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &gui::g_pSwapChain, &gui::device, &featureLevel, &gui::context);
	if (res != S_OK)
		return false;

	CreateRenderTarget();
	return true;
}

void CleanupDeviceD3D()
{
	CleanupRenderTarget();
	if (gui::g_pSwapChain)
	{
		gui::g_pSwapChain->Release(); gui::g_pSwapChain = nullptr;
	}
	if (gui::context)
	{
		gui::context->Release(); gui::context = nullptr;
	}
	if (gui::device)
	{
		gui::device->Release(); gui::device = nullptr;
	}
}

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	gui::g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	gui::device->CreateRenderTargetView(pBackBuffer, nullptr, &gui::g_mainRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (gui::g_mainRenderTargetView)
	{
		gui::g_mainRenderTargetView->Release(); gui::g_mainRenderTargetView = nullptr;
	}
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}

	switch (msg)
	{
		case WM_SIZE: // Queue resize
			if (wParam == SIZE_MINIMIZED)
				return 0;
			gui::g_ResizeWidth = (UINT)LOWORD(lParam);
			gui::g_ResizeHeight = (UINT)HIWORD(lParam);
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
	}

	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
