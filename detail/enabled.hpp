#pragma once
#ifndef OBJC_ENABLED
    #if defined(__GNUC__) && defined(__clang__)
        #if defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__)
            #define OBJC_ENABLED 1
        #elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
            #define OBJC_ENABLED 1
        #endif
    #endif
#endif
