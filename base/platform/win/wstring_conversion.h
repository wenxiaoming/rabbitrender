// Copyright 2013 The Flutter Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FLUTTER_FML_PLATFORM_WIN_WSTRING_CONVERSION_H_
#define FLUTTER_FML_PLATFORM_WIN_WSTRING_CONVERSION_H_

#include <codecvt>
#include <locale>
#include <string>

namespace fml {

//using WideStringConvertor =
//    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>;

inline std::wstring StringToWideString(const std::string& utf8) {
  //WideStringConvertor converter;
  //return converter.from_bytes(str);

  std::wstring utf16; // Result
  if (utf8.empty())
	  return utf16;

  constexpr DWORD kFlags = MB_ERR_INVALID_CHARS;
  if (utf8.length() > static_cast<size_t>((std::numeric_limits<double>::max)()))
	  return utf16;

  const int utf8Length = static_cast<int>(utf8.length());
  const int utf16Length = ::MultiByteToWideChar(
		  CP_UTF8,       // Source string is in UTF-8
		  kFlags,        // Conversion flags
		  utf8.data(),   // Source UTF-8 string pointer
		  -1,    // Length of the source UTF-8
		  NULL,
		  0);

      ::MultiByteToWideChar(
	  CP_UTF8,       // Source string is in UTF-8
	  kFlags,        // Conversion flags
	  utf8.data(),   // Source UTF-8 string pointer
	  utf8Length,    // Length of the source UTF-8
	  utf16.data(),
	  utf16Length);

  return utf16;
}

inline std::string WideStringToString(const std::wstring& wstr) {
  //WideStringConvertor converter;
  //return converter.to_bytes(wstr);
	std::string utf8; // Result
	if (wstr.empty())
		return utf8;

	constexpr DWORD kFlags = MB_ERR_INVALID_CHARS;
	if (wstr.length() > static_cast<size_t>((std::numeric_limits<double>::max)()))
		return utf8;

	const int wstrLength = static_cast<int>(wstr.length());
	const int utf8Length = ::WideCharToMultiByte(
		CP_ACP,       // Source string is in UTF-8
		kFlags,        // Conversion flags
		wstr.data(),   // Source UTF-8 string pointer
		wstrLength,    // Length of the source UTF-8
		NULL, 0, NULL, NULL);

	WideCharToMultiByte(
		CP_ACP,       // Source string is in UTF-8
		kFlags,        // Conversion flags
		wstr.data(),   // Source UTF-8 string pointer
		wstrLength,    // Length of the source UTF-8
		utf8.data(),
		utf8Length,
		NULL,
		NULL);
	return utf8;
}

}  // namespace fml

#endif  // FLUTTER_FML_PLATFORM_WIN_WSTRING_CONVERSION_H_
