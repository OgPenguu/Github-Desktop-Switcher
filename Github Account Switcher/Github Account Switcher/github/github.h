#pragma once
#include <filesystem>
#include <string>
#include <Windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <cpr/cpr.h>
#include <cpr/util.h>
#include <nlohmann/json.hpp>

namespace github
{
	namespace account
	{
		std::string GetUsername();
		std::string GetEmail();
		std::vector<uint8_t> GetAvatar();
	}
	namespace filesystem
	{
		std::filesystem::path GetGithubSessionFolder();
		std::filesystem::path GetGithubProgramFolder();
		std::string ReadFile(std::string path);

		bool ReplaceGithubFolder(std::filesystem::path sourceFolder);
		bool CopyGithubFolder(std::filesystem::path output);
		bool DeleteGithubFolder();
	}
	namespace program
	{
		void Terminate();
		void Start();
		void Restart();
	}
}