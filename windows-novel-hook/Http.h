#pragma once

#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <windows.h>
#include <string>
#include <stdio.h>

class Http
{
public:
	Http();
	~Http();

	std::string ConstructURL(std::string symbol);
	std::string MakeRequest(const std::string& url);
private:
	CURL* curl;
	static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
};