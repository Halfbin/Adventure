#pragma once

#ifdef  _MBCS
#   error "_MBCS (multi-byte character set) is defined, but only Unicode is supported"
#endif
#undef  _UNICODE
#define _UNICODE        // For [tchar.h]
#undef  UNICODE
#define UNICODE         // For [windows.h]

#undef  NOMINMAX
#define NOMINMAX        // C++ standard library compatibility
#undef  STRICT
#define STRICT          // C++ type-checking compatibility

#ifndef _WIN32_WINNT
#   define _WIN32_WINNT     _WIN32_WINNT_WINXP  // Minimum version as per 2011/2012.
#endif
#ifdef  _MSC_VER            // Visual C++ only (the SDK is Visual C++ specific).
#   include <SDKDDKVer.h>   // E.g. g++ 4.4.1 does not support this file/scheme.
#endif

#undef  WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
