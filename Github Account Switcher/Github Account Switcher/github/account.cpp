#include "github.h"

std::string GetAccountInfoFile()
{
	return github::filesystem::ReadFile((github::filesystem::GetGithubSessionFolder() / "Local Storage" / "leveldb" / "000003.log").string());
}
std::string ExtractValue(const std::string& data, const std::string& key)
{
	size_t pos = data.find(key);
	if (pos == std::string::npos)
		return {};

	pos += key.length();
	size_t end = data.find('"', pos);
	if (end == std::string::npos)
		return {};

	return data.substr(pos, end - pos);
}

std::string github::account::GetUsername()
{
	auto data = GetAccountInfoFile();
	return ExtractValue(data, "\"login\":\"");
}
std::string github::account::GetEmail()
{
	auto data = GetAccountInfoFile();
	return ExtractValue(data, "\"email\":\"");
}
std::vector<uint8_t> github::account::GetAvatar()
{
	std::string email = GetEmail();
	std::string url_email = cpr::util::urlEncode(email).c_str();
	std::string url = "https://avatars.githubusercontent.com/u/e?email=" + url_email + "&s=64";

	cpr::Response r = cpr::Get(cpr::Url{ url });
	if (r.status_code != 200)
	{
		return {}; 
	}

	std::vector<uint8_t> avatar(r.text.begin(), r.text.end());
	return avatar;
}
