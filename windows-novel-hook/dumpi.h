#pragma once

#include <Windows.h>
#include <detours/detours.h>
#include <stdio.h>
#include <strsafe.h>
#include <string>
#include <vector>

struct ImportedFunction
{
    std::string File;
    std::string Symbol;
};

BOOL DimpFile(PCHAR pszPath, std::vector<ImportedFunction*>& vector);