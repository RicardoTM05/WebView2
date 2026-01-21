/*
** Copyright (C) 2025 nstechbytes. All rights reserved.
*/

#include "Utils.h"
#include <sstream>
#include <iomanip>

// String utilities
std::wstring ToLower(std::wstring s)
{
	if (!s.empty())
	{
		CharLowerBuffW(s.data(), static_cast<DWORD>(s.size()));
	}
	return s;
}

std::wstring Utf8ToWstring(const char* data, int len)
{
	if (len <= 0) return std::wstring();

	// First call to get required buffer size
	int required = MultiByteToWideChar(CP_UTF8, 0, data, len, nullptr, 0);
	if (required == 0) {
		throw std::runtime_error("Utf8ToWstring: MultiByteToWideChar failed (size query)");
	}

	std::wstring out;
	out.resize(required);
	int res = MultiByteToWideChar(CP_UTF8, 0, data, len, &out[0], required);
	if (res == 0) {
		throw std::runtime_error("Utf8ToWstring: MultiByteToWideChar failed (conversion)");
	}
	return out;
}

// INI file utilities
bool ParseBool(const wchar_t* value)
{
	if (!value) return false;

	return _wcsicmp(value, L"true") == 0 ||
		_wcsicmp(value, L"1") == 0 ||
		_wcsicmp(value, L"yes") == 0 ||
		_wcsicmp(value, L"on") == 0;
}

bool GetIniBool(CSimpleIniW& ini, bool& dirty, const wchar_t* section, const wchar_t* key, bool def)
{
	const wchar_t* value = ini.GetValue(section, key, nullptr);
	if (!value)
	{
		ini.SetValue(section, key, def ? L"true" : L"false");
		dirty = true;
		return def;
	}
	return ParseBool(value);
}

std::wstring GetIniString(CSimpleIniW& ini, bool& dirty, const wchar_t* section, const wchar_t* key, const wchar_t* def, bool forceDefault)
{
	const wchar_t* value = ini.GetValue(section, key, nullptr);
	if (!value)
	{
		ini.SetValue(section, key, def);
		dirty = true;
		return def;
	}
	if (forceDefault && std::wcscmp(value, def) != 0)
	{
		ini.SetValue(section, key, def);
		dirty = true;
		return def;
	}
	return value;
}
