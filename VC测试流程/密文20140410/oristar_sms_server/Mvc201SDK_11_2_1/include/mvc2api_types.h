#ifndef _MVC2API_TYPES_H_
#define _MVC2API_TYPES_H_

#include "mm_types.h"
#include "mm_rc.h"

#ifdef _WIN32
#ifdef MVC2API_EXPORTS
#define MVC2_API __declspec(dllexport)
#else
#define MVC2_API __declspec(dllimport)
#endif
#else
#define MVC2_API
#endif

#endif
