#include <iostream>
#include "../github/github.h"
#include "../imgui/drawing/overlay.h"

using namespace github;
using namespace github::filesystem;
using namespace github::account;
using namespace github::program;
using namespace std::filesystem;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	gui::Create();
}