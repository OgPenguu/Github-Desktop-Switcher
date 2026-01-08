#include "github.h"

HANDLE GetGithubHandle()
{
	DWORD processId;
	GetWindowThreadProcessId(FindWindowA(0, "Github Desktop"), &processId);

	return OpenProcess(PROCESS_ALL_ACCESS, 0, processId);
}

void github::program::Terminate()
{
	HANDLE github = GetGithubHandle();
	TerminateProcess(github, 0);
}
void github::program::Start()
{
	auto githubBinaries = filesystem::GetGithubProgramFolder() / "GitHubDesktop.exe";

	std::string cmd = "start \"\" \"" + githubBinaries.string() + "\"";
	system(cmd.c_str());
}
void github::program::Restart()
{
	Terminate();
	Sleep(1500);
	Start();
}