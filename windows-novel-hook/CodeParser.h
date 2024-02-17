#pragma once

#include <clang-c/Index.h>
#include <string>
#include <stdio.h>

struct FunctionSignature
{
	std::string resType;
	std::string functionName;
	std::string params;
};

FunctionSignature ParseFunctionSignature(const std::string& signature);