#include "overlay.h"
#include "../../github/github.h"
#include <filesystem>
#include <d3d11.h>

ImVec2 GetCursorPosition()
{
	ImVec2 out{};

	POINT p;
	GetCursorPos(&p);
	out.x = (float)p.x;
	out.y = (float)p.y;

	return out;
}

std::vector<std::filesystem::path> GetAccounts()
{
	std::filesystem::path accountsPath = std::filesystem::current_path() / "accounts";
	std::vector<std::filesystem::path> accounts;

	if (std::filesystem::exists(accountsPath) && std::filesystem::is_directory(accountsPath))
	{
		for (const auto& entry : std::filesystem::directory_iterator(accountsPath))
		{
			if (entry.is_directory())
			{
				accounts.push_back(entry.path());
			}
		}
	}

	return accounts;
}

std::string GetAccountName(const std::filesystem::path& accountPath)
{
	return accountPath.filename().string();
}

std::vector<uint8_t> LoadAvatar(const std::filesystem::path& accountPath)
{
	// For now, try to get avatar from current account logic, but we might need to parse saved data
	// This is a placeholder - we'll need to implement proper avatar loading from saved accounts
	return github::account::GetAvatar(""); // Empty string for now, need to get email from saved data
}

void SwitchToAccount(const std::filesystem::path& accountPath)
{
	github::program::Terminate();
	Sleep(1000); // Wait for termination

	if (github::filesystem::ReplaceGithubFolder(accountPath / "GitHub Desktop"))
	{
		github::program::Start();
	}
}

void SaveCurrentAccount()
{
	std::filesystem::path accountsPath = std::filesystem::current_path() / "accounts";
	std::string username = github::account::GetUsername();

	if (!username.empty())
	{
		std::filesystem::path accountPath = accountsPath / username;
		github::filesystem::CopyGithubFolder(accountPath);
	}
}

void DeleteAccount(const std::filesystem::path& accountPath)
{
	std::filesystem::remove_all(accountPath);
}

void gui::DrawGui()
{
	static bool open = true;

	ImGui::SetNextWindowSize(ImVec2(width, height));
	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("GitHub Account Switcher", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
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

		// Account management buttons
		if (ImGui::Button("Save Current Account"))
		{
			SaveCurrentAccount();
		}

		ImGui::Separator();

		// Display accounts
		auto accounts = GetAccounts();
		int buttonsPerRow = 4;
		int buttonIndex = 0;

		for (const auto& account : accounts)
		{
			std::string accountName = GetAccountName(account);

			if (buttonIndex % buttonsPerRow != 0)
				ImGui::SameLine();

			// Placeholder for avatar image button
			// For now, using text button until we implement proper image loading
			if (ImGui::Button(accountName.c_str(), ImVec2(64, 64)))
			{
				SwitchToAccount(account);
			}

			// Right-click context menu for delete
			if (ImGui::BeginPopupContextItem(accountName.c_str()))
			{
				if (ImGui::MenuItem("Delete Account"))
				{
					DeleteAccount(account);
				}
				ImGui::EndPopup();
			}

			buttonIndex++;
		}
	}
	ImGui::End();

	if (!open)
	{
		Sleep(500); // letting any operation finish?
		ExitProcess(0);
	}
}
