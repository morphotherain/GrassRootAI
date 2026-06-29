#pragma once

#include <Windows.h>
#include <string>

inline std::wstring Utf8ToWide(const std::string& text)
{
	if (text.empty())
	{
		return {};
	}
	const int size = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0);
	if (size <= 0)
	{
		return {};
	}
	std::wstring wide(static_cast<size_t>(size), L'\0');
	MultiByteToWideChar(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), wide.data(), size);
	return wide;
}

inline std::string WideToUtf8(const std::wstring& text)
{
	if (text.empty())
	{
		return {};
	}
	const int size = WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), nullptr, 0, nullptr, nullptr);
	if (size <= 0)
	{
		return {};
	}
	std::string utf8(static_cast<size_t>(size), '\0');
	WideCharToMultiByte(CP_UTF8, 0, text.c_str(), static_cast<int>(text.size()), utf8.data(), size, nullptr, nullptr);
	return utf8;
}

inline bool IsFloatEditorFieldId(const std::string& fieldId)
{
	return fieldId.rfind("rect.", 0) == 0
		|| fieldId.rfind("scale.", 0) == 0
		|| fieldId == "rotation"
		|| fieldId == "fontSize";
}
