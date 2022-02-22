/*! @file   dllmain_win.cpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   09-02-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.0
 */

// Include: Pulsar
#include "pulsar/pulsar.hpp"

// Include: Windows
#ifdef PF_OS_WINDOWS
#	include <windows.h>

BOOL APIENTRY DllMain(
		HANDLE __module,
		DWORD __reason,
		LPVOID __reserved)
{
	std::ignore = __reserved;

	// TODO: Security
	switch (__reason)
	{
		case DLL_PROCESS_ATTACH:
			break;

		case DLL_PROCESS_DETACH:
			break;

		case DLL_THREAD_ATTACH:
			break;

		case DLL_THREAD_DETACH:
			break;
	}

	return TRUE;
}

#endif // PF_OS_WINDOWS