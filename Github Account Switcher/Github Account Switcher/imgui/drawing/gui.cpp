#include "overlay.h"
#include <algorithm>

ImVec2 GetCursorPosition()
{
	ImVec2 out{};

	POINT p;
	GetCursorPos(&p);
	out.x = (float)p.x;
	out.y = (float)p.y;

	return out;
}

void gui::DrawGui()
{
	static bool open = true;

	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Hello World", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	{
		// Dragging
		{
			static bool dragging = false;
			static ImVec2 dragStartMouse;
			static ImVec2 dragStartWin;

			if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemActive())
			{
				dragging = true;

				dragStartMouse = GetCursorPosition();

				RECT rect;
				int x, y;
				if (GetWindowRect(hwnd, &rect))
				{
					x = rect.left;
					y = rect.top; 
				}
				dragStartWin = ImVec2((float)x, (float)y);
			}

			if (dragging && ImGui::IsMouseDown(0))
			{
				ImVec2 cur = GetCursorPosition();
				ImVec2 delta = ImVec2(cur.x - dragStartMouse.x,
					cur.y - dragStartMouse.y);

				gui::x = std::clamp((dragStartWin.x + delta.x), 0.0f, gui::screenSize.x - gui::width);
				gui::y = std::clamp((dragStartWin.y + delta.y), 0.0f, gui::screenSize.y - gui::height);
				SetWindowPos(hwnd, nullptr, (int)gui::x, (int)gui::y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
			}

			if (dragging && !ImGui::IsMouseDown(0))
				dragging = false;
		}

	}
	ImGui::End();

	if (!open)
	{
		Sleep(500); // letting any operation finish?
		ExitProcess(0);
	}
}