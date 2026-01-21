/*
** Copyright (C) 2025 nstechbytes. All rights reserved.
*/

#pragma once

#include <Windows.h>
#include <string>
#include <fstream>

// Path and URI utilities
std::wstring NormalizeUri(const std::wstring& uri);
std::wstring NormalizePath(void* rm, LPCWSTR path);
bool IsFilePathSyntax(LPCWSTR input);

// File reading utilities
std::wstring ReadScriptFile(const std::wstring& path);
