#pragma once

#ifdef _WIN32
#define SHARED_EXPORT
#define CALLING_CONVENTION __cdecl
#else
#define SHARED_EXPORT
#define CALLING_CONVENTION
#endif