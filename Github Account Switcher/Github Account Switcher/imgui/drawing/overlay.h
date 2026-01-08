#pragma once
#include "variables.h"

#include <imgui_internal.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <dwmapi.h>
#include <algorithm>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <DirectXMath.h>

namespace gui
{
	void Create();
	void DrawGui();
}
