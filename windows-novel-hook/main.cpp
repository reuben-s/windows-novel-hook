#include "dumpi.h"
#include "Http.h"
#include "HtmlParser.h"

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

int main(int argc, char** argv)
{
    // Calculate time taken to resolve function signatures
    

    if (argc < 2) {
        printf("Usage: %s <path>\n", argv[0]);
        return 1;
    }

    // Convert the path from argv to PCHAR
    PCHAR path = argv[1];

    std::vector<ImportedFunction*> importedFunctions;

    if (!DimpFile(path, importedFunctions))
        return 1;

    printf("Found following functions:\n");
    for (const auto& func : importedFunctions) {
        printf("\t%s::%s\n", func->File.c_str(), func->Symbol.c_str()); // Note: Use c_str() to print std::string
    }

    std::vector<std::string> functionSignatures;
    Http* http = new Http();

    int count = 1;
    for (const auto& func : importedFunctions) {

        //if (count > 1) break; // Only process the first function (for now)

		printf("\nAttempting to resolve function signature for %s ...\n", func->Symbol.c_str());
        printf("Fetching documentation ... \n");

        std::vector<std::string> links;
        if (!ResolveDocsLinks(http, func->Symbol, links)) continue;
        printf("Found %zd documentation links\n", links.size());


        printf("Fetching function signature ... \n");
        
        bool found = false;
        for (const auto& link : links) {
            std::string html = http->MakeRequest(link);
            std::string codeElement = GetFirstCodeElement(html);

            if (!codeElement.empty()) {
                functionSignatures.push_back(codeElement);
                printf("Resolved %s function signature!\n", func->Symbol.c_str());
                found = true;
                break; // We only need the first code snippet
            }
        }
        if (!found)
            printf("Failed to resolve %s function signature\n", func->Symbol.c_str());
        else
            count++;
	}

    for (const auto& signature : functionSignatures) {
		printf("\n%s\n", signature.c_str());
	}

    double resolved = (double)functionSignatures.size() / importedFunctions.size() * 100;
    printf("\nResolved %zd out of %zd function signatures (%.2f%%)\n", functionSignatures.size(), importedFunctions.size(), resolved);

    return 0;
}