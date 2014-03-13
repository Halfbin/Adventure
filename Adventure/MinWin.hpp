#pragma once

#ifdef  _MBCS
# error "Do not use _MBCS builds. Use _UNICODE instead."
#endif

#pragma warning (push)
#pragma warning (disable: 4005)

#define _UNICODE // tchar.h
#define UNICODE  // windows.h
#define NOMINMAX // ugh
#define STRICT   // just in case
#define WIN32_LEAN_AND_MEAN

#ifndef _WIN32_WINNT
# define _WIN32_WINNT _WIN32_WINNT_WIN7
#endif

#pragma warning (pop)

#ifdef  _MSC_VER
# include <SDKDDKVer.h>
#endif

#include <Windows.h>
