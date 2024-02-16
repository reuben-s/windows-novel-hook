#include "dumpi.h"
#include <vector>

static CHAR s_szFile[MAX_PATH] = "\0";
static std::vector<ImportedFunction*>* importedFunctions = nullptr; // Global pointer to store imported functions

static BOOL CALLBACK ListFileCallback(_In_opt_ PVOID pContext,
    _In_z_ LPCSTR pszOrigFile,
    _In_z_ LPCSTR pszFile,
    _Outptr_result_maybenull_ LPCSTR* ppszOutFile)
{
    (void)pContext;
    (void)pszFile;

    *ppszOutFile = NULL;

    StringCchCopyA(s_szFile, sizeof(s_szFile), pszOrigFile);

    PCHAR psz;
    if ((psz = strchr(s_szFile, '.')) != NULL) {
        *psz = '\0';
    }
    return TRUE;
}

static BOOL CALLBACK ListSymbolCallback(_In_opt_ PVOID pContext,
    _In_ ULONG nOrigOrdinal,
    _In_ ULONG nOrdinal,
    _Out_ ULONG* pnOutOrdinal,
    _In_opt_z_ LPCSTR pszOrigSymbol,
    _In_opt_z_ LPCSTR pszSymbol,
    _Outptr_result_maybenull_ LPCSTR* ppszOutSymbol)
{
    (void)pContext;
    (void)nOrdinal;
    (void)pszSymbol;

    *ppszOutSymbol = NULL;
    *pnOutOrdinal = 0;

    ImportedFunction* function = new ImportedFunction;
    function->File = std::string(s_szFile);

    if (nOrigOrdinal != 0) {
        function->Symbol = std::to_string(nOrigOrdinal);
    }
    else {
        function->Symbol = pszOrigSymbol;
    }

    importedFunctions->push_back(function); // Add the function to the vector

    return TRUE;
}

BOOL DimpFile(PCHAR pszPath, std::vector<ImportedFunction*>& vector)
{
    BOOL bGood = TRUE;
    HANDLE hOld = INVALID_HANDLE_VALUE;
    PDETOUR_BINARY pBinary = NULL;

    // Initialize the global pointer with the address of the vector passed as an argument
    importedFunctions = &vector;

    // Open the file
    hOld = CreateFileA(pszPath,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hOld == INVALID_HANDLE_VALUE) {
        printf("%s: Failed to open input file with error: %ld\n",
            pszPath, GetLastError());
        return FALSE;
    }

    // Open the Detour binary
    pBinary = DetourBinaryOpen(hOld);
    if (pBinary == NULL) {
        printf("%s: DetourBinaryOpen failed: %ld\n", pszPath, GetLastError());
        CloseHandle(hOld);
        return FALSE;
    }

    // Close the old handle as it is no longer needed
    CloseHandle(hOld);

    // Process the binary
    if (!DetourBinaryEditImports(pBinary,
        NULL,
        NULL,
        ListFileCallback,
        ListSymbolCallback,
        NULL)) {

        printf("%s: DetourBinaryEditImports failed: %ld\n", pszPath, GetLastError());
        bGood = FALSE;
    }

    // Clean up and return
    DetourBinaryClose(pBinary);
    return bGood;
}
