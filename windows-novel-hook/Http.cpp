#include "Http.h"

Http::Http()
{
    curl = curl_easy_init();
}

Http::~Http()
{
	curl_easy_cleanup(curl);
}

std::string Http::ConstructURL(std::string symbol)
{
	char* output = curl_easy_escape(curl, symbol.c_str(), static_cast<int>(symbol.length()));
    if (output) {
		std::string result = "https://learn.microsoft.com/api/search?search=" + std::string(output) + "&locale=en-us&facet=category&facet=products&facet=tags&%24top=10&expandScope=true&includeQuestion=false&applyOperator=false";
		curl_free(output);
		return result;
	}
	return "";
}

size_t Http::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

std::string Http::MakeRequest(const std::string& url)
{
	std::string readBuffer;
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
	CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}
	return readBuffer;
}