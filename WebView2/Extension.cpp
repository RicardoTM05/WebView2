/*
** Copyright (C) 2025 nstechbytes. All rights reserved.
*/

#include "Extension.h"
#include "Plugin.h"
#include "../API/RainmeterAPI.h"
#include <sstream>

bool g_extensions_checked = false;

std::vector<std::wstring> GetExtensionsID(const std::wstring& input)
{
	std::vector<std::wstring> result;
	std::wstringstream ss(input);
	std::wstring token;

	while (std::getline(ss, token, L',')) {
		token.erase(0, token.find_first_not_of(L" \t"));
		token.erase(token.find_last_not_of(L" \t") + 1);

		if (!token.empty()) {
			result.push_back(token);
		}
	}
	return result;
}

void EnableExtension(ICoreWebView2BrowserExtension* extension, BOOL enable)
{
	extension->Enable(
		enable,
		Callback<ICoreWebView2BrowserExtensionEnableCompletedHandler>(
			[](HRESULT hr) -> HRESULT
			{
				if (FAILED(hr))
					ShowFailure(hr, L"Enable extension failed");
				return S_OK;
			}).Get());
}

void RemoveExtension(void* rm, ICoreWebView2BrowserExtension* extension, const std::wstring& name)
{
	extension->Remove(
		Callback<ICoreWebView2BrowserExtensionRemoveCompletedHandler>(
			[](HRESULT hr) -> HRESULT
			{
				if (FAILED(hr))
					ShowFailure(hr, L"Uninstall extension failed");
				return S_OK;
			}).Get());

	RmLogF(rm, LOG_DEBUG, L"WebView2: \"%s\" extension removed.", name.c_str());
}
