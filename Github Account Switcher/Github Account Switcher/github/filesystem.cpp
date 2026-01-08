#include "github.h"

namespace fs = std::filesystem;

std::filesystem::path github::filesystem::GetGithubSessionFolder()
{
	const char* appData = std::getenv("APPDATA");
	std::filesystem::path roamingPath = appData;

	return roamingPath / "GitHub Desktop";
}
std::filesystem::path github::filesystem::GetGithubProgramFolder()
{
	const char* appData = std::getenv("LOCALAPPDATA");
	std::filesystem::path roamingPath = appData;

	return roamingPath / "GitHubDesktop";
}
std::string github::filesystem::ReadFile(std::string path)
{
	std::ifstream file(path, std::ios::binary);
	if (!file) return {};

	std::ostringstream ss;
	ss << file.rdbuf();
	return ss.str();
}

bool github::filesystem::ReplaceGithubFolder(std::filesystem::path sourceFolder)
{
	fs::path target = GetGithubSessionFolder();

	if (!fs::exists(sourceFolder) || !fs::is_directory(sourceFolder))
	{
		return false;
	}

	try
	{
		if (fs::exists(target))
			fs::remove_all(target);

		fs::copy(sourceFolder, target, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
		return true;
	}
	catch (const fs::filesystem_error&)
	{
		return false;
	}
}
bool github::filesystem::CopyGithubFolder(std::filesystem::path output)
{
	fs::path source = GetGithubSessionFolder();

	if (!fs::exists(source) || !fs::is_directory(source))
	{
		return false;
	}

	try
	{
		fs::create_directories(output);

		fs::copy(source, output / source.filename(), fs::copy_options::recursive | fs::copy_options::overwrite_existing);
		return true;
	}
	catch (const fs::filesystem_error&)
	{
		return false;
	}
}
bool github::filesystem::DeleteGithubFolder()
{
	namespace fs = std::filesystem;

	fs::path folder = GetGithubSessionFolder();
	if (folder.empty())
	{
		return false;
	}

	try
	{
		if (!fs::exists(folder))
		{
			return true;
		}

		fs::remove_all(folder);
		return true;
	}
	catch (const fs::filesystem_error&)
	{
		return false;
	}
}