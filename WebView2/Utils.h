/*
** Copyright (C) 2025 nstechbytes. All rights reserved.
*/

#pragma once

#include <Windows.h>
#include <string>
#include "Ini/SimpleIni.h"

// String utilities
std::wstring ToLower(std::wstring s);
std::wstring Utf8ToWstring(const char* data, int len);

// INI file utilities
bool ParseBool(const wchar_t* value);
bool GetIniBool(CSimpleIniW& ini, bool& dirty, const wchar_t* section, const wchar_t* key, bool def);
std::wstring GetIniString(CSimpleIniW& ini, bool& dirty, const wchar_t* section, const wchar_t* key, const wchar_t* def, bool forceDefault = false);
