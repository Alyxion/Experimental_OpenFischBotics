#pragma once

#ifdef _WIN32
#ifdef UNDER_CE

#pragma once

#pragma comment(linker, "/nodefaultlib:libc.lib")
#pragma comment(linker, "/nodefaultlib:libcd.lib")

#define WINVER _WIN32_WCE

#include <ceconfig.h>
#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#define SHELL_AYGSHELL
#endif

#ifdef _CE_DCOM
#define _ATL_APARTMENT_THREADED
#endif

#include <aygshell.h>
#pragma comment(lib, "aygshell.lib")


#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#if defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP)
#ifndef _DEVICE_RESOLUTION_AWARE
#define _DEVICE_RESOLUTION_AWARE
#endif
#endif

#ifdef _DEVICE_RESOLUTION_AWARE
#include "DeviceResolutionAware.h"
#endif

#if _WIN32_WCE < 0x500 && ( defined(WIN32_PLATFORM_PSPC) || defined(WIN32_PLATFORM_WFSP) )
#pragma comment(lib, "ccrtrtti.lib")
#ifdef _X86_
#if defined(_DEBUG)
#pragma comment(lib, "libcmtx86d.lib")
#else
#pragma comment(lib, "libcmtx86.lib")
#endif
#endif
#endif

#include <altcecrt.h>

#else

#if !defined(AFX_STDAFX_H__ACED1234_5555_AAAA_BBBB_BCDF1234ACCC__INCLUDED_)
#define AFX_STDAFX_H__ACED1234_5555_AAAA_BBBB_BCDF1234ACCC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_STDAFX_H__ACED1234_5555_AAAA_BBBB_BCDF1234ACCC__INCLUDED_)

#endif
#endif

#if defined(IS_GCC) && defined(WIN32)
#include "windows.h"
#endif

#include "../../../LBNavigator/Aphereon4/Source/AphQUA.h"
#include "Tutorial.h"