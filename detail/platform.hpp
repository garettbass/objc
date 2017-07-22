#pragma once

//------------------------------------------------------------------------------

#ifndef OBJC_DEBUG
    #if defined(DEBUG) || defined(_DEBUG)
        #define OBJC_DEBUG 1
    #elif defined(__GNUC__) && !defined(__OPTIMIZE)
        #define OBJC_DEBUG 1
    #endif
#endif

//------------------------------------------------------------------------------
// OBJC_CONCAT(x,y)
//
// Concatenate the macro-expanded results of arguments x and y.
//
#define  OBJC_CONCAT(x,y) _OBJC_CONCAT(x,y)
#define _OBJC_CONCAT(x,y) x##y

//------------------------------------------------------------------------------
// OBJC_QUOTE(x)
//
// Surround the macro-expanded result of argument x in quotes.
//
#define  OBJC_QUOTE(x) _OBJC_QUOTE(x)
#define _OBJC_QUOTE(x) #x

//------------------------------------------------------------------------------

#if defined(__clang__)

    #define OBJC_CXX_ID     clang
    #define OBJC_CXX_CLANG  (__clang_major__ * 10 + __clang_minor__)
    #define OBJC_CXX_NAME   "Clang " __clang_version__

#elif defined(__GNUC__)

    #define OBJC_CXX_ID    gcc
    #define OBJC_CXX_GCC   (__GNUC__ * 10 + __GNUC_MINOR__)
    #define OBJC_CXX_NAME  "GCC " __VERSION__

#elif defined(_MSC_VER)

    #define OBJC_CXX_ID  msvc
    #if (_MSC_VER >= 1900)
        #define OBJC_CXX_MSVC 14
        #define OBJC_CXX_NAME "Microsoft Visual C++ 14 (2015)"
    #elif (_MSC_VER >= 1800)
        #define OBJC_CXX_MSVC 12
        #define OBJC_CXX_NAME "Microsoft Visual C++ 12 (2013)"
    #elif (_MSC_VER >= 1700)
        #define OBJC_CXX_MSVC 11
        #define OBJC_CXX_NAME "Microsoft Visual C++ 11 (2012)"
    #elif (_MSC_VER >= 1600)
        #define OBJC_CXX_MSVC 10
        #define OBJC_CXX_NAME "Microsoft Visual C++ 10 (2010)"
    #elif (_MSC_VER == 1500)
        #define OBJC_CXX_MSVC 9
        #define OBJC_CXX_NAME "Microsoft Visual C++ 9 (2008)"
    #elif (_MSC_VER == 1400)
        #define OBJC_CXX_MSVC 8
        #define OBJC_CXX_NAME "Microsoft Visual C++ 8 (2005)"
    #endif

    // warning C4201: nonstandard extension used: nameless struct/union
    #pragma warning(disable:4201)

#else

    #error "unrecognized compiler"

#endif

#define OBJC_CXX_FILE(file) OBJC_QUOTE(./cxx/OBJC_CXX_ID/file)

//------------------------------------------------------------------------------

#if defined(__ANDROID__)

    #define OBJC_OS_ANDROID    1
    #define OBJC_OS_ID         android
    #define OBJC_OS_NAME       "Android"

#elif defined(__linux)

    #define OBJC_OS_LINUX      1
    #define OBJC_OS_ID         linux
    #define OBJC_OS_NAME       "Linux"

#elif defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)

    #define OBJC_OS_APPLE      1
    #define OBJC_OS_IOS        1
    #define OBJC_OS_ID         ios
    #define OBJC_OS_NAME       "iOS"

#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)

    #define OBJC_OS_APPLE      1
    #define OBJC_OS_MACOS      1
    #define OBJC_OS_ID         macos
    #define OBJC_OS_NAME       "macOS"

#elif __cplusplus_winrt

    #define OBJC_OS_MICROSOFT  1
    #define OBJC_OS_WINRT      1
    #define OBJC_OS_ID         winrt
    #define OBJC_OS_NAME       "WinRT"

#elif defined(_WIN32)

    #define OBJC_OS_MICROSOFT  1
    #define OBJC_OS_WINDOWS    1
    #define OBJC_OS_ID         windows
    #define OBJC_OS_NAME       "Windows"

#else

    #error "unsupported platform"

#endif

#define OBJC_OS_FILE(file) OBJC_QUOTE(./cxx/OBJC_OS_ID/file)

//------------------------------------------------------------------------------

#if defined(__aarch64__)

    // ARM 64
    #define OBJC_CPU_ARM    1
    #define OBJC_CPU_ARM_64 1
    #define OBJC_CPU_ID     arm_64
    #define OBJC_CPU_NAME   "ARM64"
    #define OBJC_CPU_BITS   64

#elif \
    defined(ARM)     || \
    defined(_ARM)    || \
    defined(__arm)   || \
    defined(__arm__)

    // ARM 32
    #define OBJC_CPU_ARM    1
    #define OBJC_CPU_ARM_32 1
    #define OBJC_CPU_ID     arm_32
    #define OBJC_CPU_NAME   "ARM"
    #define OBJC_CPU_BITS   32

#elif \
    defined(__powerpc64__) || \
    defined(__ppc64__)

    // PPC 64
    #define OBJC_CPU_PPC    1
    #define OBJC_CPU_PPC_64 1
    #define OBJC_CPU_ID     ppc_64
    #define OBJC_CPU_NAME   "PowerPC 64"
    #define OBJC_CPU_BITS   64

#elif \
    defined(__POWERPC__) || \
    defined(__PPC__)     || \
    defined(__powerpc__) || \
    defined(__ppc__)     || \
    defined(_POWER)      || \
    defined(powerpc)

    // PPC 32
    #define OBJC_CPU_PPC    1
    #define OBJC_CPU_PPC_32 1
    #define OBJC_CPU_ID     ppc_32
    #define OBJC_CPU_NAME   "PowerPC"
    #define OBJC_CPU_BITS   32

#elif \
    defined(_M_X64)     || \
    defined(_M_AMD64)   || \
    defined(__x86_64__)

    // x86-64
    #define OBJC_CPU_X86    1
    #define OBJC_CPU_X86_64 1
    #define OBJC_CPU_ID     x86_64
    #define OBJC_CPU_NAME   "x86-64"
    #define OBJC_CPU_BITS   64

#elif \
    defined(_M_IX86)  || \
    defined(__386__)  || \
    defined(__i386__) || \
    defined(__X86__)  || \
    defined(i386)

    // x86
    #define OBJC_CPU_X86    1
    #define OBJC_CPU_X86_32 1
    #define OBJC_CPU_ID     x86_32
    #define OBJC_CPU_NAME   "x86"
    #define OBJC_CPU_BITS   32

#else

    #error "unrecognized CPU architecture"

#endif

#define OBJC_CPU_FILE(file) OBJC_QUOTE(./cxx/OBJC_CPU_ID/file)

//------------------------------------------------------------------------------

#if OBJC_CPU_X86 || OBJC_CPU_X86_64 || (OBJC_CPU_ARM && !__BIG_ENDIAN__)

    #define OBJC_ENDIAN_LE     0x01020304u
    #define OBJC_ENDIAN_BE     0
    #define OBJC_ENDIAN        OBJC_ENDIAN_LE
    #define OBJC_ENDIAN_NAME   "little endian"

#elif OBJC_CPU_PPC

    #define OBJC_ENDIAN_LE     0
    #define OBJC_ENDIAN_BE     0x04030201u
    #define OBJC_ENDIAN        OBJC_ENDIAN_BE
    #define OBJC_ENDIAN_NAME   "big endian"

#else

    #error "unrecognized endianness"

#endif
