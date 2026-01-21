/*
** Copyright (C) 2025 nstechbytes. All rights reserved.
*/

#pragma once

#include <Windows.h>
#include <WebView2.h>
#include <string>
#include <vector>

// Global flag for extension checking
extern bool g_extensions_checked;

// Extension utility functions
std::vector<std::wstring> GetExtensionsID(const std::wstring& input);
void EnableExtension(ICoreWebView2BrowserExtension* extension, BOOL enable);
void RemoveExtension(void* rm, ICoreWebView2BrowserExtension* extension, const std::wstring& name);
