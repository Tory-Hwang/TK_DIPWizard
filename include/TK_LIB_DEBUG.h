#ifndef __TK_LIB_DEBUG_H__
#define __TK_LIB_DEBUG_H__


#include <cstdio>
#include <iostream>

#if 1

/* just a helper for code location */
#define LOC std::cout << "debug:" << __FILE__ << ":" << __LINE__ << " ";

/* macro using var args */
#define DEBUG_PRINT(fmt,...) LOC OutputDebugStringA(fmt,__VA_ARGS__);

/* macro for general debug print statements. */
#define DEBUG_VAL(text) LOC std::cout << text << std::endl;

/* macro that dumps a variable name and its actual value */
#define DEBUG_VAR(text) LOC std::cout << (#text) << "=" << text << std::endl;

#else

/* when debug isn't defined all the macro calls do absolutely nothing */
#define DEBUG_PRINT(fmt,...)
#define DEBUG_VAL(text)
#define DEBUG_VAR(text)

#endif
#endif