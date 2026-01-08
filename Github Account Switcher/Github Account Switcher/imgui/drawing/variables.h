#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <imgui.h>

namespace gui
{
	inline ID3D11Device* device = nullptr;
	inline ID3D11DeviceContext* context = nullptr;
	inline IDXGISwapChain* g_pSwapChain = nullptr;
	inline UINT                     g_ResizeWidth = 0, g_ResizeHeight = 0;
	inline UINT                     a = 0, b = 0;
	inline ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
	inline UINT width = 600, height = 350;
	inline UINT x = 0, y = 0;

	inline ImVec2 screenSize;

	inline HWND hwnd;
}