/*
** Copyright (C) 2025 nstechbytes. All rights reserved.
*/

#include "PathUtils.h"
#include "Utils.h"
#include "../API/RainmeterAPI.h"
#include <vector>

// Path and URI utilities
std::wstring NormalizeUri(const std::wstring& uri)
{
	const std::wstring scheme_sep = L"://";
	auto scheme_pos = uri.find(scheme_sep);
	if (scheme_pos == std::wstring::npos)
		return uri;

	const std::wstring scheme = uri.substr(0, scheme_pos);
	const size_t after_scheme = scheme_pos + scheme_sep.length();

	if (scheme == L"file")
	{
		size_t last_slash = uri.find_last_of(L'/');
		if (last_slash != std::wstring::npos)
		{
			return uri.substr(0, last_slash + 1);
		}
		return uri;
	}

	size_t path_start = uri.find(L'/', after_scheme);
	if (path_start == std::wstring::npos)
	{
		return uri + L"/";
	}

	return uri.substr(0, path_start + 1);
}

std::wstring NormalizePath(void* rm, LPCWSTR path)
{
	if (!path || !*path)
		return {};

	std::wstring value = path;

	// Relative path - absolute
	if (value[0] != L'/' && (value.length() < 2 || value[1] != L':'))
	{
		if (LPCWSTR absolutePath = RmPathToAbsolute(rm, value.c_str()))
		{
			value = absolutePath;
		}
	}

	// Normalize slashes
	for (wchar_t& ch : value)
	{
		if (ch == L'\\') ch = L'/';
	}

	// Enforce extension if required
	auto dotPos = value.find_last_of(L'.');
	if (dotPos == std::wstring::npos)
	{
		RmLog(rm, LOG_ERROR, L"Execute: File extension is missing, use '.js'.");
		return {};
	}

	std::wstring extension = value.substr(dotPos);
	for (wchar_t& ch : extension)
		ch = towlower(ch);

	if (_wcsicmp(extension.c_str(), L".js") != 0)
	{
		std::wstring msg = L"Execute: The file extension '";
		msg.append(extension);
		msg += L"' is not supported. Use '.js' extension.";
		RmLog(rm, LOG_ERROR, msg.c_str());
		return {};
	}
	return value;
}

bool IsFilePathSyntax(LPCWSTR input)
{
	if (!input || input[0] == L'\0') return false;

	bool hasExtension = false;
	bool hasSeparator = false;

	const wchar_t* lastDot = nullptr;
	const wchar_t* p = input;

	while (*p) {
		if (wcschr(L"<>:\"|?*();'", *p)) {
			if (!(*p == L':' && p == input + 1)) {
				return false;
			}
		}

		if (*p == L'\\' || *p == L'/') {
			hasSeparator = true;
			lastDot = nullptr;
		}
		else if (*p == L'.') {
			lastDot = p;
		}
		p++;
	}

	if (lastDot != nullptr && lastDot != input) {
		if (!iswspace(*(lastDot + 1)) && *(lastDot + 1) != L'\0') {
			hasExtension = true;
		}
	}

	bool hasSpace = (wcschr(input, L' ') != nullptr);

	if (hasSpace && !hasSeparator) {
		return false;
	}

	return hasSeparator || hasExtension;
}

// File reading utilities
std::wstring ReadScriptFile(const std::wstring& path)
{
	std::ifstream is(path, std::ios::binary);
	if (!is) {
		throw std::runtime_error("Failed to open file");
	}

	// Read all bytes
	std::vector<char> bytes((std::istreambuf_iterator<char>(is)), std::istreambuf_iterator<char>());
	size_t n = bytes.size();
	if (n == 0) return std::wstring();

	const unsigned char* ub = reinterpret_cast<const unsigned char*>(bytes.data());

	// Detect BOMs
	if (n >= 3 && ub[0] == 0xEFu && ub[1] == 0xBBu && ub[2] == 0xBFu) {
		// UTF-8 with BOM -> skip BOM then convert
		return Utf8ToWstring(reinterpret_cast<const char*>(ub + 3), static_cast<int>(n - 3));
	}

	if (n >= 2 && ub[0] == 0xFFu && ub[1] == 0xFEu) {
		// UTF-16 LE with BOM
		std::wstring out;
		out.reserve((n - 2) / 2);
		for (size_t i = 2; i + 1 < n; i += 2) {
			wchar_t ch = static_cast<wchar_t>(ub[i] | (ub[i + 1] << 8));
			out.push_back(ch);
		}
		return out;
	}

	if (n >= 2 && ub[0] == 0xFEu && ub[1] == 0xFFu) {
		// UTF-16 BE with BOM -> swap bytes
		std::wstring out;
		out.reserve((n - 2) / 2);
		for (size_t i = 2; i + 1 < n; i += 2) {
			wchar_t ch = static_cast<wchar_t>((ub[i] << 8) | ub[i + 1]);
			out.push_back(ch);
		}
		return out;
	}

	// No BOM: assume UTF-8 and convert
	return Utf8ToWstring(reinterpret_cast<const char*>(ub), static_cast<int>(n));
}
