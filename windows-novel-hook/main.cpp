#include <cmath>

#include "dumpi.h"
#include "Http.h"
#include "HtmlParser.h"
#include "CodeParser.h"

BOOL ResolveDocsLinks(Http* http, std::string symbol, std::vector<std::string>& buf)
{
	std::string url = http->ConstructURL(symbol);
	std::string response = http->MakeRequest(url);

	nlohmann::json jsonResponse = nlohmann::json::parse(response);

    if (!jsonResponse.contains("results") || jsonResponse["results"].empty()) {
		printf("No documentation found for %s\n", symbol.c_str());
		return FALSE;
	}

    for (auto& result : jsonResponse["results"]) {
		buf.push_back(result["url"].get<std::string>());
	}

	return TRUE;
}

void RenderProgressBar(int progress, int total, const char* currentTask)
{
    int barWidth = 40;
    float progressRatio = (float)progress / total;
    float pos = std::round(barWidth * progressRatio);

    printf("\r%*s\r", barWidth + 50, ""); // Clear the line

    printf("\r[");
    for (int i = 0; i < barWidth; ++i) {
        if (i <= pos) printf("=");
        else printf(" ");
    }
    printf("] %d%% | (%i/%i) | %s", (int)(progressRatio * 100.0), progress, total, currentTask);

    fflush(stdout);
}

int main(int argc, char** argv)
{    
    if (argc < 2) {
        printf("Usage: %s <path>\n", argv[0]);
        return 1;
    }

    // Convert the path from argv to PCHAR
    PCHAR path = argv[1];

    std::vector<ImportedFunction*> importedFunctions;

    if (!DimpFile(path, importedFunctions))
        return 1;

    printf("Resolving function signatures for %s ...\n", path);

    std::vector<std::string> functionSignatures;
    Http* http = new Http();

    int count = 0;
    for (const auto& func : importedFunctions) {
        count++;

        RenderProgressBar(static_cast<int>(count), static_cast<int>(importedFunctions.size()), func->Symbol.c_str());
		//printf("\nAttempting to resolve function signature for %s ...\n", func->Symbol.c_str());
        //printf("Fetching documentation ... \n");

        std::vector<std::string> links;
        if (!ResolveDocsLinks(http, func->Symbol, links)) continue;
        //printf("Found %zd documentation links\n", links.size());

        //printf("Fetching function signature ... \n");
        
        bool found = false;
        for (const auto& link : links) {
            std::string html = http->MakeRequest(link);
            std::string codeElement = GetFirstCodeElement(html);

            if (!codeElement.empty()) {
                FunctionSignature signature = ParseFunctionSignature(codeElement);

                if (signature.functionName != func->Symbol) {
					//printf("Function name mismatch.\n");
					continue;
				}

                functionSignatures.push_back(codeElement);

                //printf("Resolved function signature for %s!\n", signature.functionName.c_str());
                found = true;
                break; // We only need the first code snippet
            }
        }
	}

    double resolved = (double)functionSignatures.size() / importedFunctions.size() * 100;
    printf("\nResolved %zd out of %zd function signatures (%.2f%%)\n", functionSignatures.size(), importedFunctions.size(), resolved);

    return 0;
}