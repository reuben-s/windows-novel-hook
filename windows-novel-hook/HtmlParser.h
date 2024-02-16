#pragma once

#include <gumbo.h>
#include <string>
#include <vector>

std::string GetFirstCodeElement(const std::string& html);
bool ExtractCodeElements(GumboNode* node, std::string& codeElement);