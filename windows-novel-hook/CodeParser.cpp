#include "CodeParser.h"

FunctionSignature ParseFunctionSignature(const std::string& code) {
    FunctionSignature fs;

    CXIndex index = clang_createIndex(0, 0);
    CXUnsavedFile unsavedFile = { "temp.cpp", code.c_str(), code.length() };
    CXTranslationUnit unit = clang_parseTranslationUnit(index, "temp.cpp", nullptr, 0, &unsavedFile, 1, CXTranslationUnit_None);
    CXCursor cursor = clang_getTranslationUnitCursor(unit);

    clang_visitChildren(
        cursor,
        [](CXCursor c, CXCursor parent, CXClientData client_data) {
            FunctionSignature* fs = static_cast<FunctionSignature*>(client_data);
            if (clang_getCursorKind(c) == CXCursor_FunctionDecl) {
                CXString name = clang_getCursorSpelling(c);
                fs->functionName = clang_getCString(name);
                clang_disposeString(name);

                CXType returnType = clang_getCursorResultType(c);
                CXString returnTypeSpelling = clang_getTypeSpelling(returnType);
                fs->resType = clang_getCString(returnTypeSpelling);
                clang_disposeString(returnTypeSpelling);

                int numArgs = clang_Cursor_getNumArguments(c);
                for (int i = 0; i < numArgs; ++i) {
                    CXCursor arg = clang_Cursor_getArgument(c, i);
                    CXType argType = clang_getCursorType(arg);
                    CXString argTypeSpelling = clang_getTypeSpelling(argType);
                    if (i > 0) fs->params += ", ";
                    fs->params += clang_getCString(argTypeSpelling);
                    clang_disposeString(argTypeSpelling);
                }
            }
            return CXChildVisit_Recurse;
        },
        &fs);

    clang_disposeTranslationUnit(unit);
    clang_disposeIndex(index);

    return fs;
}